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

#include <server/ServerConnectAuthHandler.h>
#include <server/ServerConnectHandler.h>
#include <server/ServerChannelManager.h>
#include <server/ServerBanned.h>
#include <server/ScorchedServer.h>
#include <server/ScorchedServerUtil.h>
#include <server/ServerCommon.h>
#include <server/ServerSimulator.h>
#include <server/ServerState.h>
#include <engine/SaveGame.h>
#include <tank/TankModelStore.h>
#include <tank/TankColorGenerator.h>
#include <tank/TankContainer.h>
#include <tank/TankModelContainer.h>
#include <tank/TankAvatar.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <tankai/TankAIAdder.h>
#include <tankai/TankAIStrings.h>
#include <common/Defines.h>
#include <common/FileLines.h>
#include <common/OptionsScorched.h>
#include <common/StatsLogger.h>
#include <common/Logger.h>
#include <net/NetInterface.h>
#include <simactions/TankAddSimAction.h>
#include <coms/ComsAddPlayerMessage.h>
#include <coms/ComsConnectAcceptMessage.h>
#include <coms/ComsConnectMessage.h>
#include <coms/ComsMessageSender.h>
#ifndef S3D_SERVER
#include <client/ClientParams.h>
#endif

ServerConnectAuthHandler *ServerConnectAuthHandler::instance_ = 0;

ServerConnectAuthHandler *ServerConnectAuthHandler::instance()
{
	if (!instance_)
	{
		instance_ = new ServerConnectAuthHandler;
	}
	return instance_;
}

ServerConnectAuthHandler::ServerConnectAuthHandler()
{
	ScorchedServer::instance()->getComsMessageHandler().addHandler(
		ComsConnectAuthMessage::ComsConnectAuthMessageType,
		this);
}

ServerConnectAuthHandler::~ServerConnectAuthHandler()
{
}

bool ServerConnectAuthHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType, NetBufferReader &reader)
{
	// Decode the auth connect message
	AuthMessage *authMessage = new AuthMessage();
	authMessage->destinationId = netMessage.getDestinationId();
	authMessage->ipAddress = netMessage.getIpAddress();
	if (!authMessage->message.readMessage(reader))
	{
		ServerCommon::serverLog("Invalid auth message format");
		ServerCommon::kickDestination(netMessage.getDestinationId());
		delete authMessage;
		return true;
	}

	// Record the auth message till later
	authMessages_.push_back(authMessage);
	return true;
}

void ServerConnectAuthHandler::processMessages()
{
	// Check that there are no outstanding tank addition requests
	if (TankAddSimAction::TankAddSimActionCount > 0) return;

	// Only process the new auth requests once there are no more outstanding
	// tank addition requests
	// Just makes some of the checks easier
	while (!authMessages_.empty())
	{
		AuthMessage *message = authMessages_.back();
		processMessageInternal(message->destinationId, message->ipAddress, message->message);
		authMessages_.pop_back();
		delete message;
	}
}

