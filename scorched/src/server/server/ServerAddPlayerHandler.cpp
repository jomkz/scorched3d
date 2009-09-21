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

#include <server/ServerAddPlayerHandler.h>
#include <server/ServerConnectHandler.h>
#include <server/ScorchedServer.h>
#include <server/ScorchedServerUtil.h>
#include <server/ServerChannelManager.h>
#include <server/ServerSimulator.h>
#include <server/ServerState.h>
#include <common/OptionsScorched.h>
#include <common/StatsLogger.h>
#include <common/Logger.h>
#include <common/Defines.h>
#include <coms/ComsAddPlayerMessage.h>
#include <simactions/TankChangeSimAction.h>
#include <tankai/TankAIStore.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>

ServerAddPlayerHandler *ServerAddPlayerHandler::instance_ = 0;

ServerAddPlayerHandler *ServerAddPlayerHandler::instance()
{
	if (!instance_)
	{
		instance_ = new ServerAddPlayerHandler;
	}
	return instance_;
}

ServerAddPlayerHandler::ServerAddPlayerHandler()
{
	ScorchedServer::instance()->getComsMessageHandler().addHandler(
		ComsAddPlayerMessage::ComsAddPlayerMessageType,
		this);
}

ServerAddPlayerHandler::~ServerAddPlayerHandler()
{
}

bool ServerAddPlayerHandler::processMessage(NetMessage &netMessage,
	const char *messageType, NetBufferReader &reader)
{
	ComsAddPlayerMessage message;
	if (!message.readMessage(reader)) return false;

	// Validate player
	unsigned int playerId = message.getPlayerId();
	Tank *tank = ScorchedServer::instance()->getTankContainer().getTankById(playerId);
	if (!tank || 
		(tank->getState().getState() != TankState::sDead &&
		tank->getState().getState() != TankState::sSpectator))
	{
		ServerChannelManager::instance()->sendText( 
			ChannelText("info", "CHANGE_WHEN_DEAD", 
			"Can only change tank when dead."),
			netMessage.getDestinationId(), 
			false);
		return true;
	}

	// Add a computer player (if chosen and a single player match)
	if (0 != strcmp(message.getPlayerType(), "Human"))
	{
		if (ScorchedServer::instance()->getServerState().getState() !=
			ServerState::ServerWaitingForPlayersState)
		{
			ServerChannelManager::instance()->sendText( 
				ChannelText("info", "CHANGE_WHEN_STARTED", 
					"Can only change type before game starts."),
					netMessage.getDestinationId(),
					false);
			return true;
		}

		// Only allow this on a single player game
#ifdef S3D_SERVER
		return true;
#endif // #ifdef S3D_SERVER

		// Check tank ai is valid
		TankAI *ai = 
			ScorchedServer::instance()->getTankAIs().
			getAIByName(message.getPlayerType());
		if (!ai) return true;

		// Set the tank to have the ai
		tank->setTankAI(ai->createCopy(tank));
		tank->setDestinationId(0);
	}
	else
	{
		tank->setDestinationId(netMessage.getDestinationId());
		tank->setTankAI(0);
	}

	// Setup the new player
	LangString name(message.getPlayerName());
	filterName(tank, name);
	message.setPlayerName(name);

#ifdef S3D_SERVER
	// Tell this computer that a new tank has connected
	if (name != tank->getTargetName())
	{
		Logger::log(S3D::formatStringBuffer(
			"Player playing dest=\"%i\" id=\"%i\" \"%s\"->\"%s\"",
			tank->getDestinationId(), tank->getPlayerId(),
			tank->getCStrName().c_str(), name.c_str()));

		ServerChannelManager::instance()->sendText( 
			ChannelText("info",
				"PLAYER_NAME_CHANGE",
				"Player \"{0}\" changed name to \"{1}\"",
				tank->getTargetName(), name),
			true);

		StatsLogger::TankRank rank = StatsLogger::instance()->tankRank(tank);
		if (rank.rank >= 0)
		{
			ServerChannelManager::instance()->sendText( 
				ChannelText("info",
					"WELCOME_BACK",
					"Welcome back [p:{0}], you are ranked {1}",
					tank->getTargetName(), rank.rank),
				tank->getDestinationId(),
				false);
		}

		if (tank->getState().getState() == TankState::sSpectator)
		{
			ServerChannelManager::instance()->sendText( 
				ChannelText("info",
					"PLAYER_PLAYING",
					"Player playing [p:{0}]",
					tank->getTargetName()),
				true);
		}
	}
#endif // #ifdef S3D_SERVER

	// Send to client
	TankChangeSimAction *simAction = new TankChangeSimAction(message);
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(simAction);

	return true;
}

bool ServerAddPlayerHandler::filterName(Tank *tank,
	LangString &sentname)
{
	LangString originalname = sentname;

	// Remove spaces from the front and end of the name 
	LangStringUtil::trim(sentname);

	// Ensure this name does not have any "bad" words in it
	ScorchedServerUtil::instance()->textFilter.filterString(sentname);

	// Remove unwanted characters from middle
	for (unsigned int *c = (unsigned int *) sentname.c_str(); *c;  c++)
	{
		if (*c == '\"') *c = '\'';
		else if (*c == ']') *c = ')';
		else if (*c == '[') *c = '(';
		else if (*c == '%') *c = '$'; // Save problems with special chars
		if (!ScorchedServer::instance()->getOptionsGame().getAllowMultiLingualNames())
		{
			if (*c > 127) *c = '?';
		}
	}

	// Ensure this name does not have the bot name in it
	LangString botPrefix = 
		LANG_STRING(ScorchedServer::instance()->getOptionsGame().getBotNamePrefix());
	unsigned int *botPrefixPos = LangStringUtil::stristr(sentname.c_str(), botPrefix);
	if (botPrefixPos)
	{
		for (int i=0; i<(int) botPrefix.size(); i++, botPrefixPos++)
		{
			(*botPrefixPos) = '*';
		}
	}		

	// Check the client provides a name with a least 1 char in it
	// and the name is less than 16 chars
	if (sentname.size() == 0) sentname = LANG_STRING("NoName");
	if (sentname.size() > 22) sentname = sentname.substr(0, 22);

	// Make sure no-one has the same name
	for (;;)
	{
		bool found = false;
		std::map<unsigned int, Tank *>::iterator mainitor;
		std::map<unsigned int, Tank *> tanks = 
			ScorchedServer::instance()->getTankContainer().getAllTanks();
		for (mainitor = tanks.begin();
			mainitor != tanks.end();
			mainitor++)
		{
			Tank *currentTank = (*mainitor).second;
			if (currentTank->getTargetName() == sentname &&
				tank != currentTank) 
			{
				found = true;
				break;
			}
		}

		if (!found) break;
		sentname += LANG_STRING("(2)");
	}

	// Make sure that no-one else has the same registered name
	// except the prefered user that has this name
	if (ScorchedServer::instance()->getOptionsGame().getRegisteredUserNames())
	{
		ServerAuthHandler *authHandler =
			ScorchedServerUtil::instance()->getAuthHandler();
		if (authHandler)
		{
			while (!authHandler->authenticateUserName(tank->getUniqueId(),
				sentname))
			{
				sentname += LANG_STRING("(2)");
			}
		}
	}

	return (sentname != originalname);
}
