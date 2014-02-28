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

#include <weapons/WeaponTankPosition.h>
#include <weapons/AccessoryStore.h>
#include <target/TargetContainer.h>
#include <tanket/Tanket.h>
#include <tanket/TanketShotInfo.h>
#include <target/TargetLife.h>
#include <tank/TankLib.h>

REGISTER_ACCESSORY_SOURCE(WeaponTankPosition);

WeaponTankPosition::WeaponTankPosition() : 
	Weapon("WeaponTankPosition", 
		"Moves the next action to the player's current position. This can be used for firing from a moving tank. "),
	sightPos_("If true, the tank's gun position is used", 0, false)
{
	addChildXMLEntry("sightpos", &sightPos_);
	addChildXMLEntry("aimedweapon", &aimedWeapon_);

}

WeaponTankPosition::~WeaponTankPosition()
{
}

void WeaponTankPosition::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	// This weapon re-centers the current shot on the firing tank.
	// This can be used for firing from a moving tank

	Tanket *tanket = context.getTargetContainer().getTanketById(weaponContext.getPlayerId());
	if (tanket && tanket->getAlive())
	{
		FixedVector newPosition = tanket->getLife().getTargetPosition();
		if (sightPos_.getValue())
		{
			tanket->getLife().getTankTurretPosition(newPosition);
		}

		aimedWeapon_.getValue()->fire(context, weaponContext, newPosition, velocity);
	}
}

