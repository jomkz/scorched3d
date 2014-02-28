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

#include <landscapedef/LandscapeEventAction.h>
#include <weapons/AccessoryStore.h>
#include <weapons/Weapon.h>
#include <engine/ObjectGroups.h>
#include <engine/ObjectGroupEntry.h>
#include <engine/Simulator.h>

LandscapeEventActionChoice::LandscapeEventActionChoice() :
	XMLEntryTypeChoice<LandscapeEventAction>("LandscapeEventActionChoice", 
		"A choice of actions to perform when this action fires")
{
}

LandscapeEventActionChoice::~LandscapeEventActionChoice()
{
}

LandscapeEventAction *LandscapeEventActionChoice::createXMLEntry(const std::string &type, void *xmlData)
{
	if (0 == strcmp(type.c_str(), "fireweapon")) return new LandscapeEventActionFireWeapon;
	if (0 == strcmp(type.c_str(), "fireweaponfromgroup")) return new LandscapeEventActionFireWeaponFromGroup;
	S3D::dialogMessage("LandscapeEventAction", S3D::formatStringBuffer("Unknown event action type %s", type));
	return 0;
}

void LandscapeEventActionChoice::getAllTypes(std::set<std::string> &allTypes)
{
	allTypes.insert("fireweapon");
	allTypes.insert("fireweaponfromgroup");
}

LandscapeEventAction::LandscapeEventAction(const char *name, const char *description) :
	XMLEntryContainer(name, description)
{
}

LandscapeEventAction::~LandscapeEventAction()
{
}

// LandscapeEventActionFireWeapon
LandscapeEventActionFireWeapon::LandscapeEventActionFireWeapon() :
	LandscapeEventAction("LandscapeEventActionFireWeapon", 
		"When the event fires, fires the given weapon."),
	weapon("The name of the weapon accessory to fire, accessory are defined in the accessories file.")
{
	addChildXMLEntry("weapon", &weapon);
}

LandscapeEventActionFireWeapon::~LandscapeEventActionFireWeapon()
{
}

void LandscapeEventActionFireWeapon::fireAction(ScorchedContext &context)
{
	Accessory *accessory = 
		context.getAccessoryStore().findByPrimaryAccessoryName(
			weapon.getValue().c_str());
	if (!accessory) S3D::dialogExit("LandscapeEventActionFireWeapon",
		S3D::formatStringBuffer("Failed to find weapon named \"%s\"", weapon.getValue().c_str()));
	if (accessory->getType() != AccessoryPart::AccessoryWeapon) 
		S3D::dialogExit("LandscapeEventActionFireWeapon",
			S3D::formatStringBuffer("Accessory named \"%s\" is not a weapon", weapon.getValue().c_str()));
	Weapon *weapon = (Weapon *) accessory->getAction();

	WeaponFireContext weaponContext(0, 0, 0, FixedVector(), false, false);
	FixedVector pos, vel;
	weapon->fire(context, weaponContext, pos, vel);
}

// LandscapeEventActionFireWeaponFromGroup
LandscapeEventActionFireWeaponFromGroup::LandscapeEventActionFireWeaponFromGroup() :
	LandscapeEventAction("LandscapeEventActionFireWeaponFromGroup", 
		"When the event fires, fires the given weapon from an object in the specified group."),
	weapon("The name of the weapon accessory to fire, accessory are defined in the accessories file."),
	groupname("The name of the group to randomly choose an object from.")
{
	addChildXMLEntry("weapon", &weapon, "groupname", &groupname);
}

LandscapeEventActionFireWeaponFromGroup::~LandscapeEventActionFireWeaponFromGroup()
{
}

void LandscapeEventActionFireWeaponFromGroup::fireAction(ScorchedContext &context)
{
	Accessory *accessory = 
		context.getAccessoryStore().findByPrimaryAccessoryName(
			weapon.getValue().c_str());
	if (!accessory) S3D::dialogExit("LandscapeEventActionFireWeaponFromGroup",
		S3D::formatStringBuffer("Failed to find weapon named \"%s\"", weapon.getValue().c_str()));
	if (accessory->getType() != AccessoryPart::AccessoryWeapon) 
		S3D::dialogExit("LandscapeEventActionFireWeaponFromGroup",
			S3D::formatStringBuffer("Accessory named \"%s\" is not a weapon", weapon.getValue().c_str()));
	Weapon *weapon = (Weapon *) accessory->getAction();

	// Find the group to select the objects in
	ObjectGroup *objectGroup = context.getObjectGroups().getGroup(groupname.getValue().c_str());
	if (!objectGroup) return;

	// Select the object
	int objectCount = objectGroup->getObjectCount();
	if (objectCount == 0) return;
	unsigned int object = context.getSimulator().getRandomGenerator().getRandUInt("LandscapeEvents") % objectCount;
	ObjectGroupEntry *entry = objectGroup->getObjectByPos(object);

	FixedVector newPosition = entry->getPosition();
	FixedVector newVelocity = entry->getVelocity();

	WeaponFireContext weaponContext(entry->getPlayerId(), 0, 0, newVelocity, false, false);
	weapon->fire(context, weaponContext, newPosition, newVelocity);
}
