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

#include <server/ServerAddPlayerHandler.h>
#include <server/ServerConnectHandler.h>
#include <server/ScorchedServer.h>
#include <server/ScorchedServerUtil.h>
#include <server/ServerState.h>
#include <server/ServerChannelManager.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <common/StatsLogger.h>
#include <common/Logger.h>
#include <common/Defines.h>
#include <coms/ComsAddPlayerMessage.h>
#include <coms/ComsPlayerStateMessage.h>
#include <coms/ComsMessageSender.h>
#include <net/NetLoopBack.h>
#include <tankai/TankAIStore.h>
#include <tank/TankModelStore.h>
#include <tank/TankModelContainer.h>
#include <tank/TankContainer.h>
#include <tank/TankColorGenerator.h>
#include <tank/TankState.h>
#include <tank/TankAvatar.h>

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
		"ComsAddPlayerMessage",
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
		tank->getState().getState() != TankState::sPending &&
		tank->getState().getState() != TankState::sLoading &&
		tank->getState().getState() != TankState::sInitializing))
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
		if (ScorchedServer::instance()->getGameState().getState() !=
			ServerState::ServerStateTooFewPlayers)
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
	std::string name(message.getPlayerName());
	filterName(tank, name);
	if (name != tank->getName())
	{
		getUniqueName(tank, name);

		// Tell this computer that a new tank has connected
#ifdef S3D_SERVER
		{
			Logger::log(S3D::formatStringBuffer("Player playing dest=\"%i\" id=\"%i\" \"%s\"->\"%s\"",
				tank->getDestinationId(), tank->getPlayerId(),
				tank->getName(), name.c_str()));

			if (name != tank->getName())
			{
				ServerChannelManager::instance()->sendText( 
					ChannelText("info",
						"PLAYER_NAME_CHANGE",
						"Player changed name \"{0}\"->\"{1}\"",
						tank->getName(), name),
					true);
			}
		}
#endif // #ifdef S3D_SERVER
	}
	tank->setName(name.c_str());

	// Player has set a new color
	if (tank->getTeam() == 0 &&
		message.getPlayerColor() != tank->getColor())
	{
		// Check the color is not already in use
		std::map<unsigned int, Tank *> &tanks = 
			ScorchedServer::instance()->getTankContainer().getPlayingTanks();
		if (TankColorGenerator::instance()->colorAvailable(
			message.getPlayerColor(), tanks, tank))
		{
			// Set this color
			tank->setColor(message.getPlayerColor());
		}
	}

	bool noAvatar = !tank->getAvatar().getName()[0] 
		&& message.getPlayerIconName()[0];
	if (noAvatar) // Currently we can only set the avatar once
	{
		if (message.getPlayerIcon().getBufferUsed() <=
			(unsigned) ScorchedServer::instance()->getOptionsGame().getMaxAvatarSize())
		{
			tank->getAvatar().setFromBuffer(
				message.getPlayerIconName(),
				message.getPlayerIcon());
	
			// Send a new add message to all clients (this contains the avatar)
			// the client will not add the player but will update the avatar instead
			// Note: this can be removed if we ever have enough bandwidth to send
			// the avatar in each state message along with the rest of the tanks
			// attributes.
			ComsMessageSender::sendToAllConnectedClients(message);
		}
	}

	// Tell the logger about a new tank
	StatsLogger::instance()->tankJoined(tank);

#ifdef S3D_SERVER
	{
		StatsLogger::TankRank rank = StatsLogger::instance()->tankRank(tank);
		if (rank.rank >= 0)
		{
			ServerChannelManager::instance()->sendText( 
				ChannelText("info",
					"WELCOME_BACK",
					"Welcome back {0}, you are ranked {1}",
					tank->getName(), rank.rank),
				true);
		}

		if (tank->getState().getSpectator())
		{
			ServerChannelManager::instance()->sendText( 
				ChannelText("info",
					"PLAYER_PLAYING",
					"Player playing \"{0}\"",
					tank->getName()),
				true);

			if (ScorchedServer::instance()->getGameState().getState() == 
				ServerState::ServerStateStarting)
			{
				// Reset the starting timer
				ScorchedServer::instance()->getGameState().stimulate(
					ServerState::ServerStimulusStarting);
			}
		}
	}
