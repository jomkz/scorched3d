////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
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

#include <weapons/AccessoryStore.h>
#include <weapons/WeaponMoveTank.h>
#include <tank/TankWeapon.h>
#include <tank/TankContainer.h>
#include <tank/TankAccessories.h>
#include <target/TargetLife.h>
#include <engine/ScorchedContext.h>
#include <common/Defines.h>
#include <common/ChannelManager.h>
#ifndef S3D_SERVER
	#include <client/ScorchedClient.h>
	#include <client/ClientState.h>
	#include <landscape/Landscape.h>
#endif
#include <landscapemap/LandscapeMaps.h>
#include <landscapemap/MovementMap.h>

TankWeapon::TankWeapon(ScorchedContext &context) : 
	currentWeapon_(0), context_(context),
	tank_(0)
{
}

TankWeapon::~TankWeapon()
{
}

void TankWeapon::newMatch()
{
	setCurrentWeapon(0);
}

void TankWeapon::changed()
{
	int weaponCount = tank_->getAccessories().
		getAccessoryCount(currentWeapon_);
	if (weaponCount == 0 ||
		currentWeapon_ == 0)
	{
		std::list<Accessory *> &result =
			tank_->getAccessories().getAllAccessoriesByGroup("weapon");
		if (!result.empty())
		{
			setWeapon(result.front());
		}
		else
		{
			setCurrentWeapon(0);
		}
	}
}

bool TankWeapon::setWeapon(Accessory *wp)
{
	if (tank_->getAccessories().getAccessoryCount(wp) != 0)
	{
		setCurrentWeapon(wp);
		return true;
	}
	return false;
}

Accessory *TankWeapon::getCurrent()
{
	return currentWeapon_;
}

void TankWeapon::setCurrentWeapon(Accessory *wp)
{
#ifndef S3D_SERVER
	if (!context_.getServerMode())
	{
		// Only show this information on this tanks client
		if (ScorchedClient::instance()->getTankContainer().getCurrentDestinationId() ==
			tank_->getDestinationId() &&
			ScorchedClient::instance()->getGameState().getState() == ClientState::StatePlaying)
		{

		// Turn off fuel display (if any)
		if (currentWeapon_ && 
			currentWeapon_->getPositionSelect() != Accessory::ePositionSelectNone)
		{
			Landscape::instance()->restoreLandscapeTexture();
		}
		// Turn on selection display (if any)
		if (wp &&
			wp->getPositionSelect() != Accessory::ePositionSelectNone)
		{
			if (wp->getPositionSelect() == Accessory::ePositionSelectFuel)
			{
				WeaponMoveTank *moveWeapon = (WeaponMoveTank *)
					context_.getAccessoryStore().findAccessoryPartByAccessoryId(
						wp->getAccessoryId(), "WeaponMoveTank");
				if (moveWeapon)
				{
					MovementMap mmap(
						tank_, 
						context_);
					mmap.calculateAllPositions(mmap.getFuel(moveWeapon));
					mmap.movementTexture();	
				}
			}
			else if (wp->getPositionSelect() == Accessory::ePositionSelectFuelLimit)
			{
				MovementMap mmap(
					tank_, 
					context_);
				mmap.calculateAllPositions(fixed(wp->getPositionSelectLimit()));
				mmap.movementTexture();	
			}
			else if (wp->getPositionSelect() == Accessory::ePositionSelectLimit)
			{
				MovementMap::limitTexture(tank_->getLife().getTargetPosition(), 
					wp->getPositionSelectLimit());
			}

			ChannelManager::showText(
				S3D::formatStringBuffer("Click ground to activate %s", wp->getName()));
		}

		}
	}
#endif

	currentWeapon_ = wp;
}

const char *TankWeapon::getWeaponString()
{
	if (!getCurrent()) return "";

	static char buffer[256];
	int count = tank_->getAccessories().getAccessoryCount(getCurrent());
	snprintf(buffer, 256, ((count>0)?"%s (%i)":"%s (In)"),
		getCurrent()->getName(), count);
	return buffer;
}
