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

#include <server/ServerTurnsSimultaneous.h>
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

ServerTurnsSimultaneous::ServerTurnsSimultaneous(bool waitForShots) :
	ServerTurns(waitForShots),
	nextMoveId_(0)
{
}

ServerTurnsSimultaneous::~ServerTurnsSimultaneous()
{
	std::map<unsigned int, ComsPlayedMoveMessage*>::iterator itor;
	for (itor = moves_.begin();
		itor != moves_.end();
		itor++)
	{
		delete itor->second;
	}
	moves_.clear();
}

void ServerTurnsSimultaneous::internalEnterState()
{
	nextMoveId_++;

	std::map<unsigned int, ComsPlayedMoveMessage*>::iterator movesItor;
	for (movesItor = moves_.begin();
		movesItor != moves_.end();
		movesItor++)
	{
		delete movesItor->second;
	}
	moves_.clear();

	std::map<unsigned int, Tank*> &tanks = 
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank*>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		Tank *tank = itor->second;
		tank->getState().setMoveId(0);
	}	
}

void ServerTurnsSimultaneous::internalShotsFinished()
{
	incrementTurn();
}

void ServerTurnsSimultaneous::internalSimulate(fixed frameTime)
{
	// Build list of currently playing destinations
	std::set<unsigned int> playingDestinations;
	std::map<unsigned int, Tank*> &tanks = 
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank*>::iterator itor;

	bool finished = true;
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

		if (tank->getState().getState() == TankState::sNormal)
		{
			if (moves_.find(tank->getPlayerId()) == moves_.end())
			{
				finished = false;
			}
		}
	}
	
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		Tank *tank = itor->second;
		if (tank->getState().getState() == TankState::sNormal &&
			tank->getState().getMoveId() == 0)
		{
			if (moves_.find(tank->getPlayerId()) == moves_.end())
			{
				if (playingDestinations.find(tank->getDestinationId()) == 
					playingDestinations.end())
				{
					fixed delayShotTime = 0;
					if (tank->getDestinationId() != 0)
					{
						playingDestinations.insert(tank->getDestinationId());
					}
					else
					{
						if (!waitForShots_)
						{
							// Add some thinking time on the AIs shots
							delayShotTime = fixed(ScorchedServer::instance()->getOptionsGame().getAIShotTime());
							delayShotTime -= fixed(true, rand() % 50000);
							if (delayShotTime < 0) delayShotTime = 0;
						}
					}

					fixed shotTime = fixed(
						ScorchedServer::instance()->getOptionsGame().getShotTime());
					playMove(tank, ++nextMoveId_, shotTime, delayShotTime);
				}
			}
		}
	}

	if (finished)
	{
		std::list<ComsPlayedMoveMessage*> messages;
		std::map<unsigned int, ComsPlayedMoveMessage*>::iterator movesItor;
		for (movesItor = moves_.begin();
			movesItor != moves_.end();
			movesItor++)
		{
			unsigned int playerId = movesItor->first;
			ComsPlayedMoveMessage *message = movesItor->second;

			Tank *tank = ScorchedServer::instance()->
				getTankContainer().getTankById(playerId);
			if (tank && tank->getState().getState() == TankState::sNormal)
			{
				messages.push_back(message);
			}
			else
			{
				delete message;
			}
		}
		moves_.clear();
		playShots(messages, nextMoveId_, true);
		if (!waitForShots_)
		{
			incrementTurn();
		}
	}
}

void ServerTurnsSimultaneous::internalMoveFinished(ComsPlayedMoveMessage &playedMessage)
{
	unsigned int playerId = playedMessage.getPlayerId();
	unsigned int moveId = playedMessage.getMoveId();

	Tank *tank = ScorchedServer::instance()->getTankContainer().getTankById(playerId);
	if (!tank || tank->getState().getMoveId() != moveId) return;
	if (moves_.find(playerId) != moves_.end()) return;
	
	playMoveFinished(tank);
	
	moves_[playerId] = new ComsPlayedMoveMessage(playedMessage);
}
