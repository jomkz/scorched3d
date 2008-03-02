////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
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
//    You should have received a copy of the GNU General Public License
//    along with Scorched3D; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

#include <weapons/WeaponTeleport.h>
#include <engine/ActionController.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeTex.h>
#include <actions/Teleport.h>
#include <common/Defines.h>

REGISTER_ACCESSORY_SOURCE(WeaponTeleport);

WeaponTeleport::WeaponTeleport() : groundOnly_(true)
{

}

WeaponTeleport::~WeaponTeleport()
{

}

bool WeaponTeleport::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(context, accessoryNode)) return false;
	if (!accessoryNode->getNamedChild("delay", delay_)) return false;
	if (!accessoryNode->getNamedChild("sound", sound_)) return false;
	accessoryNode->getNamedChild("groundonly", groundOnly_, false);
	
	if (!S3D::checkDataFile(getSound())) return false;

	return true;
}

fixed WeaponTeleport::getDelay(ScorchedContext &context)
{
	return delay_.getValue(context);
}

void WeaponTeleport::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	// Mininum height
	fixed allowedHeight = 0;
	LandscapeTex &tex = *context.landscapeMaps->getDefinitions().getTex();
		if (tex.border->getType() == LandscapeTexType::eWater)
	{
		LandscapeTexBorderWater *water = 
			(LandscapeTexBorderWater *) tex.border;

		allowedHeight = water->height;
	}

	int mapWidth = context.landscapeMaps->getGroundMaps().getMapWidth();
	int mapHeight = context.landscapeMaps->getGroundMaps().getMapHeight();

	if (position[0] > 5 && 
		position[1] > 5 &&
		position[0] < fixed(mapWidth - 5) &&
		position[1] < fixed(mapHeight - 5))
	{
		fixed landscapeHeight = context.landscapeMaps->getGroundMaps().getInterpHeight(
			position[0], position[1]);
		if (landscapeHeight >= allowedHeight - 1)
		{
			context.actionController->addAction(
				new Teleport(position, weaponContext, this));
		}
	}
}
