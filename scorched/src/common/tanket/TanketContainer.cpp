////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011
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
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
////////////////////////////////////////////////////////////////////////////////

#include <tanket/TanketContainer.h>
#include <tank/TankState.h>

TanketContainer::TanketContainer(TargetContainer &targets) : 
	targets_(targets)
{

}

TanketContainer::~TanketContainer()
{
	tankets_.clear();
}

void TanketContainer::addTanket(Tanket *tanket)
{
	targets_.internalAddTarget(tanket);
	tankets_[tanket->getPlayerId()] = tanket;
}

Tanket *TanketContainer::removeTanket(unsigned int playerId)
{
	Target *target = targets_.internalRemoveTarget(playerId);
	if (target)
	{
		tankets_.erase(playerId);
	}
	return (Tanket *) target;
}

Tanket *TanketContainer::getTanketById(unsigned int id)
{
	std::map<unsigned int, Tanket *>::iterator findItor =
		tankets_.find(id);
	if (findItor != tankets_.end())
	{
		return (*findItor).second;
	}
	return 0;
}

std::map<unsigned int, Tanket *> &TanketContainer::getAllTankets()
{
	return tankets_;
}
