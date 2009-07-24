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

ServerStatePlaying *ServerStatePlaying::instance_ = 0;
unsigned int ServerStatePlaying::moveId_ = 0;

ServerStatePlaying *ServerStatePlaying::instance()
{
	return instance_;
}

ServerStatePlaying::ServerStatePlaying() : 
	simulatingShots_(false)
{
	instance_ = this;
	simulTurns_.setUser(this);
}

ServerStatePlaying::~ServerStatePlaying()
{
}

void ServerStatePlaying::enterState()
{
	makeMoves();
}

void ServerStatePlaying::makeMoves()
{
	simulatingShots_ = false;
	moveId_++;
	simulTurns_.clear();

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
			simulTurns_.addPlayer(itor->first);
		}
	}

	std::map<unsigned int, ComsPlayedMoveMessage *>::iterator messageItor;
	for (messageItor = messages_.begin();
		messageItor != messages_.end();
		messageItor++)
	{
		delete messageItor->second;
	}
	messages_.clear();
}

void ServerStatePlaying::simulate(float frameTime)
{
	if (simulatingShots_)
	{
	}
	else
	{
		simulatePlaying(frameTime);
	}
}

void ServerStatePlaying::simulatePlaying(float frameTime)
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

	simulTurns_.simulate(frameTime);
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

void ServerStatePlaying::allPlayersFinished()
{
	playShots();
}

void ServerStatePlaying::shotsFinished(unsigned int moveId)
{
	if (moveId_ != moveId) return;

	makeMoves();
}

void ServerStatePlaying::playerFinishedPlaying(ComsPlayedMoveMessage &playedMessage)
{
	unsigned int playerId = playedMessage.getPlayerId();
	unsigned int moveId = playedMessage.getMoveId();
	Tank *tank = ScorchedServer::instance()->getTankContainer().getTankById(playerId);
	if (!tank || tank->getState().getState() != TankState::sNormal) return;
	if (moveId_ != moveId) return;
	
	if (messages_.find(playerId) == messages_.end())
	{
		messages_[playerId] = new ComsPlayedMoveMessage(playedMessage);
	}

	tank->getScore().setMissedMoves(0);
	tank->getState().setServerState(TankState::serverNone);

	simulTurns_.playerFinished(playerId);
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
