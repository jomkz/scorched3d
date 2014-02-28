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

#include <weapons/WeaponGroupSelect.h>
#include <weapons/AccessoryStore.h>
#include <engine/Simulator.h>
#include <engine/ObjectGroups.h>
#include <engine/ObjectGroup.h>
#include <engine/ObjectGroupEntry.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeTex.h>
#include <target/Target.h>
#include <target/TargetLife.h>

REGISTER_ACCESSORY_SOURCE(WeaponGroupSelect);

WeaponGroupSelect::WeaponGroupSelect() :
	Weapon("WeaponGroupSelect", "Instructs weapon to shift it's origin to an object with a pre-specified <groupname>." 
		"Groups objects may include boids, ships, trees, or weapon-created objects (eg. projectiles and rollers. "),
	groupNames_(),
	nextAction_()
{
	addChildXMLEntry("groupname", &groupNames_);
	addChildXMLEntry("nextaction", &nextAction_);
}

WeaponGroupSelect::~WeaponGroupSelect()
{
}

void WeaponGroupSelect::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	// Find the group to select the objects in
	std::vector<ObjectGroupEntry *> positions;
	std::list<XMLEntryString *>::iterator itor = groupNames_.getChildren().begin(),
		end = groupNames_.getChildren().end();
	for (;itor!=end;++itor)
	{
		const char *groupName = (*itor)->getValue().c_str();
		ObjectGroup *objectGroup = weaponContext.getInternalContext().getLocalGroups().getGroup(groupName);
		if (!objectGroup) objectGroup = context.getObjectGroups().getGroup(groupName);
		if (objectGroup) 
		{
			ObjectGroup::ObjectGroupEntryHolderIterator iterator(objectGroup);
			ObjectGroupEntry *entry;
			while (entry = iterator.getNext())
			{
				positions.push_back(entry);
			}			
		}
	}

	if (positions.empty()) return;
	unsigned int object = context.getSimulator().getRandomGenerator().getRandUInt("WeaponGroupSelect") % positions.size();
	ObjectGroupEntry *entry = positions[object];

	FixedVector newPosition = entry->getPosition();
	FixedVector newVelocity = entry->getVelocity();
	nextAction_.getValue()->fire(context, weaponContext, newPosition, newVelocity);
}
