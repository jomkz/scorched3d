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

#include <landscapedef/LandscapeDefn.h>
#include <landscapedef/LandscapeDefnTankStart.h>
#include <landscapedef/LandscapeDefinitions.h>
#include <common/Defines.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

LandscapeDefn::LandscapeDefn(LandscapeDefinitions *definitions) :
	LandscapeInclude(definitions, "LandscapeDefn", 
		"A landscape/scene definition, usualy related to the physical aspects of the landscape"),
	minplayers("The minimum number of players that must be playing before this map is chosen"),
	maxplayers("The maximum number of players that must be playing before this map is chosen"),
	landscapewidth("The width  of the landscape, must be a multiple of 128"),
	landscapeheight("The height  of the landscape, must be a multiple of 128"),
	arenawidth("The playable width of the landscape", 0, -1),
	arenaheight("The playable height of the landscape", 0, -1)
{
	addChildXMLEntry("minplayers", &minplayers, "maxplayers", &maxplayers);
	addChildXMLEntry("landscapewidth", &landscapewidth, "landscapeheight", &landscapeheight, "arenawidth", &arenawidth, "arenaheight", &arenaheight);
	addChildXMLEntry("tankstart", &tankstart, "heightmap", &heightmap, "deform", &deform);
}

LandscapeDefn::~LandscapeDefn()
{
}

bool LandscapeDefn::readXML(XMLNode *node)
{
	if (!LandscapeInclude::readXML(node)) return false;

	if (landscapewidth.getValue() % 128 != 0 ||
		landscapeheight.getValue() % 128 != 0)
	{
		S3D::dialogMessage("Scorched3D",
			S3D::formatStringBuffer(
				"ERROR: Landscape width and height must each be a multiple of 128.\n"
				"Specified size : %ix%i", landscapewidth, landscapeheight));
		return false;
	}

	if (arenawidth.getValue() == -1) arenawidth.setValue(landscapewidth.getValue());
	if (arenaheight.getValue() == -1) arenaheight.setValue(landscapeheight.getValue());
	if (arenawidth.getValue() > landscapewidth.getValue() ||
		arenaheight.getValue() > landscapeheight.getValue())
	{
		S3D::dialogMessage("Scorched3D",
			S3D::formatStringBuffer(
				"ERROR: Arena width and height must each be less than or equal to the landscape size.\n"
				"Specified size : %ix%i", arenawidth, arenaheight));
		return false;
	}
	arenax = (landscapewidth.getValue() - arenawidth.getValue()) / 2;
	arenay = (landscapeheight.getValue() - arenaheight.getValue()) / 2;

	return true;
}

