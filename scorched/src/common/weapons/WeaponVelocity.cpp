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

#include <weapons/WeaponVelocity.h>
#include <weapons/AccessoryStore.h>

REGISTER_ACCESSORY_SOURCE(WeaponVelocity);

WeaponVelocity::WeaponVelocity() :
	Weapon("WeaponVelocity", 
		"Changes the velocity of the current weapon either relative to its current velocity or to a pre-defined velocity."),
	velocityChange_("WeaponVelocity::velocityChange", 
			"Amount to change the velocity."
           "If abs is false: 0.5 = 50%, 1.5 = 150%, etc."
		   "If abs is true: 0.5 = 500 power, 1.0 = 1000 power, etc."), 
	abs_("Whether or not the velocity change is absolute", 0, false)
{
	addChildXMLEntry("aimedweapon", &aimedWeapon_);
	addChildXMLEntry("velocitychange", &velocityChange_);
	addChildXMLEntry("abs", &abs_);
}

WeaponVelocity::~WeaponVelocity()
{
}

void WeaponVelocity::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	// Add a shot that will fall where the original was aimed
	// but with altered velocity
	FixedVector newVelocity;
	if (abs_.getValue())
	{
		newVelocity = velocity.Normalize() * 50 * velocityChange_.getValue(context); 
		aimedWeapon_.getValue()->fire(context, weaponContext, position, newVelocity);
	}
	else
	{
		newVelocity = velocity * velocityChange_.getValue(context);
		aimedWeapon_.getValue()->fire(context, weaponContext, position, newVelocity);
	}
}

