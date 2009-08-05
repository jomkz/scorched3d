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

#include <server/ServerStateNewGame.h>
#include <server/ScorchedServer.h>
#include <server/ServerCommon.h>
#include <server/ServerConsoleProgressCounter.h>
#include <server/ServerChannelManager.h>
#include <server/ServerMessageHandler.h>
#include <server/ServerLoadLevel.h>
#include <weapons/EconomyStore.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <tank/TankModelStore.h>
#include <tank/TankModelContainer.h>
#include <tankai/TankAI.h>
#include <tankai/TankAIAdder.h>
#include <engine/Simulator.h>
#include <landscapedef/LandscapeDefinitions.h>
#include <landscapemap/LandscapeMaps.h>
#include <common/OptionsTransient.h>
#include <common/ProgressCounter.h>
#include <common/StatsLogger.h>
#include <common/Logger.h>
#include <algorithm>

ServerStateNewGame::ServerStateNewGame()
{
}

ServerStateNewGame::~ServerStateNewGame()
{
}

void ServerStateNewGame::newGame()
{
	// Make sure options are up to date
	if (ScorchedServer::instance()->getOptionsGame().commitChanges())
	{
		ServerChannelManager::instance()->sendText(
			ChannelText("info", 
				"GAME_OPTIONS_CHANGED", 
				"Game options have been changed!"),
			true);
	}	

	// Make any enconomic changes
	EconomyStore::instance()->getEconomy()->calculatePrices();
	EconomyStore::instance()->getEconomy()->savePrices();

	// Get a landscape definition to use
	ServerCommon::serverLog("Generating landscape");
	LandscapeDefinition defn = ScorchedServer::instance()->getLandscapes().getRandomLandscapeDefn(
		ScorchedServer::instance()->getContext().getOptionsGame(),
		ScorchedServer::instance()->getContext().getTankContainer());

	// Set all options (wind etc..)
	ScorchedServer::instance()->getContext().getOptionsTransient().newGame();

	// Generate the new level
	ScorchedServer::instance()->getLandscapeMaps().generateMaps(
		ScorchedServer::instance()->getContext(), defn, 
		ServerConsoleProgressCounter::instance()->getProgressCounter());
	ScorchedServer::instance()->getSimulator().reset();
	ServerCommon::serverLog("Finished generating landscape");

	// Make sure tanks are in correct state
	std::set<unsigned int> loadingDestinations;
	std::map<unsigned int, Tank *> &tanks = 
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank *>::iterator tankItor;
	for (tankItor = tanks.begin();
		tankItor != tanks.end();
		tankItor++)
	{
		Tank *tank = tankItor->second;
		if (tank->getState().getState() != TankState::sLoading)
		{
			tank->getState().setState(TankState::sLoading);
			if (loadingDestinations.find(tank->getDestinationId()) == loadingDestinations.end())
			{
				loadingDestinations.insert(tank->getDestinationId());
			}
		}
	}

	// Check bots
	checkBots(true);

	// Tell all destinations to load
	std::set<unsigned int>::iterator destItor;
	for (destItor = loadingDestinations.begin();
		destItor != loadingDestinations.end();
		destItor++)
	{
		ServerLoadLevel::destinationLoadLevel(*destItor);
	}
}

void ServerStateNewGame::checkTeams()
{
	// Make sure everyone is in a team if they should be
	std::map<unsigned int, Tank *> &playingTanks = 
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank *>::iterator mainitor;
	for (mainitor = playingTanks.begin();
		 mainitor != playingTanks.end();
		 mainitor++)
	{
		Tank *current = (*mainitor).second;
		if (current->getState().getTankPlaying())
		{
			if (ScorchedServer::instance()->getOptionsGame().getTeams() > 1 &&
				current->getTeam() == 0) current->setTeam(1); 
			if (ScorchedServer::instance()->getOptionsGame().getTeams() == 1 &&
				current->getTeam() > 0) current->setTeam(0); 
		}
	}

	// Do we check teams ballance
	if (ScorchedServer::instance()->getOptionsGame().getTeams() != 1)
	{
		// Check for team ballance types
		switch (ScorchedServer::instance()->getOptionsGame().getTeamBallance())
		{
			case OptionsGame::TeamBallanceAuto:
			case OptionsGame::TeamBallanceAutoByScore:
			case OptionsGame::TeamBallanceAutoByBots:
				checkTeamsAuto();
				break;
			case OptionsGame::TeamBallanceBotsVs:
				checkTeamsBotsVs();
				break;
			default:
				break;
		}
	}

	// Make sure everyone is using a team model
	for (mainitor = playingTanks.begin();
		 mainitor != playingTanks.end();
		 mainitor++)
	{
		Tank *current = (*mainitor).second;
		if (current->getState().getTankPlaying())
		{
			TankModel *model = 
				ScorchedServer::instance()->getTankModels().getModelByName(
					current->getModelContainer().getTankModelName(),
					current->getTeam(),
					current->isTemp());
			if (0 != strcmp(model->getName(),
				current->getModelContainer().getTankModelName()))
			{
				// The model is not allowed for this team,
				// use the correct model
				current->getModelContainer().setTankModelName(
					model->getName(), 
					model->getName(),
					model->getTypeName());
			}
		}
	}
}

