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

#include <server/ServerStateEnoughPlayers.h>
#include <server/ServerChannelManager.h>
#include <server/ScorchedServer.h>
#include <server/ServerSimulator.h>
#include <server/ServerCommon.h>
#include <simactions/TankTeamBallanceSimAction.h>
#include <simactions/TankBotBallanceSimAction.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <common/OptionsScorched.h>
#include <common/Logger.h>

ServerStateEnoughPlayers::ServerStateEnoughPlayers()
{
}

ServerStateEnoughPlayers::~ServerStateEnoughPlayers()
{
}

bool ServerStateEnoughPlayers::enoughPlayers()
{
	/*
	if (TankTeamBallanceSimAction::needsTeamBallance(
		ScorchedServer::instance()->getContext()))
	{
		TankTeamBallanceSimAction *teamBallance =
			new TankTeamBallanceSimAction();
		ScorchedServer::instance()->getServerSimulator().
			addSimulatorAction(teamBallance);
	}
	if (TankBotBallanceSimAction::needsBotBallance(
		ScorchedServer::instance()->getContext()))
	{
		TankBotBallanceSimAction *botBallance =
			new TankBotBallanceSimAction();
		ScorchedServer::instance()->getServerSimulator().
			addSimulatorAction(botBallance);
	}
	*/

	// Make sure we have enough players to play a game
	if (ScorchedServer::instance()->getTankContainer().getNoOfNonSpectatorTanks() <
		ScorchedServer::instance()->getOptionsGame().getNoMinPlayers())
	{
		checkExit();
		return false;
	}
	
	// Check we have enough team players
	if (ScorchedServer::instance()->getOptionsGame().getTeams() > 1 &&
		ScorchedServer::instance()->getOptionsGame().getTeamBallance() !=
			OptionsGame::TeamBallanceAuto &&
		ScorchedServer::instance()->getOptionsGame().getTeamBallance() !=
			OptionsGame::TeamBallanceAutoByScore &&
		ScorchedServer::instance()->getOptionsGame().getTeamBallance() !=
			OptionsGame::TeamBallanceAutoByBots)
	{
		// If it is auto ballanced, then if there are at least two players
		// then we are ok.  And if there are not two players then
		// the first check will catch it.
				
		// Check there is at least one player in each team
		int teamCount[4];
		for (int i=0; i<ScorchedServer::instance()->getOptionsGame().getTeams();i++)
		{
			teamCount[i] = 0;
		}
		std::map<unsigned int, Tank *> &playingTanks = 
			ScorchedServer::instance()->getTankContainer().getPlayingTanks();
		std::map<unsigned int, Tank *>::iterator mainitor;
		for (mainitor = playingTanks.begin();
			 mainitor != playingTanks.end();
			 mainitor++)
		{
			Tank *current = (*mainitor).second;
			if (current->getState().getTankPlaying() &&
				current->getTeam() > 0)
			{
				teamCount[current->getTeam() - 1]++;
			}
		}
		for (int i=0; i<ScorchedServer::instance()->getOptionsGame().getTeams();i++)
		{
			if (teamCount[i] == 0)
			{
				checkExit();
				return false;
			}
		}
	}
	
	return true;
}

void ServerStateEnoughPlayers::checkExit()
{
	if (ServerCommon::getExitEmpty())
	{
		Logger::log("Exit server when empty");
		exit(0);
	}
}
