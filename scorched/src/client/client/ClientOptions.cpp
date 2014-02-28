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

static OptionEntryEnum::EnumEntry accessorySortKeyEnum[] =
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
	hostDescription_(options_, "HostDescription",
		"The description of this host given to any servers for stats.", OptionEntry::DataRAccess | OptionEntry::DataNoRestore, ""),
	validateServerIp_(options_, "ValidateServerIp",
		"Checks if the server ip address matches the published address", OptionEntry::DataRAccess, true),
	waterAnimate_(options_, "WaterAnimate",
		"Should we animate the landscape water", OptionEntry::DataRWAccess, true),
	waterDraw_(options_, "WaterDraw",
		"Should we draw the landscape water", OptionEntry::DataRWAccess, true),
	waterWireframe_(options_, "WaterWireframe",
		"Should we draw the landscape water as wireframe", OptionEntry::DataRWAccess, false),
	landscapeDraw_(options_, "LandscapeDraw",
		"Should we draw the landscape", OptionEntry::DataRWAccess, true),
	cameraWireframe_(options_, "CameraWireframe",
		"Should everything we draw be wireframe", OptionEntry::DataRWAccess, false),
	targetsDraw_(options_, "TargetsDraw",
		"Should we draw targets", OptionEntry::DataRWAccess, true)
{
}

ClientOptions::~ClientOptions()
{	
}

bool ClientOptions::writeOptionsToFile(bool allOptions)
{
	std::string path = S3D::getSettingsFile("display.xml");

	// Check the options files are writeable
	FILE *checkfile = fopen(path.c_str(), "a");
	if (!checkfile)
	{
		S3D::dialogMessage(
			"Scorched3D", S3D::formatStringBuffer(
			"Warning: Your display settings file (%s) cannot be\n"
			"written to.  Your settings will not be saved from one game to the next.\n\n"
			"To fix this problem correct the permissions for this file.",
			path.c_str()));
	}
	else fclose(checkfile);

	if (!OptionEntryHelper::writeToFile(options_, path, allOptions)) return false;
	return true;
}

bool ClientOptions::readOptionsFromFile()
{
	std::string path = S3D::getSettingsFile("display.xml");

	if (!OptionEntryHelper::readFromFile(options_, path))
	{
		S3D::dialogMessage(
			"Scorched3D", S3D::formatStringBuffer(
			"Warning: Your display settings file (%s) cannot be\n"
			"read.  This may be because it was create by an out of date version of Scorched3D.\n"
			"If this is the case it can be safely deleted.",
			path.c_str()));
		return false;
	}

	return true;
}

void ClientOptions::loadDefaultValues()
{
        std::list<OptionEntry *>::iterator itor;
        for (itor = options_.begin();
                itor != options_.end();
                ++itor)
	{
		OptionEntry *entry = (*itor);
		if (!(entry->getData() & OptionEntry::DataNoRestore))
		{
			entry->setValueFromString(entry->getDefaultValueAsString());
		}
	}
}

void ClientOptions::loadSafeValues()
{
	std::string path = S3D::getDataFile("data/safedisplay.xml");
	OptionEntryHelper::readFromFile(options_, path);
}

void ClientOptions::loadMediumValues()
{
	std::string path = S3D::getDataFile("data/mediumdisplay.xml");
	OptionEntryHelper::readFromFile(options_, path);
}

void ClientOptions::loadFastestValues()
{
	std::string path = S3D::getDataFile("data/fastestdisplay.xml");
	OptionEntryHelper::readFromFile(options_, path);
}

void ClientOptions::loadUltraValues()
{
	std::string path = S3D::getDataFile("data/ultradisplay.xml");
	OptionEntryHelper::readFromFile(options_, path);
}
