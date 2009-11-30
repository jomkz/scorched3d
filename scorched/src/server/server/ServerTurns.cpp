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
	shotsState_(eNone),
	waitForShots_(waitForShots)
{
	shotsStarted_ = new SimulatorIAdapter<ServerTurns>(this,  &ServerTurns::shotsStarted);
}

ServerTurns::~ServerTurns()
{
}

void ServerTurns::enterState()
{
	shotsState_ = eNone;
	internalEnterState();
}

void ServerTurns::simulate(fixed frameTime)
{
	// Check what we are allowed to do
	if (shotsState_ == eWaitingStart) return;
	if (shotsState_ == eWaitingEnd)
	{
		if (ScorchedServer::instance()->getActionController().noReferencedActions())
		{
			shotsState_ = eNone;
		}
		else
		{
			return;
		}
	}

	// Do other stuff
	internalSimulate(frameTime);
}

void  ServerTurns::shotsStarted(fixed simulationTime, SimAction *action)
{
	shotsState_ = eWaitingEnd;
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
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(tankSimAction);
}

void ServerTurns::playMoveFinished(Tank *tank)
{	
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
		shotsState_ = eWaitingStart;
	}
	ScorchedServer::instance()->getServerSimulator().
		addSimulatorAction(movesAction, callback);
}
