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

#include <landscapedef/LandscapeOptions.h>
#include <math.h>

LandscapeOptions::LandscapeOptions()
{
}

LandscapeOptions::~LandscapeOptions()
{
}

bool LandscapeOptions::readXML(XMLNode *parentNode)
{
	XMLNode *node = 0;
	if (!parentNode->getNamedChild(xmlEntryName_, node)) return false;

	std::list<XMLNode *>::iterator itor = node->getChildren().begin(),
		end = node->getChildren().end();
	for (;itor!=end;++end)
	{
		changedOptionNames_.push_back((*itor)->getName());
	}

	return OptionsGame::readXML(parentNode);
}

LandscapeOptionsList::LandscapeOptionsList() :
	 XMLEntryList<LandscapeOptions>("options", 
		 "Defines the game options for this level, these options override any set globaly as the server settings.")
{
}

LandscapeOptionsList::~LandscapeOptionsList()
{
}

LandscapeOptions *LandscapeOptionsList::createXMLEntry()
{
	return new LandscapeOptions();
}