void ServerConnectAuthHandler::processMessageInternal(
	unsigned int destinationId,
	unsigned int ipAddress,
	ComsConnectAuthMessage &message)
{
	// Check for acceptance bassed on standard checks
	if (!ServerConnectHandler::instance()->checkStandardParams(destinationId, ipAddress)) return;

	// Check player availability
	if (message.getNoPlayers() > 
		(unsigned int)
		(ScorchedServer::instance()->getOptionsGame().getNoMaxPlayers() -
		ScorchedServer::instance()->getTankContainer().getNoOfTanks()))
	{
		std::string kickMessage = 
			S3D::formatStringBuffer(
			"--------------------------------------------------\n"
			"This server is full, you cannot join!\n"
			"--------------------------------------------------");
		ServerCommon::serverLog("Server full, kicking");
		ServerCommon::kickDestination(destinationId, kickMessage);
		return;
	}

	// Auth handler, make sure that only prefered players can connect
	// DO FIRST AS THE HANDLER MAY ALTER THE MESSAGE
	ServerAuthHandler *authHandler =
		ScorchedServerUtil::instance()->getAuthHandler();
	if (authHandler)
	{
		std::string resultMessage;
		if (!authHandler->authenticateUser(message, resultMessage))
		{
			std::string kickMessage = 
				S3D::formatStringBuffer(
				"--------------------------------------------------\n"
				"%s"
				"Connection failed.\n"
				"--------------------------------------------------",
				resultMessage.c_str());
			Logger::log(S3D::formatStringBuffer("User failed authentication \"%s\"",
				message.getUserName()));

			ServerCommon::kickDestination(destinationId, kickMessage);			
			return;
		}
	}

	std::string uniqueId = message.getUniqueId();
	if (!uniqueId.c_str()[0]) // No ID
	{
		// Generate the players unique id (if we need to)
		//
		// This will only actualy generate an id when using a persistent
		// stats logger as this is the only time we can be sure of not
		// giving out duplicate ids.
		//
		uniqueId = StatsLogger::instance()->allocateId();
	}
	std::string SUid = message.getSUI(); //request for SUI

	// Get compat ver
	unsigned int compatVer = message.getCompatabilityVer();
	// TODO send back proper version message

	// Check if this unique id has been banned
	// Do after auth handler as this may update uniqueid
	ServerBanned::BannedType type = 
		ScorchedServerUtil::instance()->bannedPlayers.getBanned(uniqueId.c_str(), SUid.c_str());
	if (type == ServerBanned::Banned)
	{
		Logger::log(S3D::formatStringBuffer("Banned uniqueid/suid connection from destination \"%i\"", 
			destinationId));
		ServerCommon::kickDestination(destinationId);
		return;
	}

	// Check that this unique id has not already connected (if unique ids are in use)
	if (!ScorchedServer::instance()->getOptionsGame().getAllowSameUniqueId())
	{
		std::map<unsigned int, Tank *> &playingTanks = 
			ScorchedServer::instance()->getTankContainer().getPlayingTanks();
		std::map<unsigned int, Tank *>::iterator playingItor;
		for (playingItor = playingTanks.begin();
			playingItor != playingTanks.end();
			playingItor++)
		{
			Tank *current = (*playingItor).second;

			if (uniqueId.c_str()[0])
			{
				if (0 == strcmp(current->getUniqueId(), uniqueId.c_str()))
				{
					Logger::log(S3D::formatStringBuffer("Duplicate uniqueid connection from destination \"%i\"", 
						destinationId));
					ServerCommon::kickDestination(destinationId);
					return;
				}
			}
			if (SUid.c_str()[0])
			{
				if (0 == strcmp(current->getSUI(), SUid.c_str()))
				{
					Logger::log(S3D::formatStringBuffer("Duplicate SUI connection from destination \"%i\"", 
						destinationId));
					ServerCommon::kickDestination(destinationId);
					return;
				}
			}
		}
	}

	// Send the connection accepted message to the client
	ComsConnectAcceptMessage acceptMessage(
		destinationId,
		ScorchedServer::instance()->getOptionsGame().getServerName(),
		ScorchedServer::instance()->getOptionsGame().getPublishAddress(),
		uniqueId.c_str());
	// Add the connection png
#ifdef S3D_SERVER
	{
		std::string fileName = 
			S3D::getSettingsFile(S3D::formatStringBuffer("icon-%i.png",
				ScorchedServer::instance()->getOptionsGame().getPortNo()));
		FILE *in = fopen(fileName.c_str(), "rb");
		if (in)
		{
			acceptMessage.getServerPng().reset();
			unsigned char readBuf[512];
			while (unsigned int size = fread(readBuf, sizeof(unsigned char), 512, in))
			{
				acceptMessage.getServerPng().addDataToBuffer(readBuf, size);
			}
			fclose(in);
		}
	}
#endif
	if (!ComsMessageSender::sendToSingleClient(acceptMessage, destinationId))
	{
		Logger::log(S3D::formatStringBuffer(
			"Failed to send accept to client \"%i\"",
			destinationId));
		ServerCommon::kickDestination(destinationId);
		return;
	}

	const char *savedGame = "";
#ifndef S3D_SERVER
	savedGame = ClientParams::instance()->getSaveFile();
#endif

	if (savedGame[0])
	{
		SaveGame::loadTargets(savedGame);
		ScorchedServer::instance()->getServerState().setState(ServerState::ServerNewLevelState);
	}
	else
	{
		// Add all the new tanks
		for (unsigned int i=0; i<message.getNoPlayers(); i++)
		{
			addNextTank(destinationId,
				ipAddress,	
				uniqueId.c_str(),
				SUid.c_str(),
				message.getHostDesc(),
				false);
		}

		// For the single player game
		// Add a spectator that will always remain a spectator
		// this is so if we only have computer players we still
		// send messages to them
	#ifndef S3D_SERVER
		{
			addNextTank(destinationId,
				ipAddress,
				uniqueId.c_str(),
				SUid.c_str(),
				message.getHostDesc(),
				true);
		}
	#endif
	}
}

void ServerConnectAuthHandler::addNextTank(unsigned int destinationId,
	unsigned int ipAddress,
	const char *sentUniqueId,
	const char *sentSUI,
	const char *sentHostDesc,
	bool extraSpectator)
{
	// The player has connected
	Vector color;
	unsigned int tankId = 0;
	LangString playerName;

	if (extraSpectator)
	{
		tankId = TargetID::SPEC_TANK_ID;
		playerName = LANG_STRING("Spectator");
		color = Vector(0.7f, 0.7f, 0.7f);
	}
	else
	{
		playerName = LANG_STRING(TankAIStrings::instance()->getPlayerName());
		color = TankColorGenerator::instance()->getNextColor(
			ScorchedServer::instance()->getTankContainer().getPlayingTanks());
		tankId = TankAIAdder::getNextTankId(
			sentUniqueId,
			ScorchedServer::instance()->getContext());
	}

	// Make sure host desc does not contain \"
	for (char *h=(char *)sentHostDesc; *h; h++) if (*h == '\"') *h=' ';

	// Use the stats name if stats are enabled and the player has one
	std::list<std::string> aliases  = 
		StatsLogger::instance()->getAliases(sentUniqueId);
	if (!aliases.empty())
	{
		LangString alias = LANG_STRING(aliases.front());
		playerName = alias;
	}

	// Chose a random model for this tank
	TankModel *tankModel = 
		ScorchedServer::instance()->getTankModels().getRandomModel(0, false);

	// Create this tank
	ComsAddPlayerMessage addPlayerMessage(
		tankId,
		playerName,
		color,
		tankModel->getName(),
		destinationId,
		0,
		"");
	TankAddSimAction *simAction = new TankAddSimAction(addPlayerMessage,
		sentUniqueId, sentSUI, sentHostDesc, ipAddress);
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(simAction);
}
