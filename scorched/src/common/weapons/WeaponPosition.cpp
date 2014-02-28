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

#include <weapons/WeaponPosition.h>
#include <weapons/AccessoryStore.h>
#include <engine/Simulator.h>
#include <landscapemap/LandscapeMaps.h>

REGISTER_ACCESSORY_SOURCE(WeaponPosition);

WeaponPosition::WeaponPosition() :
	Weapon("WeaponPosition", "Sets the position of an accessory explicitly on (or over or under) the landscape. "),
	position_("The coordinates of the position desired for the next accessory"),
	positionOffset_("A random offset value. The position will be offset by up to +/- each of these coordinates", 0, FixedVector::getNullVector()),
	onGround_("When set to true the height will be forced to ground level, ignoring any 'c' value in position.  The offset value can still be used (carefully!)", 0, false)
{
	addChildXMLEntry("aimedweapon", &aimedWeapon_);
	addChildXMLEntry("position", &position_);
	addChildXMLEntry("positionoffset", &positionOffset_);
	addChildXMLEntry("onground", &onGround_);
}

WeaponPosition::~WeaponPosition()
{
}

void WeaponPosition::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	// Position the fired weapon at the desired position, 
	// +/- optional random offset
	RandomGenerator &random = context.getSimulator().getRandomGenerator();
	FixedVector newPosition;
	newPosition[0] = position_.getValue()[0] - positionOffset_.getValue()[0] +
		positionOffset_.getValue()[0] * 2 * random.getRandFixed("WeaponPositionX");
	newPosition[1] = position_.getValue()[1] - positionOffset_.getValue()[1] +
		positionOffset_.getValue()[1] * 2 * random.getRandFixed("WeaponPositionY");
	newPosition[2] = position_.getValue()[2] - positionOffset_.getValue()[2] +		
		positionOffset_.getValue()[2] * 2 * random.getRandFixed("WeaponPositionZ");

	if(onGround_.getValue())
	{
		fixed minHeight = context.getLandscapeMaps().getGroundMaps().getInterpHeight(
		                newPosition[0], newPosition[1]);
		newPosition[2] = minHeight;
	}
	
	aimedWeapon_.getValue()->fire(context, weaponContext, newPosition, velocity);
}

