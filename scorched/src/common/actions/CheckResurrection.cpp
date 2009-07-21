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

#include <actions/CheckResurrection.h>
#include <actions/Resurrection.h>
#include <engine/ActionController.h>
#include <engine/Simulator.h>
#include <placement/PlacementTankPosition.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>

CheckResurrection::CheckResurrection() :
	Action(-1),
	firstTime_(true)
{
}

CheckResurrection::~CheckResurrection()
{
}

void CheckResurrection::init()
{
}

void CheckResurrection::simulate(fixed frameTime, bool &remove)
{
	if (firstTime_)
	{
		firstTime_ = false;

		std::map<unsigned int, Tank *> &tanks =
			context_->getTankContainer().getPlayingTanks();
		std::map<unsigned int, Tank *>::iterator itor;
		for (itor = tanks.begin();
			itor != tanks.end();
			itor++)
		{
			Tank *tank = (*itor).second;

			// Check for any dead tanks that can be rezed
			if (tank->getState().getState() == TankState::sDead &&
				(tank->getState().getLives() > 0 ||
				tank->getState().getMaxLives() == 0))
			{
				FixedVector tankPos = PlacementTankPosition::placeTank(
					tank->getPlayerId(), tank->getTeam(),
					*context_,
					context_->getSimulator().getRandomGenerator());

				Resurrection *rez = new Resurrection(
					tank->getPlayerId(), tankPos);
				context_->getActionController().addAction(rez);
			}
		}	
	}

	remove = true;
	Action::simulate(frameTime, remove);
}
