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

#include <landscapedef/LandscapeWater.h>

LandscapeWater::LandscapeWater() :
	XMLEntryContainer("LandscapeWater", 
		"Defines water."),
	waterHeight("The height of the landscape water", 0, 1),
	strength("Water perlin noise strength governs the height of the waves", 0, 35),
	smooth("Smooth vertex", 0, false),
	choppywaves("Choppy waves enabled", 0, true),
	choppystrength("Choppy waves strength", 0, fixed(true, 37500))
{
	addChildXMLEntry("waterHeight", &waterHeight);
	addChildXMLEntry("strength", &strength);
	addChildXMLEntry("smooth", &smooth);
	addChildXMLEntry("choppywaves", &choppywaves);
	addChildXMLEntry("choppystrength", &choppystrength);
	addChildXMLEntry("watercolorgradient", &watergradient);
}

LandscapeWater::~LandscapeWater()
{
}

