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

#include <weapons/WeaponGiveLives.h>
#include <weapons/AccessoryStore.h>
#include <engine/ActionController.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <common/Defines.h>
#include <common/ChannelManager.h>

REGISTER_ACCESSORY_SOURCE(WeaponGiveLives);

WeaponGiveLives::WeaponGiveLives()
{

}

WeaponGiveLives::~WeaponGiveLives()
{

}

bool WeaponGiveLives::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(context, accessoryNode)) return false;

	if (!accessoryNode->getNamedChild("lives", lives_)) return false;

	return true;
}

void WeaponGiveLives::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	context.getActionController().addAction(
		new CallbackWeapon("WeaponGiveLives", this, 0, 0, 
			weaponContext, position, velocity));
}

void WeaponGiveLives::weaponCallback(
	ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity,
	unsigned int userData)
{
	Tank *tank = context.getTankContainer().getTankById(weaponContext.getPlayerId());
	if (!tank) return;

	if (tank->getState().getMaxLives() > 0)
	{
		tank->getState().setLives(
			MAX(tank->getState().getLives() + lives_, 1));

		if (!context.getServerMode())
		{
			if (lives_ > 0)
			{
				ChannelText text("combat", 
					S3D::formatStringBuffer("[p:%s] has received %i extra live(s)", 
					tank->getName(), lives_));
				//info.setPlayerId(weaponContext.getPlayerId());
				ChannelManager::showText(text);
			}
			else
			{
				ChannelText text("combat", 
					S3D::formatStringBuffer("[p:%s] has lost %i extra live(s)", 
					tank->getName(), -lives_));
				//info.setPlayerId(weaponContext.getPlayerId());
				ChannelManager::showText(text);
			}
		}
	}
}

