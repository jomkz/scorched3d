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

#include <server/ServerStateBuying.h>
#include <server/ScorchedServer.h>
#include <server/ServerSimulator.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <tank/TankAccessories.h>
#include <tankai/TankAI.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <simactions/TankStartMoveSimAction.h>
#include <simactions/TankStopMoveSimAction.h>
#include <simactions/TankAliveSimAction.h>

ServerStateBuying::ServerStateBuying() :
	nextMoveId_(0)
{
}

ServerStateBuying::~ServerStateBuying()
{
}

void ServerStateBuying::enterState()
{
	waitingPlayers_.clear();
	playingPlayers_.clear();
	totalTime_ = 0;
}

bool ServerStateBuying::simulate(fixed frameTime)
{
	// Check options
	bool firstRound =
		(ScorchedServer::instance()->getOptionsTransient().getCurrentRoundNo() == 1);
	bool buying =
		(ScorchedServer::instance()->getOptionsTransient().getCurrentRoundNo() >=
		ScorchedServer::instance()->getOptionsGame().getBuyOnRound());
	if (ScorchedServer::instance()->getOptionsGame().getGiveAllWeapons())
	{
		buying = false;
	}
	fixed buyingTime(ScorchedServer::instance()->getOptionsGame().getBuyingTime());
	bool timeExpired = false;
#ifdef S3D_SERVER
	if (buyingTime != 0)
	{
		totalTime_ += frameTime;
		if (totalTime_ > buyingTime) timeExpired = true;
	}
#endif

	// Add any new players that should be buying
	bool loading = false;
	std::map<unsigned int, Tank*> &tanks = 
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank*>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		Tank *tank = itor->second;
		if (tank->getState().getState() == TankState::sDead)
		{						
			// Add tank to game
			tank->getState().setState(TankState::sNormal);
			TankAliveSimAction *tankAliveSimAction = 
				new TankAliveSimAction(tank->getPlayerId(), firstRound);
			ScorchedServer::instance()->getServerSimulator().addSimulatorAction(tankAliveSimAction);

			if (buying)
			{
				// Add tank to list of tanks to get buying for
				waitingPlayers_.insert(tank->getPlayerId());
			}
		} 
		else if (tank->getState().getState() == TankState::sLoading) 
		{
			loading = true;
		}
	}

	// Check if all the tanks have made their moves
	if (waitingPlayers_.empty() && playingPlayers_.empty()) 
	{
		if (!loading || timeExpired)
		{
			return true;
		}
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
			// There is a player waiting that has no other player playing
			// from the same destination
			// Let this player play
			playingDestinations.insert(tank->getDestinationId());
			playingPlayers_.insert(playerId);
			removeWaiting.push_back(playerId);

			playerBuying(playerId);
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

	return false;
}

void ServerStateBuying::playerBuying(unsigned int playerId)
{
	nextMoveId_++;

	Tank *tank = ScorchedServer::instance()->getTankContainer().getTankById(playerId);
	tank->getState().setMoveId(nextMoveId_);

	fixed buyingTime
		(ScorchedServer::instance()->getOptionsGame().getBuyingTime());
#ifdef S3D_SERVER
	if (buyingTime != 0)
	{
		buyingTime = buyingTime - totalTime_;
		if (buyingTime < 1) buyingTime = 1;
	}
#endif

	TankStartMoveSimAction *tankSimAction = 
		new TankStartMoveSimAction(playerId, nextMoveId_, buyingTime, true);
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(tankSimAction);
}

void ServerStateBuying::buyingFinished(ComsPlayedMoveMessage &playedMessage)
{
	unsigned int playerId = playedMessage.getPlayerId();
	unsigned int moveId = playedMessage.getMoveId();
	Tank *tank = ScorchedServer::instance()->getTankContainer().getTankById(playerId);
	if (!tank || !tank->getState().getTankPlaying()) return;
	if (moveId != tank->getState().getMoveId()) return;
	std::set<unsigned int>::iterator itor =
		playingPlayers_.find(playerId);
	if (itor == playingPlayers_.end()) return;

	playingPlayers_.erase(itor);
	tank->getState().setMoveId(0);

	TankStopMoveSimAction *tankSimAction = 
		new TankStopMoveSimAction(playerId);
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(tankSimAction);	
}
