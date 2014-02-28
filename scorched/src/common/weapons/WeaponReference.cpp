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

#include <weapons/WeaponReference.h>
#include <weapons/AccessoryStore.h>
#include <common/DefinesString.h>

REGISTER_ACCESSORY_SOURCE(WeaponReference);

WeaponReference::WeaponReference() :
	Weapon("WeaponReference", "Use this to reference another accessory."
		"The reference accessory must be positioned above the accessory you are creating. "
		"If it is not correctly positioned above the accessory or the called accessory does not exist or is misspelled, you will receive an error stating it could not find the referenced accessory and the accessory failed."
		"This is different from WeaponInvokeWeapon as it: calculates the earnings and skill from the accessory fired rather than the reference accessory, when a player is killed it will display a kill by the fired accessory rather than the reference accessory"),
	weaponName_("Name of the weapon you want to fire at this point.  The referenced weapon must come before the current weapon")
{
	addChildXMLEntry("weapon", &weaponName_);
}

WeaponReference::~WeaponReference()
{
}

bool WeaponReference::readXML(XMLNode *node, void *xmlData)
{
	if (!Weapon::readXML(node, xmlData)) return false;

	// Find the accessory name
	AccessoryCreateContext *context = (AccessoryCreateContext *) xmlData;
	if (context)
	{
		std::map<std::string, XMLNode *> &nodes = context->getAccessoryStore().getParsingNodes();
		std::map<std::string, XMLNode *>::iterator finditor =
			nodes.find(weaponName_.getValue());
		if (finditor == nodes.end()) 
		{
			S3D::dialogMessage("WeaponReference", S3D::formatStringBuffer(
				"Failed to find weapon \"%s\"",
				weaponName_.getValue().c_str()));
			return false;	
		}
		XMLNode *weaponNode = (*finditor).second;
		weaponNode->resurrectRemovedChildren();

		// Action
		XMLNode *actionNode = 0;
		if (!weaponNode->getNamedChild("accessoryaction", actionNode)) return false;
		if (!refWeapon_.readXML(actionNode, xmlData))
		{
			S3D::dialogMessage("Accessory", S3D::formatStringBuffer(
				"Failed to find create weapon/wrong type \"%s\"",
				weaponName_.getValue().c_str()));
			return false;
		}
	}

	return true;
}

void WeaponReference::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	refWeapon_.getValue()->fire(context, weaponContext, position, velocity);
}

