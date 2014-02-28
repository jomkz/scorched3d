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

#include <weapons/WeaponScatterPosition.h>
#include <weapons/AccessoryStore.h>
#include <engine/Simulator.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeTex.h>
#include <common/Defines.h>
#include <math.h>

REGISTER_ACCESSORY_SOURCE(WeaponScatterPosition);

WeaponScatterPosition::WeaponScatterPosition() :
	Weapon("WeaponScatterPosition", 
		"Randomly offsets the current position by a percentage of the landscape size."),
	scatterpercentage_("WeaponScatterPosition::scatterpercentage", "Percentage of landscape size to randomize the weapon's position by."),
	landonly_("Weapon cannot occur over water"),
	landheight_("Weapon is snapped to landscape height")
{
	addChildXMLEntry("aimedweapon", &aimedWeapon_);
	addChildXMLEntry("landonly", &landonly_);
	addChildXMLEntry("landheight", &landheight_);
	addChildXMLEntry("scatterpercentage", &scatterpercentage_);
}

WeaponScatterPosition::~WeaponScatterPosition()
{
}

void WeaponScatterPosition::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &p, FixedVector &velocity)
{
	// Mininum height, if we are grounding
	LandscapeTex &tex = *context.getLandscapeMaps().getDescriptions().getTex();
	fixed allowedHeight = S3D_MAX(fixed(0), tex.waterHeight.getValue());

	fixed scatterpercentage = scatterpercentage_.getValue(context);

	fixed arenaWidth = fixed(context.getLandscapeMaps().getGroundMaps().getArenaWidth());
	fixed arenaHeight = fixed(context.getLandscapeMaps().getGroundMaps().getArenaHeight());
	fixed scatterWidth = arenaWidth * scatterpercentage / 100;
	fixed scatterHeight = arenaHeight * scatterpercentage / 100;

	FixedVector pos;
	RandomGenerator &random = context.getSimulator().getRandomGenerator();
	bool ok = false;
	while (!ok)
	{
		ok = true;

		pos[0] = p[0] + (random.getRandFixed("WeaponScatterPosition") * scatterWidth) - (scatterWidth / 2);
		pos[1] = p[1] + (random.getRandFixed("WeaponScatterPosition") * scatterHeight) - (scatterHeight / 2);
		pos[2] = p[2];
		if (landheight_.getValue())
		{
			pos[2] = context.getLandscapeMaps().getGroundMaps().getInterpHeight(
				pos[0], pos[1]);
		}

		if (landonly_.getValue())
		{
			if (pos[2] < allowedHeight)
			{
				ok = false;
				allowedHeight -= fixed(true, 100);
			}
		}
	}

	aimedWeapon_.getValue()->fire(context, weaponContext, pos, velocity);
}

