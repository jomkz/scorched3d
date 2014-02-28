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

#include <client/ClientOptions.h>
#include <common/Defines.h>
#include <stdio.h>

// This is a mirror of AccessoryStore::SortKey.

static XMLEntryEnum::EnumEntry accessorySortKeyEnum[] =
{
	{ "SortNothing", 0 },
	{ "SortName", 1 },
	{ "SortPrice", 2 },
	{ "", -1 }
};

ClientOptions *ClientOptions::instance_ = 0;

ClientOptions *ClientOptions::instance()
{
	if (!instance_)
	{
		instance_ = new ClientOptions;
	}

	return instance_;
}

ClientOptions::ClientOptions() :
	XMLEntryRoot<XMLEntrySimpleContainer>(S3D::eSettingsLocation, 
		"display.xml", "ClientOptions", 
		"ClientOptions", 
		"The Scorched3D client side options, these options change aspects of the game not relating to gameplay"),
	hostDescription_("The description of this host given to any servers for stats.", XMLEntry::eDataRWAccess | XMLEntry::eDataNoRestore, ""),
	validateServerIp_("Checks if the server ip address matches the published address", XMLEntry::eDataRWAccess, true),
	waterAnimate_("Should we animate the landscape water", XMLEntry::eDataRWAccess, true),
	waterDraw_("Should we draw the landscape water", XMLEntry::eDataRWAccess, true),
	waterWireframe_("Should we draw the landscape water as wireframe", XMLEntry::eDataRWAccess | XMLEntry::eDataDebugOnly, false),
	skyDraw_("Should we draw the sky", XMLEntry::eDataRWAccess, true),
	landscapeDraw_("Should we draw the landscape", XMLEntry::eDataRWAccess, true),
	landscapeGridDraw_("Should we draw the landscape grid", XMLEntry::eDataRWAccess | XMLEntry::eDataDebugOnly, false),
	shadows_("Should we draw shadows", XMLEntry::eDataRWAccess, false),
	cameraWireframe_("Should everything we draw be wireframe", XMLEntry::eDataRWAccess | XMLEntry::eDataDebugOnly, false),
	targetsDraw_("Should we draw targets", XMLEntry::eDataRWAccess, true)
{
	addChildXMLEntry("WaterAnimate", &waterAnimate_, "WaterDraw", &waterDraw_, "WaterWireframe", &waterWireframe_);
	addChildXMLEntry("SkyDraw", &skyDraw_);
	addChildXMLEntry("LandscapeDraw", &landscapeDraw_, "LandscapeGridDraw", &landscapeGridDraw_);
	addChildXMLEntry("CameraWireframe", &cameraWireframe_);
	addChildXMLEntry("TargetsDraw", &targetsDraw_);
	addChildXMLEntry("Shadows", &shadows_);
	addChildXMLEntry("ValidateServerIp", &validateServerIp_, "HostDescription", &hostDescription_);
}

ClientOptions::~ClientOptions()
{	
}

bool ClientOptions::writeOptionsToFile()
{
	checkFile();
	return saveFile(0);
}

bool ClientOptions::readOptionsFromFile()
{
	return loadFile(0);
}

void ClientOptions::loadDefaultValues()
{
	std::map<std::string, XMLEntry *>::iterator itor = xmlEntryChildren_.begin(),
		end = xmlEntryChildren_.end();
	for (;itor!=end;++itor)
	{
		XMLEntrySimpleType *entry = (XMLEntrySimpleType *) itor->second;
		if (!(entry->getData() & XMLEntry::eDataNoRestore))
		{
			entry->resetDefaultValue();
		}
	}
}

void ClientOptions::loadSafeValues()
{
	std::string path = S3D::getDataFile("data/safedisplay.xml");
	loadFromFile(path, 0);
}

void ClientOptions::loadMediumValues()
{
	std::string path = S3D::getDataFile("data/mediumdisplay.xml");
	loadFromFile(path, 0);
}

void ClientOptions::loadFastestValues()
{
	std::string path = S3D::getDataFile("data/fastestdisplay.xml");
	loadFromFile(path, 0);
}

void ClientOptions::loadUltraValues()
{
	std::string path = S3D::getDataFile("data/ultradisplay.xml");
	loadFromFile(path, 0);
}
