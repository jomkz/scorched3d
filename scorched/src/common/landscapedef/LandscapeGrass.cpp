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

#include <landscapedef/LandscapeGrass.h>

LandscapeGrass::LandscapeGrass() :
	XMLEntryContainer("LandscapeGrass", 
		"Defines grass that can be added to a landscape texture layer."),
	materialName("The name of the ogre material to be applied to the grass, "
		"this will be automatically re-colored to match the landscape layer to which it is applied."),
	visibleDistance("The maximim distance at which this grass will be visible."),
	density("The maximum amount of grass that will be applied at each area."),
	minimumSize("The minimum size of the grass model/sprites"),
	maximumSize("The maximum size of the grass model/sprites")
{
	addChildXMLEntry("materialname", &materialName);
	addChildXMLEntry("visibledistance", &visibleDistance);
	addChildXMLEntry("density", &density);
	addChildXMLEntry("minimumsize", &minimumSize);
	addChildXMLEntry("maximumsize", &maximumSize);
}

LandscapeGrass::~LandscapeGrass()
{
}

LandscapeGrassList::LandscapeGrassList() : 
	XMLEntryList<LandscapeGrass>("Defines a set of grass layers that will be added to the given landscape layer.")
{
}

LandscapeGrassList::~LandscapeGrassList()
{
}

LandscapeGrass *LandscapeGrassList::createXMLEntry(void *xmlData)
{
	return new LandscapeGrass();
}