static inline bool lt_score(const Tank *o1, const Tank *o2) 
{ 
	return ((Tank*)o1)->getScore().getScore() > ((Tank *)o2)->getScore().getScore();
}

static inline bool lt_bots(const Tank *o1, const Tank *o2) 
{ 
	return ((Tank*)o1)->getTankAI() < ((Tank *)o2)->getTankAI();
}

void ServerStateNewGame::checkTeamsAuto()
{
	// Count players in each team
	std::vector<Tank *> teamPlayers[4];
	std::map<unsigned int, Tank *> &playingTanks = 
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank *>::iterator mainitor;
	for (mainitor = playingTanks.begin();
		 mainitor != playingTanks.end();
		 mainitor++)
	{
		Tank *current = (*mainitor).second;
		if (current->getState().getTankPlaying())
		{
			if (current->getTeam() > 0)
			{
				teamPlayers[current->getTeam() - 1].push_back(current);
			}
		}
	}

	bool ballanced = false;
	bool check = true;
	while (check)
	{
		check = false;
	
		// Find the teams with the min and max players in them
		std::vector<Tank *> *minPlayers = &teamPlayers[0];
		std::vector<Tank *> *maxPlayers = &teamPlayers[0];
		for (int i=0; i<ScorchedServer::instance()->getOptionsGame().getTeams(); i++)
		{
			if (teamPlayers[i].size() < minPlayers->size()) minPlayers = &teamPlayers[i];
			if (teamPlayers[i].size() > maxPlayers->size()) maxPlayers = &teamPlayers[i];			
		}
		
		// Is the difference between the min and max teams >= 2 players
		if (maxPlayers->size() - minPlayers->size() >= 2)
		{
			check = true;
			ballanced = true;
			
			// Sort teams (if needed)
			if (ScorchedServer::instance()->getOptionsGame().getTeamBallance() ==
				OptionsGame::TeamBallanceAutoByScore)
			{
				std::sort(minPlayers->begin(), minPlayers->end(), lt_score); 
				std::sort(maxPlayers->begin(), maxPlayers->end(), lt_score); 
			}
			else if (ScorchedServer::instance()->getOptionsGame().getTeamBallance() ==
				OptionsGame::TeamBallanceAutoByBots)
			{
				std::sort(minPlayers->begin(), minPlayers->end(), lt_bots); 
				std::sort(maxPlayers->begin(), maxPlayers->end(), lt_bots); 
			}
		
			// Find out which team has the least players
			for (int i=0; i<ScorchedServer::instance()->getOptionsGame().getTeams(); i++)
			{	
				if (minPlayers == &teamPlayers[i])
				{
					// Ballance the teams
					Tank *tank = maxPlayers->back();
					maxPlayers->pop_back();
					minPlayers->push_back(tank);
					tank->setTeam(i+1);
				}
			}
		}
	}

	// Check if we needed to ballance
	if (ballanced)
	{
		if (ScorchedServer::instance()->getOptionsGame().getTeamBallance() ==
			OptionsGame::TeamBallanceAutoByScore)
		{
			ServerChannelManager::instance()->sendText(
				ChannelText("info", 
					"SCORE_AUTO_BALLANCE", 
					"Auto ballancing teams, by score"),
				true);
		}
		else if (ScorchedServer::instance()->getOptionsGame().getTeamBallance() ==
			OptionsGame::TeamBallanceAutoByBots)
		{
			ServerChannelManager::instance()->sendText(
				ChannelText("info",
					"BOTS_AUTO_BALLANCE",
					"Auto ballancing teams, by bots"),
				true);
		}
		else
		{
			ServerChannelManager::instance()->sendText(
				ChannelText("info",
					"NORMAL_AUTO_BALLANCE",
					"Auto ballancing teams"),
				true);
		}
	}
}

void ServerStateNewGame::checkTeamsBotsVs()
{
	std::map<unsigned int, Tank *> &playingTanks = 
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank *>::iterator mainitor;
	for (mainitor = playingTanks.begin();
		 mainitor != playingTanks.end();
		 mainitor++)
	{
		Tank *current = (*mainitor).second;
		if (current->getState().getTankPlaying())
		{
			if (current->getDestinationId() == 0) current->setTeam(1);
			else current->setTeam(2);
		}
	}
}

void ServerStateNewGame::checkBots(bool removeBots)
{
	int requiredPlayers =
		ScorchedServer::instance()->getOptionsGame().
			getRemoveBotsAtPlayers();
	if (requiredPlayers == 0)
	{
		// Check if this feature is turned off
		return;
	}

	// Add up the number of tanks that are either
	// human and not spectators
	// or an ai
	int noPlayers = 0;
	std::map<unsigned int, Tank *> &playingTanks = 
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
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

	if (noPlayers > requiredPlayers &&
		removeBots)
	{
		std::multimap<unsigned int, unsigned int> ais_;

		// Get this list of computer players and sort them
		// by the time they have been playing for
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
	if (noPlayers < requiredPlayers)
	{
		std::multimap<std::string, unsigned int> ais_;

		// Get this list of computer players and sort them
		// by ai name
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
}
