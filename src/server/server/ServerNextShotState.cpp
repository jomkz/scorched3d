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

#include <server/ServerNextShotState.h>
#include <server/TurnController.h>
#include <server/ScorchedServer.h>
#include <server/ServerState.h>
#include <server/ServerCommon.h>
#include <server/ServerShotHolder.h>
#include <server/ServerChannelManager.h>
#include <tank/TankContainer.h>
#include <tank/TankTeamScore.h>
#include <tank/TankScore.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <common/Logger.h>

ServerNextShotState::ServerNextShotState() :
	GameStateI("ServerNextShotState")
{
}

ServerNextShotState::~ServerNextShotState()
{
}

bool ServerNextShotState::getRoundFinished()
{
	// Check why this round has finished
	int teamWonGame = 
		ScorchedServer::instance()->getContext().getTankTeamScore().getWonGame();
	if (teamWonGame > 0)
	{
		// A team has won
		return true;
	}

	std::map<unsigned int, Tank *> &tanks =
		ScorchedServer::instance()->getTankContainer().getAllTanks();
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		Tank *tank = (*itor).second;
		if (tank->getScore().getWonGame())
		{
			return true;
		}
	}
	
	if (ScorchedServer::instance()->getOptionsGame().getTeams() > 1 &&
		ScorchedServer::instance()->getTankContainer().teamCount() == 1)
	{
		// Only one team left
		return true;
	}
	else if (ScorchedServer::instance()->getTankContainer().aliveCount() < 2)
	{
		// Only one person left
		return true;
	}
	return false;
}

void ServerNextShotState::enterState(const unsigned state)
{
	// Check if this round has finished
	if (getRoundFinished())
	{
		// We have finished with this round
		// check for all rounds completely finished
		if (ScorchedServer::instance()->getOptionsTransient().getCurrentRoundNo() >=
			ScorchedServer::instance()->getOptionsGame().getNoRounds())
		{
			// We have finished with all rounds restart
			ScorchedServer::instance()->getGameState().stimulate(ServerState::ServerStimulusStarting);
		}
		else
		{
			// We have finished with this round, go onto the next round
			ScorchedServer::instance()->getGameState().stimulate(ServerState::ServerStimulusNewGame);
		}
	}
	else
	{
		// Check if turn limit has been exceeded
		if (ScorchedServer::instance()->getOptionsTransient().getCurrentGameNo() >
			ScorchedServer::instance()->getOptionsGame().getNoMaxRoundTurns() &&
			ScorchedServer::instance()->getOptionsGame().getNoMaxRoundTurns() > 0)
		{
			ServerChannelManager::instance()->sendText(
				ChannelText("info", 
					"ROUND_SKIP_TURN_LIMIT", 
					"Skipping round due to turn limit"),
				true);

			// Clear any shots that may be waiting
			ServerShotHolder::instance()->clearShots();
			ScorchedServer::instance()->getGameState().stimulate(ServerState::ServerStimulusNextTurn);
		}
		else
		{
			TurnController::instance()->nextShot();
			if (TurnController::instance()->getPlayersThisShot().empty())
			{
				// There are no players still to have a shot
				// The round must have finished
				ScorchedServer::instance()->getGameState().stimulate(ServerState::ServerStimulusNextRound);
			}
			else
			{
				// We have shots to make, lets make them
				ScorchedServer::instance()->getGameState().stimulate(ServerState::ServerStimulusNextTurn);
			}
		}
	}
}

