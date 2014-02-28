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

#include <landscapedef/LandscapeDescriptionsBase.h>
#include <common/Defines.h>

LandscapeDescriptionsDefnEntryList::LandscapeDescriptionsDefnEntryList() :
	XMLEntryList<XMLEntryString>(
		"The list of physical property files of a landscape, one of which will be chosen at random for the terrain", 1)
{
}

LandscapeDescriptionsDefnEntryList::~LandscapeDescriptionsDefnEntryList()
{
}

XMLEntryString *LandscapeDescriptionsDefnEntryList::createXMLEntry(void *xmlData)
{
	return new XMLEntryString(
		"The name of a landscape definition file that contains the definition of the purely physical aspects of the terrain");
}

LandscapeDescriptionsTexEntryList::LandscapeDescriptionsTexEntryList() :
	XMLEntryList<XMLEntryString>(
		"The list of visual property files of a landscape, one of which will be chosen at random for the terrain", 1)
{
}

LandscapeDescriptionsTexEntryList::~LandscapeDescriptionsTexEntryList()
{
}

XMLEntryString *LandscapeDescriptionsTexEntryList::createXMLEntry(void *xmlData)
{
	return new XMLEntryString(
		"The name of a landscape texture file that contains the purely visual aspects of the terrain");
}

LandscapeDescriptionsEntry::LandscapeDescriptionsEntry() :
	XMLEntryContainer("LandscapeDescription", 
		"The description of a landscape, or terrain that makes up the Scorched3D playing field/arena.\n"
		"If more than one physical and/or visual file is provided one of each will be chosen at random"),
	name("The name of this landscape"),
	description("The description of this landscape"),
	picture("The icon to be displayed for this landscape"),
	weight("The change that this landscape will be chosen, when random landscapes are enabled")
{
	addChildXMLEntry("name", &name);
	addChildXMLEntry("description", &description);
	addChildXMLEntry("picture", &picture);
	addChildXMLEntry("weight", &weight);
	addChildXMLEntry("definition", &defns);
	addChildXMLEntry("texture", &texs);
}

LandscapeDescriptionsEntry::~LandscapeDescriptionsEntry()
{
}

LandscapeDescriptionsEntryList::LandscapeDescriptionsEntryList() :
	XMLEntryList<LandscapeDescriptionsEntry>("The list of landscape descriptions that can be chosen to make a terrain", 1)
{
}

LandscapeDescriptionsEntryList::~LandscapeDescriptionsEntryList()
{
}

LandscapeDescriptionsEntry *LandscapeDescriptionsEntryList::createXMLEntry(void *xmlData)
{
	return new LandscapeDescriptionsEntry();
}

LandscapeDescriptionsBase::LandscapeDescriptionsBase() :
	XMLEntryRoot<XMLEntryContainer>(S3D::eModLocation, "data/landscapes/landscapes.xml",
		"landscapes", "LandscapeDescriptions", 
		"The set of landscape descriptions that can be used to form the playing arena")
{
	addChildXMLEntry("landscape", &entries_);
}

LandscapeDescriptionsBase::~LandscapeDescriptionsBase()
{
}

void LandscapeDescriptionsBase::clearLandscapeDescriptions()
{
	entries_.clear();
}

bool LandscapeDescriptionsBase::readLandscapeDescriptions()
{
	entries_.clear();
	return loadFile(true, 0);
}

bool LandscapeDescriptionsBase::landscapeEnabled(OptionsGame &context, 
											const char *name)
{
	std::string landscapes = context.getLandscapes();
	if (landscapes.empty()) return true; // Default un-initialized state

	char *token = strtok((char *) landscapes.c_str(), ":");
	while(token != 0)
	{
		if (0 == strcmp(token, name)) return true;
		token = strtok(0, ":");
	}
	return false;
}

LandscapeDescriptionsEntry *LandscapeDescriptionsBase::getLandscapeByName(
	const char *name)
{
	std::list<LandscapeDescriptionsEntry *>::iterator itor;
	for (itor = entries_.getChildren().begin();
		itor != entries_.getChildren().end();
		++itor)
	{
		LandscapeDescriptionsEntry *result = *itor;
		if (0 == strcmp(name, result->name.getValue().c_str()))
		{
			return result;
		}
	}
	return 0;
}
