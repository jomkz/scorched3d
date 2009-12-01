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
	ServerTurns(false),
	nextMoveId_(0)
{
}

ServerTurnsFree::~ServerTurnsFree()
{
}


void ServerTurnsFree::internalEnterState()
{
	nextMoveId_++;

	waitingPlayers_.clear();
	timedPlayers_.clear();

	std::map<unsigned int, Tank*> &tanks = 
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank*>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		Tank *tank = itor->second;
		tank->getState().setMoveId(0);
		if (tank->getState().getState() == TankState::sNormal)
		{
			waitingPlayers_.push_back(tank->getPlayerId());
		}
	}	
}

void ServerTurnsFree::internalSimulate(fixed frameTime)
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
		if (tank->getState().getMoveId() != 0)
		{
			if (tank->getState().getState() == TankState::sNormal)
			{
				if (tank->getDestinationId() != 0)
				{
					playingDestinations.insert(tank->getDestinationId());
				}
			}
			else
			{
				playMoveFinished(tank);
			}
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
				playingDestinations.end() &&
				timedPlayers_.find(tank->getPlayerId()) ==
				timedPlayers_.end())
			{
				if (tank->getDestinationId() != 0)
				{
					playingDestinations.insert(tank->getDestinationId());
				}

				playMove(tank, ++nextMoveId_);

				waitingPlayers_.erase(waitingItor);
				waitingPlayers_.push_back(playerId);
				break;
			}
		}
	}

	// Let the AIs play
	std::map<unsigned int, fixed>::iterator timedItor;
	for (timedItor = timedPlayers_.begin();
		timedItor != timedPlayers_.end();
		timedItor++)
	{
		unsigned int playerId = timedItor->first;

		timedItor->second -= frameTime;
		if (timedItor->second <= 0)
		{
			timedPlayers_.erase(timedItor);
			break;
		}
	}	
}

void ServerTurnsFree::internalMoveFinished(ComsPlayedMoveMessage &playedMessage)
{
	unsigned int playerId = playedMessage.getPlayerId();
	unsigned int moveId = playedMessage.getMoveId();

	Tank *tank = ScorchedServer::instance()->getTankContainer().getTankById(playerId);
	if (!tank || tank->getState().getMoveId() != moveId) return;
	
	playMoveFinished(tank);

	int thinkingTime = 0;
	int shotTime = ScorchedServer::instance()->getOptionsGame().getShotTime();
	if (ScorchedServer::instance()->getOptionsGame().getTurnType() ==
		OptionsGame::TurnFreeTimed)
	{
		thinkingTime = shotTime;
	}
	else
	{
		if (tank->getDestinationId() == 0)
		{
			thinkingTime = (shotTime / 3) + (rand() % (shotTime / 2));
		}
	}
	
	if (thinkingTime > 0)
	{
		timedPlayers_[tank->getPlayerId()] = fixed(thinkingTime);
	}

	if (tank->getState().getState() == TankState::sNormal)
	{
		std::list<ComsPlayedMoveMessage*> messages;
		messages.push_back(new ComsPlayedMoveMessage(playedMessage));
		playShots(messages, nextMoveId_, false);
	}
}
