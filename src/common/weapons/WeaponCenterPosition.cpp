////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
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

#include <weapons/WeaponCenterPosition.h>
#include <weapons/AccessoryStore.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeTex.h>
#include <engine/ActionController.h>

REGISTER_ACCESSORY_SOURCE(WeaponCenterPosition);

WeaponCenterPosition::WeaponCenterPosition() :
	nextAction_(0)
{

}

WeaponCenterPosition::~WeaponCenterPosition()
{

}

bool WeaponCenterPosition::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(context, accessoryNode)) return false;

	if (!accessoryNode->getNamedChild("height", height_)) return false;

	XMLNode *subNode = 0;
	if (!accessoryNode->getNamedChild("nextaction", subNode)) return false;
	
	// Check next weapon is correct type
	AccessoryPart *accessory = context.getAccessoryStore()->
		createAccessoryPart(context, parent_, subNode);
	if (!accessory || accessory->getType() != AccessoryPart::AccessoryWeapon)
	{
		return subNode->returnError("Failed to find sub weapon, not a weapon");
	}
	nextAction_ = (Weapon*) accessory;

	return true;
}

void WeaponCenterPosition::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	int arenaX = context.landscapeMaps->getGroundMaps().getArenaX();
	int arenaY = context.landscapeMaps->getGroundMaps().getArenaY();
	int arenaWidth = context.landscapeMaps->getGroundMaps().getArenaWidth();
	int arenaHeight = context.landscapeMaps->getGroundMaps().getArenaHeight();

	FixedVector newPositon = position;
	newPositon[0] = (arenaWidth / 2) + arenaX;
	newPositon[1] = (arenaHeight / 2) + arenaY;
	newPositon[2] = height_.getValue(context);
	
	nextAction_->fireWeapon(context, weaponContext, newPositon, velocity);

}
