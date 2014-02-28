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

#include <placement/PlacementObjectHeight.h>
#include <common/RandomGenerator.h>
#include <XML/XMLParser.h>

PlacementObjectHeightDefinition::PlacementObjectHeightDefinition() :
	XMLEntryContainer("PlacementObjectHeightDefinition",
		"A definition of a height band where objects can be placed."
		"Incoming object positions are checked against the height band to see if they are valid."
		"Valid heights (heights that fall within the band) are accepted and the object is placed/created, invalid heights are discarded."),
	object(),
	minheight("The lower/minimum height of the height band"),
	maxheight("The upper/maximum height of the height band")
{
	addChildXMLEntry("object", &object);
	addChildXMLEntry("minheight", &minheight);
	addChildXMLEntry("maxheight", &maxheight);
}

PlacementObjectHeightDefinition::~PlacementObjectHeightDefinition()
{
}

PlacementObjectHeightDefinitionList::PlacementObjectHeightDefinitionList() :
	XMLEntryList<PlacementObjectHeightDefinition>("A list of height band definitions", 1)
{
}

PlacementObjectHeightDefinitionList::~PlacementObjectHeightDefinitionList()
{
}

PlacementObjectHeightDefinition *PlacementObjectHeightDefinitionList::createXMLEntry(void *xmlData)
{
	return new PlacementObjectHeightDefinition();
}

PlacementObjectHeight::PlacementObjectHeight() :
	PlacementObject("PlacementObjectHeight", 
			"Places objects within height bands on the landscape, the height band is given between two given values."
			"If the landscape height at the input point falls outwith the height band, no object is placed."
			"A small random value can be added to the height to make the bands non-uniform."),
	objects_()
{
	addChildXMLEntry("heightobject", &objects_);
}

PlacementObjectHeight::~PlacementObjectHeight()
{
}

void PlacementObjectHeight::createObject(ScorchedContext &context,
	RandomGenerator &generator,
	unsigned int &playerId,
	PlacementType::Position &position)
{
	fixed offset = generator.getRandFixed("PlacementObjectHeight") * 10 - 5;
	fixed height = position.position[2] + offset;

	std::list<PlacementObjectHeightDefinition *>::iterator itor = objects_.getChildren().begin(),
		end = objects_.getChildren().end();
	for (;itor!=end;++itor)
	{
		PlacementObjectHeightDefinition &object = *(*itor);

		if (height >= object.minheight.getValue() &&
			height <= object.maxheight.getValue())
		{
			PlacementObject *entry = object.object.getValue();
			entry->createObject(context, generator, playerId, position);
			break;
		}
	}
}
