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

#include <placement/PlacementObjectRandom.h>
#include <common/RandomGenerator.h>
#include <XML/XMLParser.h>

PlacementObjectRandomDefinition::PlacementObjectRandomDefinition() :
	XMLEntryContainer("PlacementObjectRandomDefinition",
		"A definition of a object placement and a weighting that weights the random chance it is chosen."
		"The higher the weighting the more chance an object has of being selected."
		"If all the objects have the same weight they all have the same chance at being chosen."),
	object(),
	weight("The chance/weighting that this object has."
		"The higher the weighting the more chance an object has of being selected", 0 ,1)
{
	addChildXMLEntry("object", &object);
	addChildXMLEntry("weight", &weight);
}

PlacementObjectRandomDefinition::~PlacementObjectRandomDefinition()
{
}

PlacementObjectRandomDefinitionList::PlacementObjectRandomDefinitionList() :
	XMLEntryList<PlacementObjectRandomDefinition>(
		"A list of random object placements, only one will be selected from this list (randomly) each time.", 1)
{
}

PlacementObjectRandomDefinitionList::~PlacementObjectRandomDefinitionList()
{
}

PlacementObjectRandomDefinition *PlacementObjectRandomDefinitionList::createXMLEntry(void *xmlData)
{
	return new PlacementObjectRandomDefinition();
}

PlacementObjectRandom::PlacementObjectRandom() : 
	PlacementObject("PlacementObjectRandom",
		"A set of placements.  Once placement will be randomly chosen for each item that is to be placed."
		"A weighting can be applied to increase or decrease the chance of being chosen."),
	totalWeight_(0)
{
	addChildXMLEntry("randomobject", &objects_);
}

PlacementObjectRandom::~PlacementObjectRandom()
{
}

bool PlacementObjectRandom::readXML(XMLNode *node, void *xmlData)
{
	if (!PlacementObject::readXML(node, xmlData)) return false;

	std::list<PlacementObjectRandomDefinition *>::iterator itor = objects_.getChildren().begin(),
		end = objects_.getChildren().end();
	for (;itor!=end;++itor)
	{
		totalWeight_ += (*itor)->weight.getValue();
	}

	return true;
}

void PlacementObjectRandom::createObject(ScorchedContext &context,
	RandomGenerator &generator,
	unsigned int &playerId,
	PlacementType::Position &position)
{
	fixed totalWeight = generator.getRandFixed("PlacementObjectRandom") * totalWeight_;
	fixed currentWeight = 0;

	std::list<PlacementObjectRandomDefinition *>::iterator itor = objects_.getChildren().begin(),
		end = objects_.getChildren().end();
	for (;itor!=end;++itor)
	{
		PlacementObjectRandomDefinition *object = (*itor);
		currentWeight += object->weight.getValue();

		if (currentWeight > totalWeight)
		{
			PlacementObject *entry = object->object.getValue();
			entry->createObject(context, generator, playerId, position);
			break;
		}
	}
}
