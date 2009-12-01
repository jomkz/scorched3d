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

#include <server/ServerTurns.h>
#include <server/ScorchedServer.h>
#include <server/ServerSimulator.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <tank/TankTeamScore.h>
#include <common/OptionsScorched.h>
#include <simactions/TankStartMoveSimAction.h>
#include <simactions/TankStopMoveSimAction.h>
#include <simactions/PlayMovesSimAction.h>
#include <coms/ComsPlayedMoveMessage.h>

ServerTurns::ServerTurns(bool waitForShots) : 
	shotsState_(eShotsNone),
	waitForShots_(waitForShots)
{
	shotsStarted_ = new SimulatorIAdapter<ServerTurns>(this,  &ServerTurns::shotsStarted);
	moveStarted_ = new SimulatorIAdapter<ServerTurns>(this,  &ServerTurns::moveStarted);
}

ServerTurns::~ServerTurns()
{
}

void ServerTurns::enterState()
{
	shotsState_ = eShotsNone;
	std::map<unsigned int, PlayingPlayer*>::iterator playingItor;
	for (playingItor = playingPlayers_.begin();
		playingItor != playingPlayers_.end();
		playingItor++)
	{
		delete playingItor->second;
	}
	playingPlayers_.clear();
	internalEnterState();
}

void ServerTurns::simulate(fixed frameTime)
{
	// Check what we are allowed to do
	if (shotsState_ == eShotsWaitingStart) return;
	if (shotsState_ == eShotsWaitingEnd)
	{
		if (ScorchedServer::instance()->getActionController().noReferencedActions())
		{
			shotsState_ = eShotsNone;
		}
		else
		{
			return;
		}
	}

	// Check if any players have timed out
	std::map<unsigned int, PlayingPlayer*>::iterator playingItor;
	for (playingItor = playingPlayers_.begin();
		playingItor != playingPlayers_.end();
		playingItor++)
	{
		unsigned int playerId = playingItor->first;
		PlayingPlayer *playing = playingItor->second;
		if (playing->startedMove_)
		{
			playing->moveTime_ -= frameTime;
			if (playing->moveTime_ < 0)
			{
				// Player has timed out
				ComsPlayedMoveMessage timedOutMessage(
					playerId, playing->moveId_, ComsPlayedMoveMessage::eTimeout);
				moveFinished(timedOutMessage);
				break;
			}
		}
	}

	// Do other stuff
	internalSimulate(frameTime);
}

void ServerTurns::shotsStarted(fixed simulationTime, SimAction *action)
{
	shotsState_ = eShotsWaitingEnd;
}

void ServerTurns::moveStarted(fixed simulationTime, SimAction *action)
{
	TankStartMoveSimAction *startMove = (TankStartMoveSimAction *) action;
	
	std::map<unsigned int, PlayingPlayer*>::iterator findItor =
		playingPlayers_.find(startMove->getPlayerId());
	if (findItor != playingPlayers_.end())
	{
		findItor->second->startedMove_ = true;
	}
}

void ServerTurns::moveFinished(ComsPlayedMoveMessage &playedMessage)
{
	internalMoveFinished(playedMessage);
}

bool ServerTurns::finished()
{
	return internalFinished();
}

bool ServerTurns::internalFinished()
{
	return ServerTurns::showScore();
}

bool ServerTurns::showScore()
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

void ServerTurns::playMove(Tank *tank, unsigned int moveId, fixed shotTime)
{
	tank->getState().setMoveId(moveId);
	TankStartMoveSimAction *tankSimAction = new TankStartMoveSimAction(
		tank->getPlayerId(), tank->getState().getMoveId(), shotTime, false);

	if (shotTime > 0)
	{
		PlayingPlayer *playingPlayer = new PlayingPlayer(moveId, shotTime);
		playingPlayers_[tank->getPlayerId()] = playingPlayer;

		ScorchedServer::instance()->getServerSimulator().addSimulatorAction(tankSimAction, moveStarted_);
	}
	else
	{
		ScorchedServer::instance()->getServerSimulator().addSimulatorAction(tankSimAction);
	}
}

void ServerTurns::playMoveFinished(Tank *tank)
{	
	// Remove timeout check for this player
	std::map<unsigned int, PlayingPlayer*>::iterator findItor =
		playingPlayers_.find(tank->getPlayerId());
	if (findItor != playingPlayers_.end())
	{
		delete findItor->second;
		playingPlayers_.erase(findItor);
	}

	// Send stop move action
	TankStopMoveSimAction *tankSimAction = 
		new TankStopMoveSimAction(tank->getPlayerId());
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(tankSimAction);	
	tank->getState().setMoveId(0);
}

void ServerTurns::playShots(std::list<ComsPlayedMoveMessage *> messages, unsigned int moveId, bool timeOutPlayers)
{
	PlayMovesSimAction *movesAction = 
		new PlayMovesSimAction(moveId, timeOutPlayers);
	std::list<ComsPlayedMoveMessage *>::iterator itor;
	for (itor = messages.begin();
		itor != messages.end();
		itor++)
	{
		movesAction->addMove(*itor);
	}
	
	SimulatorI *callback = 0;
	if (waitForShots_)
	{
		callback = shotsStarted_;
		shotsState_ = eShotsWaitingStart;
	}
	ScorchedServer::instance()->getServerSimulator().
		addSimulatorAction(movesAction, callback);
}
