////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2009
//
//    This file is part of Scorched3D.
//
//    Scorched3D is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    Scorched3D is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with Scorched3D; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

#include <server/ServerSyncCheck.h>
#include <server/ServerSimulator.h>
#include <server/ServerState.h>
#include <server/ServerDestinations.h>
#include <server/ScorchedServer.h>
#include <simactions/SyncCheckSimAction.h>
#include <image/ImageFactory.h>
#include <landscapemap/GroundMaps.h>
#include <landscapemap/LandscapeMaps.h>
#include <common/OptionsScorched.h>
#include <common/Logger.h>
#include <common/FileLogger.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <target/Target.h>
#include <target/TargetLife.h>

static FileLogger *syncCheckFileLogger = 0;

static void syncCheckLog(const std::string &message)
{
	if (!syncCheckFileLogger) 
	{
		char buffer[256];
		snprintf(buffer, 256, "SyncCheckLog-%u-", time(0));
		syncCheckFileLogger = new FileLogger(buffer);
	}	

	LoggerInfo info(message.c_str());
	info.setTime();
	syncCheckFileLogger->logMessage(info);
#ifndef S3D_SERVER
	Logger::log(info);
#endif
}

ServerSyncCheck::SyncContext::SyncContext() :
	serverMessage(0)
{
}

ServerSyncCheck::SyncContext::~SyncContext()
{
	delete serverMessage;
	std::map<unsigned int, ComsSyncCheckMessage*>::iterator itor;
	for (itor = clientMessages.begin();
		itor != clientMessages.end();
		itor++)
	{
		delete itor->second;
	}
	clientMessages.clear();
}

ServerSyncCheck *ServerSyncCheck::instance_(0);

ServerSyncCheck *ServerSyncCheck::instance()
{
	if (!instance_) instance_ = new ServerSyncCheck();
	return instance_;
}

ServerSyncCheck::ServerSyncCheck()
{
	ScorchedServer::instance()->getComsMessageHandler().addHandler(
		ComsSyncCheckMessage::ComsSyncCheckMessageType,
		this);
}

ServerSyncCheck::~ServerSyncCheck()
{
}

void ServerSyncCheck::enterState()
{
	lastTime_ = 0;
}

void ServerSyncCheck::simulate()
{
	// Check if we have all messages for each synccheck
	std::map<unsigned int, SyncContext*>::iterator itor;
	for (itor = contexts_.begin();
		itor != contexts_.end();
		itor++)
	{
		SyncContext *context = itor->second;
		if (checkContext(context))
		{
			// Finished with this context
			contexts_.erase(itor);
			delete context;
			break;
		}
	}

	if (ScorchedServer::instance()->getServerState().getState() != ServerState::ServerPlayingState) return;
	if (!ScorchedServer::instance()->getOptionsGame().getAutoSendSyncCheck()) return;
	time_t currentTime = time(0);
	if (currentTime - lastTime_ >= 10)
	{
		// Auto send sync checks
		lastTime_ = currentTime;
		sendSyncCheck();
	}
}

void ServerSyncCheck::sendSyncCheck()
{
	static unsigned int nextSyncId = 0;

	// Tell everyone to send us sync check messages
	SyncCheckSimAction *syncCheckAction = new SyncCheckSimAction(
		++nextSyncId);
	ScorchedServer::instance()->getServerSimulator().
		addSimulatorAction(syncCheckAction);
}

void ServerSyncCheck::sentSyncCheck(unsigned int syncId)
{
	// Create a new sync check context for this sync check
	SyncContext *context = new SyncContext();
	contexts_[syncId] = context;

	// Add a list of all client destinations we have sent this sync too
	std::list<unsigned int> destinations;
	std::map<unsigned int, ServerDestination *>::iterator itor;
	std::map<unsigned int, ServerDestination *> dests = 
		ScorchedServer::instance()->getServerDestinations().getServerDestinations();
	for (itor = dests.begin();
		itor != dests.end();
		itor++)
	{
		ServerDestination *destination = (*itor).second;
		if (destination->getState() == ServerDestination::sFinished)
		{
			context->clientDestinations.insert(destination->getDestinationId());
		}
	}
}