#endif // #ifdef S3D_SERVER

	// Make sure the tank state is as we expected
	// This also fixes setting the state after loading 
	// a saved game
	tank->getState().setSpectator(false);

	// Choose a team (if applicable)
	if (ScorchedServer::instance()->getOptionsGame().getTeams() > 1)
	{
		if (message.getPlayerTeam() > 0 && message.getPlayerTeam() <=
			(unsigned int) ScorchedServer::instance()->getOptionsGame().getTeams())
		{
			tank->setTeam(message.getPlayerTeam());
		}
		else
		{
			tank->setTeam(ScorchedServer::instance()->getOptionsTransient().getLeastUsedTeam(
				ScorchedServer::instance()->getTankContainer()));
		}
	}

	// Make sure the model is available and for the correct team
	// Do this AFTER the team has been set
	TankModel *tankModel = 
		ScorchedServer::instance()->getTankModels().
			getModelByName(message.getModelName(), 
				tank->getTeam(),
				tank->isTemp());
	tank->getModelContainer().setTankModelName(
		tankModel->getName(), message.getModelName(), tankModel->getTypeName());

	// If we are in a waiting for players state then we can
	// send the state of these new players
	if (ScorchedServer::instance()->getGameState().getState() == ServerState::ServerStateTooFewPlayers ||
		ScorchedServer::instance()->getGameState().getState() == ServerState::ServerStateStarting)
	{
		ComsPlayerStateMessage message(false, false);
		ComsMessageSender::sendToAllConnectedClients(message);
	}
	return true;
}

void ServerAddPlayerHandler::filterName(Tank *tank,
	std::string &sentname)
{
	// Ensure this name does not have any "bad" words in it
	LangString sentNameLangString(LANG_STRING(sentname));
	ScorchedServerUtil::instance()->textFilter.filterString(sentNameLangString);
	sentname = LangStringUtil::convertFromLang(sentNameLangString);

	// Form the correct player name
	// Remove spaces from the front of the name and
	// unwanted characters from middle
	char *nameBeginning = (char *) sentname.c_str();
	while (*nameBeginning == ' ') nameBeginning++;

	char *nameEnding = nameBeginning + strlen(nameBeginning);
	while (*nameEnding == ' ' || *nameEnding == '\0') { *nameEnding = '\0'; nameEnding--; }

	char *nameMiddle = nameBeginning;
	for (;*nameMiddle; nameMiddle++)
	{
		if (*nameMiddle == '\"') *nameMiddle = '\'';
		else if (*nameMiddle == ']') *nameMiddle = ')';
		else if (*nameMiddle == '[') *nameMiddle = '(';
		else if (*nameMiddle == '%') *nameMiddle = '$'; // Save problems with special chars
	}

	// Ensure this name does not have the bot name in it
	const char *botPrefix = 
		ScorchedServer::instance()->getOptionsGame().getBotNamePrefix();
	char *botPrefixPos = S3D::stristr(sentname.c_str(), botPrefix);
	if (botPrefixPos)
	{
		for (int i=0; i<(int) strlen(botPrefix); i++, botPrefixPos++)
		{
			(*botPrefixPos) = ' ';
		}
	}		

	// Check the client provides a name with a least 1 char in it
	// and the name is less than 16 chars
	std::string playerName;
	int nameLen = strlen(nameBeginning);
	if (nameLen == 0) playerName = "NoName";
	else if (nameLen < 22) playerName = nameBeginning;
	else playerName.append(nameBeginning, 22);

	sentname = playerName;
}

void ServerAddPlayerHandler::getUniqueName(Tank *tank,
	std::string &playerName)
{
	// Make sure no-one has the same name
	while (ScorchedServer::instance()->getTankContainer().
		getTankByName(playerName.c_str()))
	{
		playerName += "(2)";
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
				playerName.c_str()))
			{
				playerName += "(2)";
			}
		}
	}
}
