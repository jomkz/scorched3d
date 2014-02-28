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

#include <landscapedef/LandscapeDefnDeform.h>

LandscapeDefnDeformChoice::LandscapeDefnDeformChoice() :
	XMLEntryTypeChoice<LandscapeDefnDeform>("LandscapeDefnDeformChoice", 
		"Defines how the landscape surface can be deformed by player actions")
{
}

LandscapeDefnDeformChoice::~LandscapeDefnDeformChoice()
{
}

LandscapeDefnDeform *LandscapeDefnDeformChoice::createXMLEntry(const std::string &type)
{
	if (0 == strcmp(type.c_str(), "solid")) return new LandscapeDefnDeformSolid;
	if (0 == strcmp(type.c_str(), "deform")) return new LandscapeDefnDeformDeform;
	if (0 == strcmp(type.c_str(), "file")) return new LandscapeDefnDeformFile;
	S3D::dialogMessage("LandscapeDefnType", S3D::formatStringBuffer("Unknown deform type %s", type));
	return 0;
}

LandscapeDefnDeform::LandscapeDefnDeform(const char *name, const char *description) :
	XMLEntryContainer(name, description)
{
}

LandscapeDefnDeform::~LandscapeDefnDeform()
{
}

LandscapeDefnDeformFile::LandscapeDefnDeformFile() :
	LandscapeDefnDeform("LandscapeDefnDeformFile", 
		"Defines the areas of the lansdscape that can be deformed by player actions via a mask file"),
	file("The file containing the mask that cannot be deformed"),
	levelsurround("A flag indicating if the surround of the mask should be flattened")
{
	addChildXMLEntry("file", &file, "levelsurround", &levelsurround);
}

LandscapeDefnDeformFile::~LandscapeDefnDeformFile()
{
}

LandscapeDefnDeformSolid::LandscapeDefnDeformSolid() :
	LandscapeDefnDeform("LandscapeDefnDeformSolid", 
		"Specifies that the landscape cannot be deformed by player actions")
{
}

LandscapeDefnDeformSolid::~LandscapeDefnDeformSolid()
{
}

LandscapeDefnDeformDeform::LandscapeDefnDeformDeform() :
	LandscapeDefnDeform("LandscapeDefnDeformDeform", 
		"Specifies that the landscape can be deformed by player actions i.e. the normal scenario")
{
}

LandscapeDefnDeformDeform::~LandscapeDefnDeformDeform()
{
}