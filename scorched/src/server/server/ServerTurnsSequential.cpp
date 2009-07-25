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

#include <server/ServerTurnsSequential.h>
#include <server/ScorchedServer.h>
#include <common/OptionsGame.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <tank/TankSort.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <list>

ServerTurnsSequential::ServerTurnsSequential() :
	playingPlayer_(0), playerFinished_(false)
{
}

ServerTurnsSequential::~ServerTurnsSequential()
{
}

void ServerTurnsSequential::newGame()
{
	playingPlayer_ = 0;
	playerFinished_ = false;
	waitingPlayers_.clear();

	OptionsGame::TurnType turnType = (OptionsGame::TurnType)
		ScorchedServer::instance()->getOptionsGame().getTurnType().getValue();

	// On the very first round make the order random (if looser first chosen)
	// as there is no looser yet!
	if ((ScorchedServer::instance()->getOptionsTransient().getCurrentRoundNo() == 1) &&
		turnType == OptionsGame::TurnSequentialLooserFirst)
	{
		turnType = OptionsGame::TurnSequentialRandom;
	}

	// Standard player ordering is the reverse of the tank score
	TankSort::getSortedTanksIds(
		ScorchedServer::instance()->getContext(), 
		waitingPlayers_,
		true); // All tanks
	waitingPlayers_.reverse();

	// Check for a different ordering
	if (turnType == OptionsGame::TurnSequentialRandom)
	{
		// Create zero player vector
		std::vector<unsigned int> tmpPlayers;
		for (int i=0; i<(int) waitingPlayers_.size(); i++)
		{
			tmpPlayers.push_back(0);
		}

		// Randomize list order into vector
		while (!waitingPlayers_.empty())
		{
			unsigned int player = waitingPlayers_.front();
			waitingPlayers_.pop_front();
			bool done = false;
			while (!done)
			{
				int pos = int(RAND * float(tmpPlayers.size()));
				if (pos < int(tmpPlayers.size()) && tmpPlayers[pos] == 0) 
				{
					tmpPlayers[pos] = player;
					done = true;
				}
			}
		}

		// Copy vector back to list
		for (int i=0; i<(int) tmpPlayers.size(); i++)
		{
			waitingPlayers_.push_back(tmpPlayers[i]);
		}
	}
}

void ServerTurnsSequential::nextMove()
{
}

bool ServerTurnsSequential::playerFinished(unsigned int playerId)
{
	if (playerId != playingPlayer_) return false;
	playingPlayer_ = 0;
	playerFinished_ = true;
	return true;
}

void ServerTurnsSequential::simulate()
{
	if (playerFinished_)
	{
		user_->playMoves();
		playerFinished_ = false;
		return;
	}

	// Check if all the tanks have made their moves
	if (waitingPlayers_.empty() && playingPlayer_ == 0) 
	{
		newGame();
	}

	// Check tanks are alive
	Tank *playingTank =
		ScorchedServer::instance()->getTankContainer().getTankById(playingPlayer_);
	if (!playingTank || playingTank->getState().getState() != TankState::sNormal)
	{
		playingPlayer_ = 0;
	}

	// Check if we are ready for the next player
	if (playingPlayer_ == 0)
	{
		while (!waitingPlayers_.empty())
		{
			unsigned int waitingPlayer = waitingPlayers_.front();
			waitingPlayers_.pop_front();

			Tank *waitingTank =
				ScorchedServer::instance()->getTankContainer().getTankById(waitingPlayer);
			if (waitingTank && waitingTank->getState().getState() == TankState::sNormal)
			{
				playingPlayer_ = waitingPlayer;
				user_->playerPlaying(playingPlayer_);
				break;
			}
		}
	}
}
