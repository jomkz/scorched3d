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

#include <weapons/WeaponTranslate.h>
#include <weapons/AccessoryStore.h>
#include <engine/ActionController.h>

REGISTER_ACCESSORY_SOURCE(WeaponTranslate);

WeaponTranslate::WeaponTranslate() :
	Weapon("WeaponTranslate", 
		"Moves the projectile or other weapon to a further point along the current trajectory by a specified distance."),
	translateDist_("WeaponTranslate::translateDist", "distance to move along the axis before performing next action", 0, "0")
{
	addChildXMLEntry("translatedist", &translateDist_);
	addChildXMLEntry("nextaction", &nextAction_);
}

WeaponTranslate::~WeaponTranslate()
{
}

void WeaponTranslate::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	FixedVector newVelocity = velocity.Normalize() * translateDist_.getValue(context);
	FixedVector newPosition = position + newVelocity;
	
	nextAction_.getValue()->fire(context, weaponContext, newPosition, velocity);
}

