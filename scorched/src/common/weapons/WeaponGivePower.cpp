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

#include <weapons/WeaponGivePower.h>
#include <weapons/AccessoryStore.h>
#include <engine/ActionController.h>
#include <tank/Tank.h>
#include <target/TargetContainer.h>
#include <tanket/TanketShotInfo.h>
#include <common/Defines.h>
#include <common/ChannelManager.h>
#include <lang/LangResource.h>

REGISTER_ACCESSORY_SOURCE(WeaponGivePower);

WeaponGivePower::WeaponGivePower() :
	WeaponCallback("WeaponGivePower", "Sets the player's maximum shot power to the specified amount. "),
	power_("WeaponGivePower::power", "What to set the player's max shot power to.  Must be set over 1000 to give a maximum power above 1000")
{
	addChildXMLEntry("power", &power_)
}

WeaponGivePower::~WeaponGivePower()
{

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
	Tank *tank = context.getTargetContainer().getTankById(weaponContext.getPlayerId());
	if (!tank) return;

	fixed power = power_.getValue(context);
	tank->getShotInfo().setMaxPower(
		S3D_MAX(tank->getShotInfo().getMaxPower(), power));

	{
		ChannelText text("combat", 
			LANG_RESOURCE_2("TANK_GET_POWER",
			"[p:{0}] received {1} power", 
			tank->getTargetName(), 
			S3D::formatStringBuffer("%.0f", power.asFloat())));
		ChannelManager::showText(context, text);
	}
}
