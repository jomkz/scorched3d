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

#include <placement/PlacementObjectGroup.h>
#include <XML/XMLParser.h>

PlacementObjectGroupDefinition::PlacementObjectGroupDefinition() :
	XMLEntryContainer("PlacementObjectGroupDefinition",
		"A definition of a position where an object can be placed."
		"The position is defined as an offset from the incoming position."
		"The provides the ability to group a set of objects around the incoming position."),
	object(),
	offset("The offset from the incoming position.")
{
	addChildXMLEntry("object", &object);
	addChildXMLEntry("offset", &offset);
}

PlacementObjectGroupDefinition::~PlacementObjectGroupDefinition()
{
}

PlacementObjectGroupDefinitionList::PlacementObjectGroupDefinitionList() :
	XMLEntryList<PlacementObjectGroupDefinition>(
		"A list of object definitions that will be grouped around the incoming position", 1)
{
}

PlacementObjectGroupDefinitionList::~PlacementObjectGroupDefinitionList()
{
}

PlacementObjectGroupDefinition *PlacementObjectGroupDefinitionList::createXMLEntry(void *xmlData)
{
	return new PlacementObjectGroupDefinition();
}

PlacementObjectGroup::PlacementObjectGroup() :
	PlacementObject("PlacementObjectGroup", 
			"Groups a given set of objects around the incoming position."
			"Each object is placed at a given offset around the incoming position."),
	groups_()
{
	addChildXMLEntry("groupobject", &groups_);
}

PlacementObjectGroup::~PlacementObjectGroup()
{
}

void PlacementObjectGroup::createObject(ScorchedContext &context,
	RandomGenerator &generator,
	unsigned int &playerId,
	PlacementType::Position &position)
{
	std::list<PlacementObjectGroupDefinition *>::iterator itor = groups_.getChildren().begin(),
		end = groups_.getChildren().end();
	for (;itor!=end;++itor)
	{
		PlacementObjectGroupDefinition *groupObject = (*itor);

		PlacementType::Position newPosition = position;
		newPosition.position += groupObject->offset.getValue();
		groupObject->object.getValue()->createObject(context, 
			generator, 
			playerId, 
			newPosition);
	}
}
