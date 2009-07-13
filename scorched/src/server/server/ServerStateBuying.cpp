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
#include <common/OptionsScorched.h>
#include <simactions/TankStartMoveSimAction.h>

ServerStateBuying::ServerStateBuying() : 
	finished_(false)
{
}

ServerStateBuying::~ServerStateBuying()
{
}

void ServerStateBuying::enterState()
{
	float buyingTime = (float)
		ScorchedServer::instance()->getOptionsGame().getBuyingTime();	

	finished_ = false;
	simulTurns_.clear();
	std::map<unsigned int, Tank*> &tanks = 
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank*>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		simulTurns_.addPlayer(itor->first, buyingTime);
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

void ServerStateBuying::playerPlaying(unsigned int playerId, float timeout)
{
	TankStartMoveSimAction *tankSimAction = new TankStartMoveSimAction(playerId, true);
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(tankSimAction);
}
