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

#include <simactions/TankAliveSimAction.h>
#include <placement/PlacementTankPosition.h>
#include <landscapemap/DeformLandscape.h>
#include <engine/Simulator.h>
#include <tank/TankAvatar.h>
#include <tank/TankState.h>
#include <tank/TankContainer.h>
#include <tank/TankAccessories.h>
#include <tank/TankScore.h>
#include <target/TargetLife.h>
#include <common/Logger.h>

REGISTER_CLASS_SOURCE(TankAliveSimAction);

TankAliveSimAction::TankAliveSimAction() :
	playerId_(0), newMatch_(false)
{
}

TankAliveSimAction::TankAliveSimAction(unsigned int playerId, bool newMatch) :
	playerId_(playerId), newMatch_(newMatch)
{
}

TankAliveSimAction::~TankAliveSimAction()
{
}

bool TankAliveSimAction::invokeAction(ScorchedContext &context)
{
	Tank *tank = context.getTankContainer().getTankById(playerId_);
	if (!tank) return false;

	if (newMatch_) tank->newMatch();

	FixedVector tankPos = PlacementTankPosition::placeTank(
		tank->getPlayerId(), tank->getTeam(),
		context,
		context.getSimulator().getRandomGenerator());
	tank->getLife().setTargetPosition(tankPos);
	DeformLandscape::flattenArea(context, tankPos);

	tank->newGame();
	if (!context.getServerMode()) tank->clientNewGame();

	if (scoreNetBuffer_.getBufferUsed() > 0)
	{
		NetBufferReader reader(scoreNetBuffer_);
		if (!tank->getAccessories().readMessage(reader) ||
			!tank->getScore().readMessage(reader))
		{
			Logger::log("ERROR: Failed to update residual player info (read)");
		}

		// Don't get credited for the new game stats
		tank->getScore().resetTotalEarnedStats();
	}

	return true;
}

bool TankAliveSimAction::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(playerId_);
	buffer.addToBuffer(newMatch_);
	buffer.addToBuffer(scoreNetBuffer_);
	return true;
}

bool TankAliveSimAction::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(playerId_)) return false;
	if (!reader.getFromBuffer(newMatch_)) return false;
	if (!reader.getFromBuffer(scoreNetBuffer_)) return false;
	return true;
}