void ServerSyncCheck::addServerSyncCheck(ComsSyncCheckMessage *message)
{
	// Add the server message to this sync check
	std::map<unsigned int, SyncContext*>::iterator itor = contexts_.find(
		message->getSyncId());
	if (itor == contexts_.end())
	{
		Logger::log(S3D::formatStringBuffer(
			"ERROR: Failed to find server sync context %u", message->getSyncId()));
		return;
	}
	SyncContext *context = itor->second;
	context->serverMessage = message;
}

bool ServerSyncCheck::processMessage(
	NetMessage &message,
	const char *messageType,
	NetBufferReader &reader)
{
	ComsSyncCheckMessage *syncMessage = new ComsSyncCheckMessage();
	if (!syncMessage->readMessage(reader)) return false;

	// Add the client message to the list of messages for this sync check
	std::map<unsigned int, SyncContext*>::iterator itor = contexts_.find(
		syncMessage->getSyncId());
	if (itor == contexts_.end())
	{
		Logger::log(S3D::formatStringBuffer(
			"ERROR: Failed to find client sync context %u", syncMessage->getSyncId()));
		return true;
	}
	SyncContext *context = itor->second;
	context->clientMessages[message.getDestinationId()] = syncMessage;

	return true;
}

bool ServerSyncCheck::checkContext(SyncContext *context)
{
	// Check we have server message to compare
	if (context->serverMessage == 0) return false;

	// Check we have all client messages to compare
	// Only care about clients that are still conected
	std::set<unsigned int>::iterator clientDestItor;
	for (clientDestItor = context->clientDestinations.begin();
		clientDestItor != context->clientDestinations.end();
		clientDestItor++)
	{
		unsigned int clientDestinationId = *clientDestItor;
		if (ScorchedServer::instance()->getServerDestinations().
			getDestination(clientDestinationId) != 0)
		{
			if (context->clientMessages.find(clientDestinationId) == 
				context->clientMessages.end())
			{
				return false;
			}
		}
	}

	// All clients and server are ready, compare syncs
	std::map<unsigned int, ComsSyncCheckMessage*>::iterator clientItor;
	for (clientItor = context->clientMessages.begin();
		clientItor != context->clientMessages.end();
		clientItor++)
	{
		unsigned int destinationId = clientItor->first;
		ComsSyncCheckMessage *clientMessage = clientItor->second;
		if (!compareSyncChecks(context->serverMessage, destinationId, clientMessage))
		{
			syncCheckLog(S3D::formatStringBuffer("**** SyncCheck failed",
						destinationId, clientMessage->getSyncId()));
		}
	}

	return true;
}

