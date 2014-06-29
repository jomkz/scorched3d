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

#include <weapons/WeaponTeleport.h>
#include <engine/ActionController.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeTex.h>
#include <actions/Teleport.h>
#include <common/Defines.h>

REGISTER_ACCESSORY_SOURCE(WeaponTeleport);

WeaponTeleport::WeaponTeleport() : 
	Weapon("WeaponTeleport",
		"Teleports the user to the weapons current position."),
	groundOnly_("Teleports to landscape height", 0, true), 
	delay_("WeaponTeleport::delay", "Number of seconds to delay before teleporting")
{
	addChildXMLEntry("groundonly", &groundOnly_);
	addChildXMLEntry("delay", &delay_);
}

WeaponTeleport::~WeaponTeleport()
{

}

fixed WeaponTeleport::getDelay(ScorchedContext &context)
{
	return delay_.getValue(context);
}

void WeaponTeleport::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	// Mininum height
	LandscapeTex &tex = *context.getLandscapeMaps().getDescriptions().getTex();
	fixed allowedHeight = S3D_MAX(fixed(0), tex.water.waterHeight.getValue());

	int arenaX = context.getLandscapeMaps().getGroundMaps().getArenaX();
	int arenaY = context.getLandscapeMaps().getGroundMaps().getArenaY();
	int arenaWidth = context.getLandscapeMaps().getGroundMaps().getArenaWidth();
	int arenaHeight = context.getLandscapeMaps().getGroundMaps().getArenaHeight();

	if (position[0] > fixed(arenaX + 5) && 
		position[1] > fixed(arenaY + 5) &&
		position[0] < fixed(arenaX + arenaWidth - 5) &&
		position[1] < fixed(arenaY + arenaHeight - 5))
	{
		fixed landscapeHeight = context.getLandscapeMaps().getGroundMaps().getInterpHeight(
			position[0], position[1]);
		if (landscapeHeight >= allowedHeight - 1)
		{
			context.getActionController().addAction(
				new Teleport(position, weaponContext, this));
		}
	}
}
