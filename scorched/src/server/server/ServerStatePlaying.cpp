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

#include <server/ServerStatePlaying.h>
#include <server/ScorchedServer.h>
#include <server/ServerSimulator.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <common/OptionsScorched.h>
#include <simactions/TankStartMoveSimAction.h>
#include <simactions/PlayMovesSimAction.h>

unsigned int ServerStatePlaying::moveId_ = 0;

ServerStatePlaying::ServerStatePlaying() : 
	simulatingShots_(false), turns_(0)
{
	turnsSequential_.setUser(this);
	turnsSimultaneous_.setUser(this);
}

ServerStatePlaying::~ServerStatePlaying()
{
}

bool ServerStatePlaying::showScore()
{
	if (!simulatingShots_)
	{
		int normal = 0;
		std::map<unsigned int, Tank *> &playingTanks = 
			ScorchedServer::instance()->getTankContainer().getPlayingTanks();
		std::map<unsigned int, Tank *>::iterator mainitor;
		for (mainitor = playingTanks.begin();
			 mainitor != playingTanks.end();
			 mainitor++)
		{
			Tank *current = (*mainitor).second;
			if (current->getState().getState() == TankState::sNormal)
			{
				normal++;
			}
		}
		return (normal <= 1);
	}

	return false;
}

void ServerStatePlaying::enterState()
{
	switch (ScorchedServer::instance()->getOptionsGame().getTurnType().getValue())
	{
	case OptionsGame::TurnSequentialLooserFirst:
	case OptionsGame::TurnSequentialRandom:
		turns_ = &turnsSequential_;
		break;
	default:
		turns_ = &turnsSimultaneous_;
		break;
	}

	clear();
	turns_->newGame();
}

void ServerStatePlaying::clear()
{
	simulatingShots_ = false;
	moveId_++;

	std::map<unsigned int, ComsPlayedMoveMessage *>::iterator messageItor;
	for (messageItor = messages_.begin();
		messageItor != messages_.end();
		messageItor++)
	{
		delete messageItor->second;
	}
	messages_.clear();
}

void ServerStatePlaying::simulate()
{
	if (simulatingShots_)
	{
	}
	else
	{
		simulatePlaying();
	}
}

void ServerStatePlaying::simulatePlaying()
{
	std::list<unsigned int> removeIds;
	std::map<unsigned int, ComsPlayedMoveMessage *>::iterator messageItor;
	for (messageItor = messages_.begin();
		messageItor != messages_.end();
		messageItor++)
	{
		unsigned int playerId = messageItor->first;
		Tank *tank = ScorchedServer::instance()->getTankContainer().getTankById(playerId);
		if (!tank) removeIds.push_back(playerId);
	}	
	while (!removeIds.empty())
	{
		unsigned int playerId = removeIds.back();
		removeIds.pop_back();
		delete messages_[playerId];
		messages_.erase(playerId);
	}

	turns_->simulate();
}

void ServerStatePlaying::playShots()
{
	simulatingShots_ = true;

	PlayMovesSimAction *movesAction = new PlayMovesSimAction(moveId_);

	std::map<unsigned int, ComsPlayedMoveMessage *>::iterator messageItor;
	for (messageItor = messages_.begin();
		messageItor != messages_.end();
		messageItor++)
	{
		ComsPlayedMoveMessage *playedMessage = messageItor->second;
		movesAction->addMove(playedMessage);
	}
	messages_.clear();

	ScorchedServer::instance()->getServerSimulator().
		addSimulatorAction(movesAction);
}

void ServerStatePlaying::playMoves()
{
	playShots();
}

void ServerStatePlaying::shotsFinished(unsigned int moveId)
{
	if (moveId_ != moveId) return;

	clear();
	turns_->nextMove();
}

void ServerStatePlaying::moveFinished(ComsPlayedMoveMessage &playedMessage)
{
	unsigned int playerId = playedMessage.getPlayerId();
	unsigned int moveId = playedMessage.getMoveId();
	Tank *tank = ScorchedServer::instance()->getTankContainer().getTankById(playerId);
	if (!tank || tank->getState().getState() != TankState::sNormal) return;
	if (moveId_ != moveId) return;

	if (turns_->playerFinished(playerId))
	{
		if (messages_.find(playerId) == messages_.end())
		{
			messages_[playerId] = new ComsPlayedMoveMessage(playedMessage);
		}

		tank->getScore().setMissedMoves(0);
		tank->getState().setServerState(TankState::serverNone);
	}	
}

void ServerStatePlaying::playerPlaying(unsigned int playerId)
{
	Tank *tank = ScorchedServer::instance()->getTankContainer().getTankById(playerId);
	float shotTime = (float)
		ScorchedServer::instance()->getOptionsGame().getShotTime();

	tank->getState().setServerState(TankState::serverMakingMove);
	TankStartMoveSimAction *tankSimAction = new TankStartMoveSimAction(
		tank->getPlayerId(), moveId_, shotTime, false);
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(tankSimAction);
}
