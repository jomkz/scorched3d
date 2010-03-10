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

#include <simactions/TankNewGameSimAction.h>
#include <placement/PlacementTankPosition.h>
#include <landscapemap/DeformLandscape.h>
#include <engine/Simulator.h>
#include <tank/TankState.h>
#include <tank/TankContainer.h>
#include <target/TargetLife.h>
#ifndef S3D_SERVER
	#include <landscape/Landscape.h>
	#include <land/VisibilityPatchGrid.h>
#endif

REGISTER_CLASS_SOURCE(TankNewGameSimAction);

TankNewGameSimAction::TankNewGameSimAction() :
	playerId_(0)
{
}

TankNewGameSimAction::TankNewGameSimAction(unsigned int playerId) :
	playerId_(playerId)
{
}

TankNewGameSimAction::~TankNewGameSimAction()
{
}

bool TankNewGameSimAction::invokeAction(ScorchedContext &context)
{
	Tank *tank = context.getTankContainer().getTankById(playerId_);
	if (!tank) return false;

	FixedVector tankPos = PlacementTankPosition::placeTank(
		tank->getPlayerId(), tank->getTeam(),
		context,
		context.getSimulator().getRandomGenerator());
	tank->getLife().setTargetPosition(tankPos);
	DeformLandscape::flattenArea(context, tankPos);
#ifndef S3D_SERVER
	if (!context.getServerMode())
	{
		Landscape::instance()->recalculate();
		VisibilityPatchGrid::instance()->recalculateErrors(tankPos, 2);
	}
#endif

	tank->newGame();
	if (!context.getServerMode()) tank->clientNewGame();

	return true;
}

bool TankNewGameSimAction::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(playerId_);
	return true;
}

bool TankNewGameSimAction::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(playerId_)) return false;
	return true;
}