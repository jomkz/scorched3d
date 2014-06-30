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

#include <landscapedef/LandscapeColorGradient.h>

LandscapeColorGradientList::LandscapeColorGradientList() :
	XMLEntryList<XMLEntryFixedVector4>("A list of colors that will be used to form a color change gradient.  "
		"Colors will be picked from the gradient depending on the time of the day.")
{
}

LandscapeColorGradientList::~LandscapeColorGradientList()
{

}

XMLEntryFixedVector4 *LandscapeColorGradientList::createXMLEntry(void *xmlData)
{
	return new XMLEntryFixedVector4("Color");
}

LandscapeColorGradient::LandscapeColorGradient() :
	XMLEntryContainer("LandscapeColorGradient", 
		"A list of colors that will be used to form a color change gradient.  "
		"Colors will be picked from the gradient depending on the time of the day.")
{
	addChildXMLEntry("color", &colors);
}

LandscapeColorGradient::~LandscapeColorGradient()
{
}

