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

#include <weapons/WeaponTankVelocity.h>
#include <weapons/AccessoryStore.h>
#include <tanket/Tanket.h>
#include <tanket/TanketShotInfo.h>
#include <target/TargetContainer.h>
#include <common/OptionsScorched.h>
#include <engine/Simulator.h>

REGISTER_ACCESSORY_SOURCE(WeaponTankVelocity);

WeaponTankVelocity::WeaponTankVelocity() :
	Weapon("WeaponTankVelocity", "Sets the next action's velocity to the player's current shot velocitiy.")
{
	addChildXMLEntry("aimedweapon", &aimedWeapon_);
}

WeaponTankVelocity::~WeaponTankVelocity()
{

}

void WeaponTankVelocity::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	Tanket *tanket = context.getTargetContainer().getTanketById(weaponContext.getPlayerId());
	if (tanket && tanket->getAlive())
	{
		FixedVector newVelocity = weaponContext.getInternalContext().getVelocityVector();
		aimedWeapon_.getValue()->fire(context, weaponContext, position, newVelocity);
	}
}

