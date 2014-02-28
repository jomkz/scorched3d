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

#include <engine/ObjectGroupEntryDefinition.h>
#include <engine/ScorchedContext.h>
#include <engine/ObjectGroups.h>

ObjectGroupEntryDefinition::ObjectGroupEntryDefinition() :
	XMLEntryList<XMLEntryString>(
		"Defines a list of object group names that this object will be added to, the groups will be created if they don't already exist.", 0)
{
}

ObjectGroupEntryDefinition::~ObjectGroupEntryDefinition()
{
}

XMLEntryString *ObjectGroupEntryDefinition::createXMLEntry(void *xmlData)
{
	return new XMLEntryString("The name of an object group, the group will be created if it doesn't already exist");
}

void ObjectGroupEntryDefinition::addToGroups(
	ObjectGroups &objectGroups,
	ObjectGroupEntry *objectGroupEntry)
{
	std::list<XMLEntryString *>::iterator itor = getChildren().begin(),
		end = getChildren().end();
	for (;itor!=end;++itor)
	{
		std::string groupname = (*itor)->getValue();
		addToGroup(groupname.c_str(), objectGroups, objectGroupEntry);
	}
}

void ObjectGroupEntryDefinition::addToGroup(
	const char *groupName,
	ObjectGroups &objectGroups,
	ObjectGroupEntry *objectGroupEntry)
{
	ObjectGroup *group = objectGroups.getGroup(groupName, true);
	if (group)
	{
		group->addObject(objectGroupEntry);
	}
}

ObjectGroupReferenceDefinition::ObjectGroupReferenceDefinition() :
	XMLEntryList<XMLEntryString>(
		"Defines a list of object group names that objects can be retrieved from", 0)
{
}

ObjectGroupReferenceDefinition::~ObjectGroupReferenceDefinition()
{
}

XMLEntryString *ObjectGroupReferenceDefinition::createXMLEntry(void *xmlData)
{
	return new XMLEntryString("The name of an object group, the group will be used to retrieve objects from");
}
