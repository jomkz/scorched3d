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

#include <weapons/WeaponLeapFrog.h>
#include <weapons/AccessoryStore.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapemap/HeightMap.h>

REGISTER_ACCESSORY_SOURCE(WeaponLeapFrog);

WeaponLeapFrog::WeaponLeapFrog():  
	Weapon("WeaponLeapFrog", "Causes the next primitive to be shot at the same angle of inclination that it came in at and with a given change in velocity. "
		"So if it was coming down at an angle of 60 degrees, it will be shot at 60 degrees toward the direction it was traveling before it hit."),
	bounce_("WeaponLeapFrog::bounce", "amount to change the velocity by (newvelocity = velocity * bounce)", 0, "0.6")
{
	addChildXMLEntry("bounce", &bounce_);
	addChildXMLEntry("collisionaction", &collisionAction_);
}

WeaponLeapFrog::~WeaponLeapFrog()
{
}

void WeaponLeapFrog::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	FixedVector newVelocity = velocity * bounce_.getValue(context);
	if (newVelocity[2] < 0) newVelocity[2] *= -1;

	FixedVector newPosition = position;
	fixed minHeight = context.getLandscapeMaps().getGroundMaps().getInterpHeight(
		position[0], position[1]);

	if (position[2] < minHeight + fixed(true, 7000))
	{
		// Make sure position is not fired underground
		if (minHeight - position[2] > fixed(true, 6500)) // Give room for shields as well
		{
		}
		else
		{
			newPosition[2] = minHeight + fixed(true, 7000);
		}
	}

	collisionAction_.getValue()->fire(context, weaponContext, newPosition, newVelocity);
}
