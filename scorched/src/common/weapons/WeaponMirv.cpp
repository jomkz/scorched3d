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

#include <weapons/WeaponMirv.h>
#include <weapons/AccessoryStore.h>
#include <engine/Simulator.h>
#include <common/Defines.h>

REGISTER_ACCESSORY_SOURCE(WeaponMirv);

WeaponMirv::WeaponMirv() :
	Weapon("WeaponMirv", "Splits up into a number of weapons fanned out from the current position in the direction it is aimed. "
		"MIRVs can be configured to spread out vertically, horizontally, or both. "),
	noWarheads_("Number of instances of the next primitive to create"), 
	hspreadDist_("WeaponMirv::hspreadDist", "Amount the mirv will fan out horizontally"
           "unlike vspread, projectiles fan out randomly within this max value"), 
	vspreadDist_("WeaponMirv::vspreadDist", "Amount the mirv will fan out vertically")
{
	addChildXMLEntry("hspreaddist", &hspreadDist_);
	addChildXMLEntry("vspreaddist", &vspreadDist_);
	addChildXMLEntry("numberwarheads", &noWarheads_);
	addChildXMLEntry("aimedweapon", &aimedWeapon_);
}

WeaponMirv::~WeaponMirv()
{
}

void WeaponMirv::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	// Add a shot that will fall where the original was aimed
	aimedWeapon_.getValue()->fire(context, weaponContext, position, velocity);

	RandomGenerator &random = context.getSimulator().getRandomGenerator();

	// Add all of the sub warheads that have a random spread
	fixed hspreadDist;
	for (int i=0; i<noWarheads_.getValue() - 1; i++)
	{
		FixedVector newDiff = velocity;
		newDiff[2] = 0;
		// Ensure the same value is used in all parts of the calc
		hspreadDist = hspreadDist_.getValue(context);
		if (hspreadDist != 0)
		{
			FixedVector diff = newDiff;
			diff[2] -= 1;
			FixedVector perp = newDiff * diff;

			newDiff += (perp * ((random.getRandFixed("WeaponMirv") * hspreadDist) - (hspreadDist * fixed(true, 5000))));
		}
		newDiff[2] += (fixed(i - (noWarheads_.getValue() / 2)) / 
			fixed(noWarheads_.getValue() / 2)) * vspreadDist_.getValue(context);

		aimedWeapon_.getValue()->fire(context, weaponContext, position, newDiff);
	}
}

