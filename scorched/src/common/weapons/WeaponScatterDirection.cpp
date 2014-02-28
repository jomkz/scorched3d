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

#include <weapons/WeaponScatterDirection.h>
#include <weapons/AccessoryStore.h>
#include <engine/Simulator.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeTex.h>
#include <common/Defines.h>
#include <math.h>

REGISTER_ACCESSORY_SOURCE(WeaponScatterDirection);

WeaponScatterDirection::WeaponScatterDirection() :
	Weapon("WeaponScatterDirection", 
		"Changes the direction to a specific direction with a random offset"),
	direction_("X, Y and Z direction to aim the weapon in",
	directionOffset_("maximum X, Y, and Z amounts to offset the direction by. "
           "For example, 10,10,9 will offset the direction with a random factor of +/- 10 degrees in each the x and y directions"),
{
	addChildXMLEntry("direction", &direction_);
	addChildXMLEntry("directionoffset", &directionOffset_);
	addChildXMLEntry("aimedweapon", &aimedWeapon_);
}

WeaponScatterDirection::~WeaponScatterDirection()
{

}

void WeaponScatterDirection::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &v)
{
	RandomGenerator &random = context.getSimulator().getRandomGenerator();
	FixedVector vel;
	vel[0] += direction_[0] - directionOffset_[0] + 
		directionOffset_[0] * 2 * random.getRandFixed("WeaponScatterDirection");
	vel[1] += direction_[1] - directionOffset_[1] + 
		directionOffset_[1] * 2 * random.getRandFixed("WeaponScatterDirection");
	vel[2] += direction_[2] - directionOffset_[2] + 
		directionOffset_[2] * 2 * random.getRandFixed("WeaponScatterDirection");

	aimedWeapon_->fire(context, weaponContext, position, vel);

}
