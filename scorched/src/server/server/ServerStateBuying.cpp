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
#include <tank/TankDeadContainer.h>
#include <tankai/TankAI.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <simactions/TankStartMoveSimAction.h>
#include <simactions/TankStopMoveSimAction.h>

ServerStateBuying::ServerStateBuying() :
	nextMoveId_(0)
{
}

ServerStateBuying::~ServerStateBuying()
{
}

void ServerStateBuying::enterState()
{
	joinedPlayers_.clear();
	boughtPlayers_.clear();
	totalTime_ = 0;

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
	std::set<unsigned int> playingDestinations;
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
			// Check if this a new match for the tank
			if (firstRound || tank->getState().getNewlyJoined())
			{
				if (tank->getState().getNewlyJoined() ||
					joinedPlayers_.find(tank->getPlayerId()) == joinedPlayers_.end()) 
				{	
					joinedPlayers_.insert(tank->getPlayerId());
					boughtPlayers_.erase(tank->getPlayerId());
					tank->getState().setNewlyJoined(false);

					TankNewMatchSimAction *tankNewMatchAction = 
						new TankNewMatchSimAction(tank->getPlayerId());
					ScorchedServer::instance()->getTankDeadContainer().getDeadTank(tank, tankNewMatchAction);
					ScorchedServer::instance()->getServerSimulator().addSimulatorAction(tankNewMatchAction);
				}
			}

			// Check if this tank should buy
			if (buying)
			{
				if (tank->getState().getMoveId() == 0)
				{
					if (boughtPlayers_.find(tank->getPlayerId()) == boughtPlayers_.end() &&
						(tank->getDestinationId() == 0 ||
						playingDestinations.find(tank->getDestinationId()) == playingDestinations.end()))
					{
						playingDestinations.insert(tank->getDestinationId());
						playerBuying(tank->getPlayerId());
					}
				}
				else
				{
					playingDestinations.insert(tank->getDestinationId());
				}

			}
		} 
		else if (tank->getState().getState() == TankState::sLoading) 
		{
			loading = true;
		}
	}

	// Check if all the tanks have made their moves
	if (playingDestinations.empty()) 
	{
		if (!loading || timeExpired)
		{
			return true;
		}
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

	boughtPlayers_.insert(tank->getPlayerId());
	tank->getState().setMoveId(0);

	TankStopMoveSimAction *tankSimAction = 
		new TankStopMoveSimAction(playerId);
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(tankSimAction);	
}
