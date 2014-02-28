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

#include <engine/ModInfo.h>
#include <common/Defines.h>

ModInfoGame::ModInfoGame() :
	XMLEntryContainer("ModInfoGame", "A game type that can be played for this mod"),
	icon_("The icon for this game type"),
	description_("The description of this game type"),
	shortdescription_("The short description of this game type"),
	gamefile_("The settings file that defines this game type")
{
	addChildXMLEntry("description", &description_);
	addChildXMLEntry("shortdescription", &shortdescription_);
	addChildXMLEntry("gamefile", &gamefile_);
	addChildXMLEntry("icon", &icon_);
}

ModInfoGame::~ModInfoGame()
{
}

ModInfoGameList::ModInfoGameList() :
	XMLEntryList<ModInfoGame>("The list of game types for this mod", 0)
{
}

ModInfoGameList::~ModInfoGameList()
{
}

ModInfoGame *ModInfoGameList::createXMLEntry(void *xmlData)
{
	return new ModInfoGame();
}

ModInfoMain::ModInfoMain() :
	XMLEntryContainer("ModInfoMainSection", "Contains the mod description"),
	description_("The description of this mod"),
	shortDescription_("The short description of this mod"),
	url_("The url of the home page for this mod"),
	protocolversion_("The protocol version of Scorched3D that mod is compatible with"),
	icon_("The icon to display for this mod")

{
	addChildXMLEntry("protocolversion", &protocolversion_);
	addChildXMLEntry("description", &description_);
	addChildXMLEntry("shortdescription", &shortDescription_);
	addChildXMLEntry("icon", &icon_);
	addChildXMLEntry("url", &url_);
}

ModInfoMain::~ModInfoMain()
{
}

ModInfoRoot::ModInfoRoot() :
	XMLEntryRoot<XMLEntryContainer>(S3D::eInvalidLocation, "multiple",
		"modinfo", "ModInfo", 
		"Contains the description of a game modification (mod) along with any game modes it supports")
{
	addChildXMLEntry("main", &main_);
	addChildXMLEntry("game", &games_);
}

ModInfoRoot::~ModInfoRoot()
{

}

ModInfo::ModInfo(const std::string &name) :
	name_(name)
{

}

ModInfo::~ModInfo()
{
}

bool ModInfo::parse(const std::string &fileName)
{
	modInfo_.games_.clear();
	return modInfo_.loadFromFile(fileName, true, 0);
}
