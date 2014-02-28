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

#include <tankai/TankAIShallow.h>

TankAIShallow::TankAIShallow() :
	TankAI("TankAIShallow", "A shallow parsed AI definition (internal use only)")
{
}

TankAIShallow::~TankAIShallow()
{
}

TankAIShallowList::TankAIShallowList() :
	XMLEntryList<TankAIShallow>("", 1)
{
}

TankAIShallowList::~TankAIShallowList()
{
}

TankAIShallow *TankAIShallowList::createXMLEntry(void *xmlData)
{
	return new TankAIShallow();
}

TankAIShallowFile::TankAIShallowFile() :
	XMLEntryRoot<XMLEntryContainer>(S3D::eModLocation, "data/tankais.xml", "ais",
		"TankAIsShallow", "The list of available tank AIs (bots)")
{
	addChildXMLEntry("ai", &tankAis_);
}

TankAIShallowFile::~TankAIShallowFile()
{
}
