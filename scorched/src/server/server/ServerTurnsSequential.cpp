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
#include <server/ServerSimulator.h>
#include <common/OptionsGame.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <simactions/PlayMovesSimAction.h>
#include <simactions/TankStopMoveSimAction.h>
#include <coms/ComsPlayedMoveMessage.h>
#include <tank/TankSort.h>
#include <tank/TankScore.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <list>

ServerTurnsSequential::ServerTurnsSequential() :
	playingPlayer_(0), nextMoveId_(0), playingMoves_(false)
{
}

ServerTurnsSequential::~ServerTurnsSequential()
{
}

void ServerTurnsSequential::enterState()
{
	playingMoves_ = false;
	playingPlayer_ = 0;
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

void ServerTurnsSequential::simulate()
{
	if (playingMoves_) return;

	// Check if all the tanks have made their moves
	if (waitingPlayers_.empty() && playingPlayer_ == 0) 
	{
		enterState();
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
				makeMove(waitingTank);
				break;
			}
		}
	}
}

void ServerTurnsSequential::makeMove(Tank *tank)
{
	nextMoveId_++;
	playMove(tank, nextMoveId_);
}

void ServerTurnsSequential::moveFinished(ComsPlayedMoveMessage &playedMessage)
{
	unsigned int playerId = playedMessage.getPlayerId();
	unsigned int moveId = playedMessage.getMoveId();
	if (playerId != playingPlayer_) return;
	if (!playMoveFinished(playedMessage)) return;

	playingPlayer_ = 0;

	playingMoves_ = true;
	PlayMovesSimAction *movesAction = new PlayMovesSimAction(moveId);
	movesAction->addMove(new ComsPlayedMoveMessage(playedMessage));
	ScorchedServer::instance()->getServerSimulator().
		addSimulatorAction(movesAction);
}

void ServerTurnsSequential::shotsFinished(unsigned int moveId)
{
	if (nextMoveId_ != moveId) return;

	playingMoves_ = false;
}

bool ServerTurnsSequential::finished()
{
	if (playingMoves_) return false;

	return ServerTurns::showScore();
}
