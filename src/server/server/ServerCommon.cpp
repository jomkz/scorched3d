////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
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

#include <server/ServerCommon.h>
#include <server/ScorchedServer.h>
#include <server/ScorchedServerUtil.h>
#include <server/ServerMessageHandler.h>
#include <server/ServerChannelManager.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <target/TargetLife.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <common/Logger.h>
#include <common/FileLogger.h>
#include <common/Defines.h>
#include <coms/ComsMessageSender.h>
#include <coms/ComsConnectRejectMessage.h>
#include <net/NetInterface.h>

static FileLogger *serverFileLogger = 0;

void ServerCommon::startFileLogger()
{
	if (!serverFileLogger) 
	{
		char buffer[256];
		snprintf(buffer, 256, "ServerLog-%i-", 
			ScorchedServer::instance()->getOptionsGame().getPortNo());

		serverFileLogger = new FileLogger(buffer);
		if (0 != strcmp(ScorchedServer::instance()->getOptionsGame().
			getServerFileLogger(), "none"))
		{
			Logger::addLogger(serverFileLogger);
			Logger::log( "Created file logger.");
		}
		else
		{
			Logger::log( "Not created file logger.");
		}
	}	
}

void ServerCommon::kickDestination(unsigned int destinationId, 
	const std::string &message)
{
	Logger::log(S3D::formatStringBuffer("Kicking destination \"%i\"", 
		destinationId));

	if (message[0])
	{
		ComsConnectRejectMessage rejectMessage(message.c_str());
		ComsMessageSender::sendToSingleClient(rejectMessage, destinationId);
		ScorchedServer::instance()->getNetInterface().processMessages();
	}

	bool kickedPlayers = false;
	std::map<unsigned int, Tank *>::iterator itor;
	std::map<unsigned int, Tank *> tanks = 
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		Tank *tank = (*itor).second;
		if (tank->getDestinationId() == destinationId)
		{
			kickedPlayers = true;
			kickPlayer(tank->getPlayerId());
		}
	}
	
	// Make sure we disconnect even if a player has not been created yet
	if (!kickedPlayers)
	{
		ScorchedServer::instance()->getNetInterface().
			disconnectClient(destinationId);
	}
}

void ServerCommon::kickPlayer(unsigned int playerId)
{
	Logger::log(S3D::formatStringBuffer("Kicking player \"%i\"", playerId));

	Tank *tank = ScorchedServer::instance()->
		getTankContainer().getTankById(playerId);
	if (tank)
	{
		ServerChannelManager::instance()->sendText(ChannelText("info", 
			"ADMIN_PLAYER_KICKED", 
			"Player \"{0}\" has been kicked from the server",
			tank->getTargetName()), true);
		Logger::log(S3D::formatStringBuffer("Kicking client \"%s\" \"%i\"", 
			tank->getCStrName().c_str(), tank->getPlayerId()));

		if (tank->getDestinationId() == 0)
		{
			ServerMessageHandler::instance()->
				destroyPlayer(tank->getPlayerId(), "Kicked");
		}
		else
		{
			// Cannot delay kick a player as the rest of the code
			// may expect him to have gone (when he is still there delayed)
			// and try to send messages to him
			ScorchedServer::instance()->getNetInterface().
				disconnectClient(tank->getDestinationId());
			ScorchedServer::instance()->getNetInterface().processMessages();
		}
	}
}

void ServerCommon::killAll()
{
	Logger::log("Killing all players");

	std::map<unsigned int, Tank *>::iterator itor;
	std::map<unsigned int, Tank *> &tanks = 
		ScorchedServer::instance()->getTankContainer().getAllTanks();
	for (itor = tanks.begin();
		 itor != tanks.end();
		 itor++)
	{
		Tank *current = (*itor).second;
		if (current->getState().getState() == TankState::sNormal)
		{
			current->getState().setState(TankState::sDead);
			current->getState().setLives(0);
		}
	}
}

void ServerCommon::startNewGame()
{
	killAll();

	Logger::log("Starting a new game");
	ScorchedServer::instance()->getOptionsTransient().startNewGame();
}

bool &ServerCommon::getExitEmpty()
{
	static bool exitEmpty = false;
	return exitEmpty;
}

void ServerCommon::serverLog(const std::string &text)
{
#ifdef S3D_SERVER
	{
		Logger::log(text);
	}
#endif
}
