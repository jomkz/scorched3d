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

ServerTurnsSimultaneous::ServerTurnsSimultaneous() :
	nextMoveId_(0), playingMoves_(false)
{
}

ServerTurnsSimultaneous::~ServerTurnsSimultaneous()
{
}


void ServerTurnsSimultaneous::enterState()
{
	nextMoveId_++;
	waitingPlayers_.clear();
	playingPlayers_.clear();

	{
		std::map<unsigned int, ComsPlayedMoveMessage*>::iterator itor;
		for (itor = moves_.begin();
			itor != moves_.end();
			itor++)
		{
			ComsPlayedMoveMessage *message = itor->second;
			delete message;
		}
		moves_.clear();
	}

	{
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
				waitingPlayers_.insert(tank->getPlayerId());
			}
		}
	}
}

void ServerTurnsSimultaneous::simulate()
{
	if (playingMoves_) return;

	// Check if all the tanks have made their moves
	if (waitingPlayers_.empty() && playingPlayers_.empty()) 
	{
		playShots();
		return;
	}

	// Check if any of the playing tanks have timed out
	// or if they have left the game
	std::set<unsigned int> playingDestinations;
	std::list<unsigned int> removePlaying;
	std::set<unsigned int>::iterator playItor;
	for (playItor = playingPlayers_.begin();
		playItor != playingPlayers_.end();
		playItor++)
	{
		unsigned int playerId = *playItor;
		Tank *tank =
			ScorchedServer::instance()->getTankContainer().getTankById(playerId);
		if (!tank || tank->getState().getState() != TankState::sNormal)
		{
			removePlaying.push_back(playerId);
			if (tank && tank->getState().getMoveId() != 0)
			{
				playMoveFinished(tank);
			}
		}
		else if (tank->getDestinationId() != 0)
		{
			playingDestinations.insert(tank->getDestinationId());
		}
	}

	// Remove any that are not playing
	std::list<unsigned int>::iterator removeItor;
	for (removeItor = removePlaying.begin();
		removeItor != removePlaying.end();
		removeItor++)
	{
		unsigned int playerId = *removeItor;
		playingPlayers_.erase(playerId);
	}

	// Add any waiting tanks to the game (if possible)
	std::set<unsigned int>::iterator waitingItor;
	std::list<unsigned int> removeWaiting;
	for (waitingItor = waitingPlayers_.begin();
		waitingItor != waitingPlayers_.end();
		waitingItor++)
	{
		unsigned int playerId = *waitingItor;
		Tank *tank =
			ScorchedServer::instance()->getTankContainer().getTankById(playerId);
		if (!tank || tank->getState().getState() != TankState::sNormal)
		{
			removeWaiting.push_back(playerId);
		}
		else if (playingDestinations.find(tank->getDestinationId()) == playingDestinations.end())
		{
			playingDestinations.insert(tank->getDestinationId());
			playingPlayers_.insert(playerId);
			makeMove(tank);
			removeWaiting.push_back(playerId);
		}
	}

	// Remove any that are not waiting
	for (removeItor = removeWaiting.begin();
		removeItor != removeWaiting.end();
		removeItor++)
	{
		unsigned int playerId = *removeItor;
		waitingPlayers_.erase(playerId);
	}
}

void ServerTurnsSimultaneous::makeMove(Tank *tank)
{
	fixed shotTime = fixed(
		ScorchedServer::instance()->getOptionsGame().getShotTime());
	playMove(tank, nextMoveId_, shotTime);
}

void ServerTurnsSimultaneous::moveFinished(ComsPlayedMoveMessage &playedMessage)
{
	unsigned int playerId = playedMessage.getPlayerId();
	unsigned int moveId = playedMessage.getMoveId();

	std::set<unsigned int>::iterator itor =
		playingPlayers_.find(playerId);
	if (itor == playingPlayers_.end()) return;
	Tank *tank = ScorchedServer::instance()->getTankContainer().getTankById(playerId);
	if (!tank || tank->getState().getMoveId() != moveId) return;
	
	playMoveFinished(tank);
	playingPlayers_.erase(itor);

	if (moves_.find(playerId) == moves_.end())
	{
		moves_[playerId] = new ComsPlayedMoveMessage(playedMessage);
	}
}

void ServerTurnsSimultaneous::playShots()
{
	PlayMovesSimAction *movesAction = new PlayMovesSimAction(nextMoveId_, true, true);

	std::map<unsigned int, ComsPlayedMoveMessage*>::iterator itor;
	for (itor = moves_.begin();
		itor != moves_.end();
		itor++)
	{
		ComsPlayedMoveMessage *message = itor->second;
		movesAction->addMove(message);
	}
	moves_.clear();
	
	playingMoves_ = true;
	ScorchedServer::instance()->getServerSimulator().
		addSimulatorAction(movesAction);
}

void ServerTurnsSimultaneous::shotsFinished(unsigned int moveId)
{
	if (nextMoveId_ != moveId) return;
	playingMoves_ = false;

	enterState();
}

bool ServerTurnsSimultaneous::finished()
{
	return ServerTurns::showScore();
}
