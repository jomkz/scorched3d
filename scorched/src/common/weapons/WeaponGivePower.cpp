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

#include <weapons/WeaponGivePower.h>
#include <weapons/AccessoryStore.h>
#include <engine/ActionController.h>
#include <tank/TankContainer.h>
#include <tank/TankPosition.h>
#include <common/Defines.h>
#include <common/ChannelManager.h>
#include <lang/LangResource.h>

REGISTER_ACCESSORY_SOURCE(WeaponGivePower);

WeaponGivePower::WeaponGivePower()
{

}

WeaponGivePower::~WeaponGivePower()
{

}

bool WeaponGivePower::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(context, accessoryNode)) return false;

	if (!accessoryNode->getNamedChild("power", power_)) return false;

	return true;
}

void WeaponGivePower::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	context.getActionController().addAction(
		new CallbackWeapon("WeaponGivePower", this, 0, 0, 
			weaponContext, position, velocity));
}

void WeaponGivePower::weaponCallback(
	ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity,
	unsigned int userData)
{
	Tank *tank = context.getTankContainer().getTankById(weaponContext.getPlayerId());
	if (!tank) return;

	fixed power = power_.getValue(context);
	tank->getPosition().setMaxPower(
		MAX(tank->getPosition().getMaxPower(), power));

	{
		ChannelText text("combat", 
			LANG_RESOURCE_2("TANK_GET_POWER",
			"[p:{0}] received {1} power", 
			tank->getTargetName(), 
			S3D::formatStringBuffer("%.0f", power.asFloat())));
		ChannelManager::showText(context, text);
	}
}