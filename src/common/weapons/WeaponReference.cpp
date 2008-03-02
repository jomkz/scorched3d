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

#include <weapons/WeaponReference.h>
#include <weapons/AccessoryStore.h>
#include <common/DefinesString.h>

REGISTER_ACCESSORY_SOURCE(WeaponReference);

WeaponReference::WeaponReference()
{

}

WeaponReference::~WeaponReference()
{

}

bool WeaponReference::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(context, accessoryNode)) return false;

	// Get the next weapon
	std::string subNode;
	if (!accessoryNode->getNamedChild("weapon", subNode)) return false;

	// Find the accessory name
	std::map<std::string, XMLNode *> &nodes = context.getAccessoryStore()->getParsingNodes();
	std::map<std::string, XMLNode *>::iterator finditor =
		nodes.find(subNode.c_str());
	if (finditor == nodes.end()) 
	{
		S3D::dialogMessage("WeaponReference", S3D::formatStringBuffer(
			"Failed to find weapon \"%s\"",
			subNode.c_str()));
		return false;	
	}
	XMLNode *weaponNode = (*finditor).second;
	weaponNode->resurrectRemovedChildren();

	// Action
	XMLNode *actionNode = 0;
	if (!weaponNode->getNamedChild("accessoryaction", actionNode)) return false;
	
	// Create the new weapon
	AccessoryPart *accessory = context.getAccessoryStore()->
		createAccessoryPart(context, parent_, actionNode);
	if (!accessory || accessory->getType() != AccessoryPart::AccessoryWeapon)
	{
		S3D::dialogMessage("Accessory", S3D::formatStringBuffer(
			"Failed to find create weapon/wrong type \"%s\"",
			subNode.c_str()));
		return false;
	}
	refWeapon_ = (Weapon*) accessory;

	return true;
}

void WeaponReference::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	refWeapon_->fireWeapon(context, weaponContext, position, velocity);
}

