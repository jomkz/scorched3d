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

#include <actions/Resurrection.h>
#include <engine/Simulator.h>
#include <engine/ScorchedContext.h>
#include <landscapemap/DeformLandscape.h>
#include <placement/PlacementTankPosition.h>
#include <common/ChannelManager.h>
#include <common/OptionsScorched.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <target/TargetLife.h>
#include <lang/LangResource.h>
#ifndef S3D_SERVER
	#include <land/VisibilityPatchGrid.h>
#endif

Resurrection::Resurrection(
	unsigned int playerId,
	FixedVector &position,
	fixed resurrectTime) :
	Action(playerId),
	playerId_(playerId),
	position_(position),
	resurrectTime_(resurrectTime)
{

}

Resurrection::~Resurrection()
{
}

void Resurrection::init()
{
}

void Resurrection::simulate(fixed frameTime, bool &remove)
{
	resurrectTime_ -= frameTime;

	if (resurrectTime_ < 0)
	{
		remove = true;

		Tank *tank = context_->getTankContainer().getTankById(playerId_);
		if (tank)
		{
	#ifndef S3D_SERVER
			if (!context_->getServerMode())
			{
				ChannelText text("combat",
					LANG_RESOURCE_2(
						"TANK_RESURRECTED", 
						"[p:{0}] was resurrected, {1} lives remaining",
						tank->getTargetName(),
						S3D::formatStringBuffer("%i", tank->getState().getLives())));
				ChannelManager::showText(*context_, text);
			}
	#endif

			if (context_->getOptionsGame().getActionSyncCheck())
			{
				context_->getSimulator().addSyncCheck(
					S3D::formatStringBuffer("TankRez: %u %s", 
						tank->getPlayerId(),
						position_.asQuickString()));
			}

			// Rez this tank
			tank->rezTank();
			tank->getLife().setTargetPosition(position_);
			DeformLandscape::flattenArea(*context_, position_);
	#ifndef S3D_SERVER
			if (!context_->getServerMode())
			{
				VisibilityPatchGrid::instance()->recalculateErrors(position_, 2);
			}
	#endif
		}
	}

	Action::simulate(frameTime, remove);
}

void Resurrection::checkResurection(ScorchedContext *context, Tank *tank)
{
	// Check if we can ressurect
	if (tank->getState().getLives() > 0 ||
		tank->getState().getMaxLives() == 0)
	{
		FixedVector tankPos = PlacementTankPosition::placeTank(
			tank->getPlayerId(), tank->getTeam(),
			*context,
			context->getSimulator().getRandomGenerator());

		Resurrection *rez = new Resurrection(
			tank->getPlayerId(), tankPos, 5);
		context->getActionController().addAction(rez);					
	}
}
