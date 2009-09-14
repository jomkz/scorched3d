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

#include <simactions/TankBotBallanceSimAction.h>
#include <common/OptionsScorched.h>
#include <common/ChannelManager.h>
#include <server/ScorchedServer.h>
#include <server/ServerMessageHandler.h>
#include <tankai/TankAIAdder.h>
#include <tankai/TankAI.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>

REGISTER_CLASS_SOURCE(TankBotBallanceSimAction);

TankBotBallanceSimAction::TankBotBallanceSimAction()
{
}

TankBotBallanceSimAction::~TankBotBallanceSimAction()
{
}

bool TankBotBallanceSimAction::invokeAction(ScorchedContext &context)
{
	// Only run this on the server for the moment
	if (!context.getServerMode())
	{
		return true;
	}

	// Tell people whats going on
	ChannelText text("info",
		"AUTO_BALLANCE_BOTS",
		"Auto ballancing bots");
	ChannelManager::showText(context, text);

	int requiredPlayers =
		context.getOptionsGame().getRemoveBotsAtPlayers();
	if (requiredPlayers == 0)
	{
		// Check if this feature is turned off
		return true;
	}

	// Get the number of players we have
	int noPlayers = countBots(context);

	// Check if we need to gain or lose players
	if (noPlayers > requiredPlayers)
	{
		removeBots(requiredPlayers, noPlayers);
	}
	else if (noPlayers < requiredPlayers)
	{
		addBots(requiredPlayers, noPlayers);
	}

	return true;
}

bool TankBotBallanceSimAction::writeMessage(NetBuffer &buffer)
{
	return true;
}
bool TankBotBallanceSimAction::readMessage(NetBufferReader &reader)
{
	return true;
}

bool TankBotBallanceSimAction::needsBotBallance(ScorchedContext &context) 
{
	// Get the number of players we require
	int requiredPlayers =
		context.getOptionsGame().getRemoveBotsAtPlayers();
	if (requiredPlayers == 0)
	{
		// Check if this feature is turned off
		return false;
	}

	// Get the number of players we have
	int noPlayers = countBots(context);

	// Check if the two are equal
	return (noPlayers != requiredPlayers);
}

int TankBotBallanceSimAction::countBots(ScorchedContext &context)
{
	// Add up the number of tanks that are either
	// human and not spectators
	// or an ai
	int noPlayers = 0;
	std::map<unsigned int, Tank *> &playingTanks = 
		context.getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank *>::iterator mainitor;
	for (mainitor = playingTanks.begin();
		mainitor != playingTanks.end();
		mainitor++)
	{
		Tank *current = (*mainitor).second;
		if (current->getState().getTankPlaying() ||
			current->getDestinationId() == 0)
		{
			noPlayers++;
		}
	}
	return noPlayers;
}

void TankBotBallanceSimAction::removeBots(int requiredPlayers, int noPlayers)
{
	std::multimap<unsigned int, unsigned int> ais_;

	// Get this list of computer players and sort them
	// by the time they have been playing for
	std::map<unsigned int, Tank *> &playingTanks = 
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank *>::iterator mainitor;
	for (mainitor = playingTanks.begin();
		mainitor != playingTanks.end();
		mainitor++)
	{
		Tank *current = (*mainitor).second;
		if (current->getDestinationId() == 0)
		{
			unsigned int startTime = (unsigned int)
				current->getScore().getStartTime();
			ais_.insert(std::pair<unsigned int, unsigned int>
				(startTime, current->getPlayerId()));
		}
	}

	// Kick the ais that have been on the server the longest
	std::multimap<unsigned int, unsigned int>::reverse_iterator
		aiItor = ais_.rbegin();
	while (noPlayers > requiredPlayers)
	{
		if (aiItor != ais_.rend())
		{
			std::pair<unsigned int, unsigned int> item = *aiItor;
			ServerMessageHandler::instance()->destroyPlayer(
				item.second, "Auto-kick");
			aiItor++;
		}
		noPlayers--;
	}
}

void TankBotBallanceSimAction::addBots(int requiredPlayers, int noPlayers)
{
	std::multimap<std::string, unsigned int> ais_;

	// Get this list of computer players and sort them
	// by ai name
	std::map<unsigned int, Tank *> &playingTanks = 
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank *>::iterator mainitor;
	for (mainitor = playingTanks.begin();
		mainitor != playingTanks.end();
		mainitor++)
	{
		Tank *current = (*mainitor).second;
		if (current->getDestinationId() == 0)
		{
			ais_.insert(std::pair<std::string, unsigned int>
				(current->getTankAI()->getName(), 
				current->getPlayerId()));
		}
	}

	// Add any computer players that should be playing 
	// and that are not in the list of currently playing
	int maxComputerAIs = 
		ScorchedServer::instance()->getOptionsGame().getNoMaxPlayers();
	for (int i=0; i<maxComputerAIs; i++)
	{
		const char *playerType = 
			ScorchedServer::instance()->getOptionsGame().getPlayerType(i);
		if (0 != stricmp(playerType, "Human") &&
			0 != stricmp(playerType, "Random"))
		{
			std::multimap<std::string, unsigned int>::iterator findItor =
				ais_.find(playerType);
			if (findItor == ais_.end())
			{
				if (noPlayers < requiredPlayers)
				{
					// This player does not exist add them
					TankAIAdder::addTankAI(*ScorchedServer::instance(), playerType);
					noPlayers++;
				}
			}
			else
			{
				// This player does exist dont add them
				ais_.erase(findItor);
			}
		}
	}
	for (int i=0; i<maxComputerAIs; i++)
	{
		const char *playerType = 
			ScorchedServer::instance()->getOptionsGame().getPlayerType(i);
		if (0 != stricmp(playerType, "Human") &&
			0 == stricmp(playerType, "Random"))
		{
			if (ais_.empty())
			{
				if (noPlayers < requiredPlayers)
				{
					// This player does not exist add them
					TankAIAdder::addTankAI(*ScorchedServer::instance(), playerType);
					noPlayers++;
				}										
			}
			else
			{
				// This player does exist dont add them
				ais_.erase(ais_.begin());
			}
		}
	}
}
