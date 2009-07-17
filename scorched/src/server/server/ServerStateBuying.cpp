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
#include <common/OptionsScorched.h>
#include <simactions/TankStartMoveSimAction.h>

ServerStateBuying *ServerStateBuying::instance_ = 0;

ServerStateBuying *ServerStateBuying::instance()
{
	return instance_;
}

ServerStateBuying::ServerStateBuying() : 
	finished_(false)
{
	instance_ = this;
	simulTurns_.setUser(this);
}

ServerStateBuying::~ServerStateBuying()
{
}

void ServerStateBuying::enterState()
{
	finished_ = false;
	simulTurns_.clear();
	std::map<unsigned int, Tank*> &tanks = 
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank*>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		Tank *tank = itor->second;
		if (tank->getState().getTankPlaying())
		{
			simulTurns_.addPlayer(itor->first);
		}
	}
	//if (ScorchedServer::instance()->getOptionsGame().getBuyOnRound() != 0)
	// CHECK BUY ON ROUND HERE
}

bool ServerStateBuying::simulate(float frameTime)
{
	if (finished_) return true;
	simulTurns_.simulate(frameTime);
	return false;
}

void ServerStateBuying::allPlayersFinished()
{
	finished_ = true;
}

void ServerStateBuying::playerFinishedBuying(unsigned int playerId, unsigned int moveId)
{
	simulTurns_.playerFinished(playerId, moveId);
}

void ServerStateBuying::playerPlaying(unsigned int playerId, unsigned int moveId)
{
	Tank *tank = ScorchedServer::instance()->getTankContainer().getTankById(playerId);
	float buyingTime = (float)
		ScorchedServer::instance()->getOptionsGame().getBuyingTime();

	tank->getState().setServerState(TankState::serverBuying);

	TankStartMoveSimAction *tankSimAction = 
		new TankStartMoveSimAction(playerId, moveId, buyingTime, true);
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(tankSimAction);
}
