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

#include <server/ServerTurnsFree.h>
#include <server/ServerSimulator.h>
#include <server/ScorchedServer.h>
#include <coms/ComsPlayedMoveMessage.h>
#include <common/OptionsScorched.h>
#include <simactions/PlayMovesSimAction.h>
#include <simactions/TankStopMoveSimAction.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <list>

ServerTurnsFree::ServerTurnsFree() :
	nextMoveId_(0)
{
}

ServerTurnsFree::~ServerTurnsFree()
{
}


void ServerTurnsFree::enterState()
{
	nextMoveId_++;

	waitingPlayers_.clear();
	thinkingAIs_.clear();

	std::map<unsigned int, Tank*> &tanks = 
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank*>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		Tank *tank = itor->second;
		if (tank->getState().getState() == TankState::sNormal)
		{
			tank->getState().setMoveId(0);
			waitingPlayers_.push_back(tank->getPlayerId());
		}
	}	
}

void ServerTurnsFree::simulate(fixed frameTime)
{
	// Build list of currently playing destinations
	std::set<unsigned int> playingDestinations;
	std::map<unsigned int, Tank*> &tanks = 
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank*>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		Tank *tank = itor->second;
		if (tank->getState().getState() == TankState::sNormal &&
			tank->getState().getMoveId() != 0 &&
			tank->getDestinationId() != 0)
		{
			playingDestinations.insert(tank->getDestinationId());
		}
	}

	// Check to see if we can play
	std::list<unsigned int>::iterator waitingItor;
	for (waitingItor = waitingPlayers_.begin();
		waitingItor != waitingPlayers_.end();
		waitingItor++)
	{
		unsigned int playerId = *waitingItor;

		Tank *tank = ScorchedServer::instance()->getTankContainer().getTankById(playerId);
		if (tank && 
			tank->getState().getState() == TankState::sNormal &&
			tank->getState().getMoveId() == 0)
		{
			if (playingDestinations.find(tank->getDestinationId()) == 
				playingDestinations.end())
			{
				if (tank->getDestinationId() != 0)
				{
					playMove(tank, ++nextMoveId_, 0);
					playingDestinations.insert(tank->getDestinationId());
				}
				else
				{
					int shotTime = ScorchedServer::instance()->getOptionsGame().getShotTime();
					int thinkingTime = (shotTime / 3) + (rand() % (shotTime / 2));

					tank->getState().setMoveId(++nextMoveId_);
					thinkingAIs_[tank->getPlayerId()] = fixed(thinkingTime);
				}
				waitingPlayers_.erase(waitingItor);
				waitingPlayers_.push_back(playerId);
				break;
			}
		}
	}

	// Let the AIs play
	std::map<unsigned int, fixed>::iterator thinkingItor;
	for (thinkingItor = thinkingAIs_.begin();
		thinkingItor != thinkingAIs_.end();
		thinkingItor++)
	{
		unsigned int playerId = thinkingItor->first;

		Tank *tank = ScorchedServer::instance()->getTankContainer().getTankById(playerId);
		if (tank && 
			tank->getState().getState() == TankState::sNormal)
		{
			thinkingItor->second -= frameTime;
			if (thinkingItor->second <= 0)
			{
				playMove(tank, ++nextMoveId_, 0);
				thinkingAIs_.erase(thinkingItor);
				break;
			}
		}
	}	
}

void ServerTurnsFree::moveFinished(ComsPlayedMoveMessage &playedMessage)
{
	unsigned int playerId = playedMessage.getPlayerId();
	unsigned int moveId = playedMessage.getMoveId();

	Tank *tank = ScorchedServer::instance()->getTankContainer().getTankById(playerId);
	if (!tank || tank->getState().getMoveId() != moveId) return;
	
	playMoveFinished(tank);

	PlayMovesSimAction *movesAction = new PlayMovesSimAction(nextMoveId_, false, false);
	movesAction->addMove(new ComsPlayedMoveMessage(playedMessage));
	
	ScorchedServer::instance()->getServerSimulator().
		addSimulatorAction(movesAction);
}

void ServerTurnsFree::shotsFinished(unsigned int moveId)
{

}

bool ServerTurnsFree::finished()
{
	return ServerTurns::showScore();
}
