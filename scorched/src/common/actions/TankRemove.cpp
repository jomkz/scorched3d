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

#include <actions/TankRemove.h>
#include <tank/TankContainer.h>
#include <common/Logger.h>

TankRemove::TankRemove(unsigned int playerId, fixed removeTime) :
	Action(playerId),
	removeTime_(removeTime),
	playerId_(playerId)
{
}

TankRemove::~TankRemove()
{
}

void TankRemove::init()
{
}

void TankRemove::simulate(fixed frameTime, bool &remove)
{
	removeTime_ -= frameTime;

	if (removeTime_ <= 0)
	{
		remove = true;

		Tank *tank = context_->getTankContainer().removeTank(playerId_);
		if (!tank) Logger::log("ERROR: Failed to find player to remove");
		delete tank;
	}

	Action::simulate(frameTime, remove);
}

std::string TankRemove::getActionDetails()
{
	return S3D::formatStringBuffer("%u %u", playerId_, removeTime_.getInternal());
}