bool ServerSyncCheck::compareSyncChecks(ComsSyncCheckMessage *server, 
	unsigned int destinationId, ComsSyncCheckMessage *client)
{
	// Check landscape
	if (server->getLandscapeBuffer().getBufferUsed() !=
		client->getLandscapeBuffer().getBufferUsed())
	{
		syncCheckLog(S3D::formatStringBuffer("**** SyncCheck landscape sizes differ, Dest %u Sync %u",
			destinationId, client->getSyncId()));
	}
	else
	{
		if (memcmp(server->getLandscapeBuffer().getBuffer(),
			client->getLandscapeBuffer().getBuffer(),
			server->getLandscapeBuffer().getBufferUsed()) != 0)
		{
			syncCheckLog(S3D::formatStringBuffer("**** SyncCheck landscapes differ, Dest %u Sync %u",
				destinationId, client->getSyncId()));

			HeightMap &map = ScorchedServer::instance()->getLandscapeMaps().
				getGroundMaps().getHeightMap();			
			NetBufferReader serverReader(server->getLandscapeBuffer());
			NetBufferReader clientReader(client->getLandscapeBuffer());
			for (int y=0; y<map.getMapHeight(); y++)
			{
				for (int x=0; x<map.getMapWidth(); x++)
				{
					fixed serverHeight, clientHeight;
					fixed serverNormal, clientNormal;
					if (!serverReader.getFromBuffer(serverHeight)) return false;
					if (!serverReader.getFromBuffer(serverNormal)) return false;
					if (!clientReader.getFromBuffer(clientHeight)) return false;
					if (!clientReader.getFromBuffer(clientNormal)) return false;

					if (serverHeight != clientHeight ||
						serverNormal != clientNormal) 
					{
						syncCheckLog(S3D::formatStringBuffer("*** %i %i %s", 
							serverHeight.getInternal(), clientHeight.getInternal(),
							(serverNormal != clientNormal?"Normal":"")));
					}					
				}
			}
		}
	}

	// Create temporary target/tanks
	static Target *tmpTarget = new Target(0, LangString(),
		ScorchedServer::instance()->getContext());
	tmpTarget->getLife().setLife(0); // Make sure not added to target space
	static Tank *tmpTank = new Tank(
		ScorchedServer::instance()->getContext(),
		0,
		0, 
		LangString(),
		Vector::getNullVector(),
		"",
		"");
	tmpTank->getState().setState(TankState::sDead); // Make sure not added to target space

	// Load client targets
	NetBufferReader clientReader(client->getTargetsBuffer());
	std::map<unsigned int, NetBuffer *> clientTanks;
	unsigned int noClientTanks;
	if (!clientReader.getFromBuffer(noClientTanks)) return false;
	for (unsigned int c=0; c<noClientTanks; c++)
	{
		unsigned int playerId;
		if (!clientReader.getFromBuffer(playerId)) return false;
		bool isTarget;
		if (!clientReader.getFromBuffer(isTarget)) return false;
		NetBuffer *tmpBuffer = new NetBuffer();
		if (!clientReader.getFromBuffer(*tmpBuffer)) return false;
		clientTanks[playerId] = tmpBuffer;
	}

	// Compare server targets
	NetBufferReader serverReader(server->getTargetsBuffer());
	unsigned int noServerTanks;
	if (!serverReader.getFromBuffer(noServerTanks)) return false;
	for (unsigned int c=0; c<noServerTanks; c++)
	{
		unsigned int playerId;
		if (!serverReader.getFromBuffer(playerId)) return false;
		bool isTarget;
		if (!serverReader.getFromBuffer(isTarget)) return false;
		NetBuffer tmpBuffer;
		if (!serverReader.getFromBuffer(tmpBuffer)) return false;
		
		std::map<unsigned int, NetBuffer *>::iterator clientTank =
			clientTanks.find(playerId);
		if (clientTank == clientTanks.end())
		{
			syncCheckLog(S3D::formatStringBuffer("**** SyncCheck cannot find client target %u, Dest %u Sync %u",
				playerId, destinationId, client->getSyncId()));
		}
		else
		{	
			unsigned int minUsed = MIN(tmpBuffer.getBufferUsed(), clientTank->second->getBufferUsed());
			for (unsigned int u=0; u<minUsed; u++)
			{
				if (tmpBuffer.getBuffer()[u] != clientTank->second->getBuffer()[u])
				{
					clientTank->second->setBufferUsed(u);

					syncCheckLog(S3D::formatStringBuffer("**** SyncCheck %s differ %u:%s, Dest %u Sync %u",
						isTarget?"target":"tank",
						playerId, 
						tmpTarget->getCStrName().c_str(), 
						destinationId, client->getSyncId()));

					Logger::addLogger(syncCheckFileLogger);

					NetBufferReader reader(*clientTank->second);
					if (isTarget)
					{
						tmpTarget->readMessage(reader);
						tmpTarget->getLife().setLife(0);// Make sure not added to target space
					}
					else
					{
						tmpTank->readMessage(reader);
						tmpTank->getState().setState(TankState::sDead);// Make sure not added to target space
					}				

					Logger::remLogger(syncCheckFileLogger);
					break;
				}
			}

			delete clientTank->second;
			clientTanks.erase(clientTank);
		}
	}

	// Check all client targets exists on server
	std::map<unsigned int, NetBuffer *>::iterator unfoundItor;
	for (unfoundItor = clientTanks.begin();
		unfoundItor != clientTanks.end();
		unfoundItor ++)
	{
		unsigned int playerId = unfoundItor->first;
		syncCheckLog(S3D::formatStringBuffer("**** SyncCheck cannot find server target %u, Dest %u Sync %u",
			playerId, destinationId, client->getSyncId()));
		delete unfoundItor->second;
	}

	// Final logging
	if (syncCheckFileLogger)
	{
		syncCheckLog(S3D::formatStringBuffer("SyncCheck checked, Dest %u Sync %u", 
			destinationId, client->getSyncId()));
	}
	else
	{
#ifndef S3D_SERVER
		Logger::log(S3D::formatStringBuffer("SyncCheck checked, Dest %u Sync %u", 
			destinationId, client->getSyncId()));
#endif
	}

	return true;
}
