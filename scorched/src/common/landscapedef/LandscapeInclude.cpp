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

#include <landscapedef/LandscapeInclude.h>
#include <landscapedef/LandscapeDefinitions.h>

LansdscapeIncludeList::LansdscapeIncludeList() :
	XMLEntryList<XMLEntryString>("A set of files to include into this file")
{
}

LansdscapeIncludeList::~LansdscapeIncludeList()
{
}

XMLEntryString *LansdscapeIncludeList::createXMLEntry(void *xmlData)
{
	return new XMLEntryString("The name of file to include into this file");
}

LandscapeInclude::LandscapeInclude(LandscapeDefinitions *definitions, const char *name, const char *description) :
	XMLEntryContainer(name, description),
	definitions_(definitions)
{
	addChildXMLEntry("event", &events);
	addChildXMLEntry("movement", &movements);
	addChildXMLEntry("sound", &sounds);
	addChildXMLEntry("music", &musics);
	addChildXMLEntry("placement", &placements);
	addChildXMLEntry("options", &options);
}

LandscapeInclude::~LandscapeInclude()
{
}

bool LandscapeInclude::readXML(XMLNode *parentNode, void *xmlData)
{
	if (!XMLEntryContainer::readXML(parentNode, xmlData)) return false;

	std::list<XMLEntryString *>::iterator itor = includeList.getChildren().begin(),
		end = includeList.getChildren().end();
	for (;itor!=end;++itor)
	{
		std::string fileName = (*itor)->getValue();
		LandscapeInclude *landscapeInclude = 
			definitions_->getInclude(fileName.c_str(), true);
		if (!landscapeInclude) return false;
		includes.push_back(landscapeInclude);
	}

	return true;
}

LandscapeIncludeFile::LandscapeIncludeFile(LandscapeDefinitions *definitions) :
	LandscapeInclude(definitions, "LandscapeIncludeFile", 
		"A landscape/scene definition fragment, this fragment can contain many different types of Scorched3D artifact")
{

}

LandscapeIncludeFile::~LandscapeIncludeFile()
{
}
