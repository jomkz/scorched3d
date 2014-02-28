////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2013
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

#include <client/ScorchedClient.h>
#include <uiactions/UITankWeapon.h>
#include <uiactions/UITankRenderer.h>
#include <tanket/TanketAccessories.h>
#include <tanket/TanketShotInfo.h>
#include <target/TargetContainer.h>
#include <weapons/AccessoryStore.h>
#include <coms/ComsPlayedMoveMessage.h>
#include <coms/ComsMessageSender.h>
#include <tank/Tank.h>
#include <algorithm>

UITankWeaponSyncActionFireShot::UITankWeaponSyncActionFireShot(UITankShotHistory::ShotEntry &entry, 
	unsigned int playerId, unsigned int accessoryId) :
	playerId_(playerId), accessoryId_(accessoryId), entry_(entry)
{
}

UITankWeaponSyncActionFireShot::~UITankWeaponSyncActionFireShot()
{
}

void UITankWeaponSyncActionFireShot::performUIAction()
{
	Tank *tank = ScorchedClient::instance()->
		getTargetContainer().getTankById(playerId_);
	if (!tank) return;
	Accessory *currentWeapon = ScorchedClient::instance()->
		getAccessoryStore().findByAccessoryId(accessoryId_);
	if (!currentWeapon) return;
	if (!tank->getAccessories().canUse(currentWeapon)) return;

	unsigned int moveId = tank->getShotInfo().getMoveId();
	if (moveId == 0) return;

	// Send message saying we are finished with shot
	ComsPlayedMoveMessage comsMessage(
		playerId_, 
		moveId,
		ComsPlayedMoveMessage::eShot);
	comsMessage.setShot(
		accessoryId_,
		entry_.rot,
		entry_.ele,
		entry_.power,
		0,
		0);
	ComsMessageSender::sendToServer(comsMessage);
}


UITankWeapon::UITankWeapon() : currentWeapon_(0)
{
}

UITankWeapon::~UITankWeapon()
{
}

void UITankWeapon::setCurrentWeapon(Accessory *accessory)
{
	std::vector<Accessory *>::iterator findIter = 
		std::find(tankAccessories_.begin(), tankAccessories_.end(), accessory);
	if (findIter != tankAccessories_.end())
	{
		currentWeapon_ = accessory;
	}
}

Accessory *UITankWeapon::getCurrentWeapon()
{
	if (currentWeapon_ == 0 && !tankAccessories_.empty())
	{
		setCurrentWeapon(tankAccessories_[0]);
	}
	return currentWeapon_;
}

void UITankWeapon::setWeapons(Tank *tank)
{
	tankAccessories_.clear();
	std::list<Accessory *> &result =
		tank->getAccessories().getAllAccessoriesByGroup("weapon");
	std::list<Accessory *>::iterator itor;
	for (itor = result.begin();
		itor != result.end();
		++itor)
	{
		if (tank->getAccessories().canUse(*itor))
		{
			tankAccessories_.push_back(*itor);
		}
	}
	if (currentWeapon_ == 0 && !tankAccessories_.empty())
	{
		setCurrentWeapon(tankAccessories_[0]);
	}
}

void UITankWeapon::nextWeapon()
{
	if (tankAccessories_.empty()) return;
	std::vector<Accessory *>::iterator findIter = 
		std::find(tankAccessories_.begin(), tankAccessories_.end(), getCurrentWeapon());
	findIter++;
	if (findIter == tankAccessories_.end())
	{
		setCurrentWeapon(tankAccessories_[0]);
	}
	else 
	{
		setCurrentWeapon(*findIter);
	}
}

void UITankWeapon::fireWeapon()
{
	Accessory *accessory = getCurrentWeapon();
	if (!accessory) return;

	UITankWeaponSyncActionFireShot *syncAction = 
		new UITankWeaponSyncActionFireShot(
			tankRenderer_->getShotHistory().getCurrentValues(), 
			tankRenderer_->getTarget()->getPlayerId(),
			accessory->getAccessoryId());
	tankRenderer_->getShotHistory().madeShot();
	ScorchedClient::getClientUISyncExternal().addActionFromUI(syncAction);
}
