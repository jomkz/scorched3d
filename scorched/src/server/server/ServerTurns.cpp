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

#include <server/ServerTurns.h>
#include <server/ScorchedServer.h>
#include <server/ServerSimulator.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <simactions/TankAliveSimAction.h>
#include <simactions/TankStartMoveSimAction.h>

ServerTurns *ServerTurns::instance_ = 0;

ServerTurns *ServerTurns::instance()
{
	if (!instance_) instance_ = new ServerTurns();
	return instance_;
}

ServerTurns::ServerTurns()
{
}

ServerTurns::~ServerTurns()
{
}

void ServerTurns::simulate()
{
	std::map<unsigned int, Tank *>::iterator itor;
	std::map<unsigned int, Tank *> &tanks = 
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		// For each tank
		Tank *tank = (*itor).second;
		processTank(tank);
	}
}

void ServerTurns::processTank(Tank *tank)
{
	switch (tank->getState().getState())
	{
	case TankState::sDead:
		switch (tank->getState().getServerState())
		{
		case TankState::serverJoined:
			tank->getState().setServerState(TankState::serverNone);
			if (true) // Check that we can join game
			{
				TankAliveSimAction *tankAliveSimAction = new TankAliveSimAction(tank->getPlayerId());
				ScorchedServer::instance()->getServerSimulator().addSimulatorAction(tankAliveSimAction);
			}
			break;
		}
		break;
	case TankState::sNormal:
		switch (tank->getState().getServerState())
		{
		case TankState::serverNone:
			tank->getState().setServerState(TankState::serverMakingMove);
			TankStartMoveSimAction *tankSimAction = new TankStartMoveSimAction(tank->getPlayerId());
			ScorchedServer::instance()->getServerSimulator().addSimulatorAction(tankSimAction);
		}
		break;
	}
}