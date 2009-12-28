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

#include <server/ServerMessageHandler.h>
#include <server/ScorchedServer.h>
#include <server/ScorchedServerUtil.h>
#include <server/ServerCommon.h>
#include <server/ServerBanned.h>
#include <server/ServerChannelManager.h>
#include <server/ServerDestinations.h>
#include <server/ServerSimulator.h>
#include <server/ServerState.h>
#include <tank/TankDeadContainer.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <tankai/TankAINone.h>
#include <simactions/TankRemoveSimAction.h>
#include <coms/ComsMessageSender.h>
#include <net/NetInterface.h>
#include <common/Logger.h>
#include <common/OptionsScorched.h>
#include <common/StatsLogger.h>

ServerMessageHandler *ServerMessageHandler::instance_ = 0;

ServerMessageHandler *ServerMessageHandler::instance()
{
	if (!instance_)
	{
		instance_ = new ServerMessageHandler;
	}
	return instance_;
}

ServerMessageHandler::ServerMessageHandler()
{
}

ServerMessageHandler::~ServerMessageHandler()
{
}

void ServerMessageHandler::messageRecv(unsigned int destinationId)
{
}

void ServerMessageHandler::messageSent(unsigned int destinationId)
{
}

void ServerMessageHandler::clientConnected(NetMessage &message)
{
	// Check if this destination has been banned
	if (message.getIpAddress() != 0 &&
		ScorchedServerUtil::instance()->bannedPlayers.getBanned(message.getIpAddress()) == 
		ServerBanned::Banned)
	{
		Logger::log(S3D::formatStringBuffer("Banned client connected dest=\"%i\" ip=\"%s\"", 
			message.getDestinationId(),
			NetInterface::getIpName(message.getIpAddress())));
		ScorchedServer::instance()->getNetInterface().
			disconnectClient(message.getDestinationId());
		return;
	}

	// Check if a player from this destination has connected already
	std::map<unsigned int, ServerDestination *> &destinations =
		ScorchedServer::instance()->getServerDestinations().getServerDestinations();
	std::map<unsigned int, ServerDestination *>::iterator destItor;
	for (destItor = destinations.begin();
		destItor != destinations.end();
		destItor++)
	{
		unsigned int serverDestinationId = destItor->first;
		ServerDestination *serverDestination = destItor->second;
		if (serverDestinationId == message.getDestinationId())
		{
			Logger::log(S3D::formatStringBuffer("Duplicate connection from destination \"%i\"", 
				message.getDestinationId()));
			ScorchedServer::instance()->getNetInterface().
				disconnectClient(message.getDestinationId());
			return;
		}
		
		if (!ScorchedServer::instance()->getOptionsGame().getAllowSameIP() &&
			message.getIpAddress() != 0)
		{
			if (message.getIpAddress() == serverDestination->getIpAddress())
			{
				Logger::log(S3D::formatStringBuffer("Duplicate ip connection from ip address \"%s\"", 
					NetInterface::getIpName(message.getIpAddress())));
				ScorchedServer::instance()->getNetInterface().
					disconnectClient(message.getDestinationId());
				return;
			}
		}
	}

	// Add to list of destinations
	ScorchedServer::instance()->getServerDestinations().addDestination(
		message.getDestinationId(), message.getIpAddress());

	Logger::log(S3D::formatStringBuffer("Client connected dest=\"%i\" ip=\"%s\"", 
		message.getDestinationId(),
		NetInterface::getIpName(message.getIpAddress())));
}

void ServerMessageHandler::clientDisconnected(NetMessage &message)
{
	const char *reason = "Unknown";
	if (message.getFlags() == NetMessage::UserDisconnect) reason = "User";
	else if (message.getFlags() == NetMessage::KickDisconnect) reason = "Kicked";
	else if (message.getFlags() == NetMessage::TimeoutDisconnect) reason = "Timeout";

	Logger::log(S3D::formatStringBuffer("Client disconnected dest=\"%i\" ip=\"%s\" reason=\"%s\"", 
		message.getDestinationId(),
		NetInterface::getIpName(message.getIpAddress()),
		reason));

	// Build up a list of players at this destination
	std::list<unsigned int> removePlayers;
	unsigned int destinationId = message.getDestinationId();
	std::map<unsigned int, Tank *>::iterator itor;
	std::map<unsigned int, Tank *> &tanks = 
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		Tank *tank = (*itor).second;
		if (tank->getDestinationId() == destinationId)
		{
			removePlayers.push_back(tank->getPlayerId());
		}
	}

	// Remove all players for this destination
	std::list<unsigned int>::iterator remItor;
	for (remItor = removePlayers.begin();
		 remItor != removePlayers.end();
		 remItor++)
	{
		unsigned int playerId = *remItor;
		destroyPlayer(playerId, reason);
	}

	// Inform the channel manager
	ServerChannelManager::instance()->destinationDisconnected(message.getDestinationId());

	// Remove from list of destinations
	ScorchedServer::instance()->getServerDestinations().removeDestination(
		message.getDestinationId());
}

void ServerMessageHandler::destroyPlayer(unsigned int tankId, const char *reason)
{
	// Try to remove this player
	Tank *tank = ScorchedServer::instance()->getTankContainer().getTankById(tankId);
	if (!tank)
	{
		Logger::log(S3D::formatStringBuffer("Unknown player disconnected id=\"%i\" (%s)", 
			tankId, reason));
		return;
	}

	// Log disconnect
	Logger::log( 
		S3D::formatStringBuffer("Player disconnected dest=\"%i\" id=\"%i\" name=\"%s\" reason=\"%s\"", 
		tank->getDestinationId(),
		tankId, 
		tank->getCStrName().c_str(),
		reason));
	ServerChannelManager::instance()->sendText(
		ChannelText("info", 
			"PLAYER_DISCONNECTED", 
			"Player disconnected [p:{0}] ({1})",
			tank->getTargetName(), reason),
		true);

	// The time to actualy remove the tank after
	fixed removalTime = 0;

	// Check if we can remove player
	if (tank->getState().getState() == TankState::sNormal &&
		ScorchedServer::instance()->getServerState().getState() == 
		ServerState::ServerPlayingState &&
		tank->getDestinationId() != 0)
	{
		// Actualy remove the tank after a few seconds have passed
		removalTime = ScorchedServer::instance()->getOptionsGame().getRemoveTime();
	}

	// Make this player a computer controlled player
	{
		TankAI *ai = new TankAINone(tank->getPlayerId());
		tank->setTankAI(ai); // Will automaticaly set the destinationId to 0
	}

	// Add tank to tank dead container to remember its stats
	if (!ScorchedServer::instance()->getOptionsGame().getResidualPlayers())
	{
		if (tank->getState().getTankPlaying() &&
			tank->getUniqueId()[0] &&
			tank->getDestinationId() != 0)
		{
			ScorchedServer::instance()->getTankDeadContainer().addDeadTank(tank);
		}
	}

	// Log the removal
	StatsLogger::instance()->tankDisconnected(tank);

	// Actualy remove the tank from the client and server
	TankRemoveSimAction *removeSimAction = 
		new TankRemoveSimAction(tankId, removalTime);
	ScorchedServer::instance()->getServerSimulator().
		addSimulatorAction(removeSimAction);
}

void ServerMessageHandler::clientError(NetMessage &message,
	const std::string &errorString)
{
	Logger::log(S3D::formatStringBuffer("Client \"%i\", ***Server Error*** \"%s\"", 
		message.getDestinationId(),
		errorString.c_str()));
	ServerCommon::kickDestination(message.getDestinationId());
}
