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

#include <weapons/ShieldSquare.h>

REGISTER_ACCESSORY_SOURCE(ShieldSquare);

ShieldSquare::ShieldSquare() :
	Shield("ShieldSquare", "A square shield centered on the tank"),
	size_("The size in x,y,z coordinates of the square shield")
{
	addChildXMLEntry("size", &size_);
}

ShieldSquare::ShieldSquare(const char *typeName, const char *description) :
	Shield(typeName, description),
	size_("The size in x,y,z coordinates of the square shield")
{
	addChildXMLEntry("size", &size_);
}

ShieldSquare::~ShieldSquare()
{
}

Shield::ShieldType ShieldSquare::getShieldType()
{
	return ShieldTypeSquareNormal;
}

bool ShieldSquare::inShield(FixedVector &offset)
{
	return 
		offset[0] > -size_.getValue()[0] &&
		offset[0] < size_.getValue()[0] &&
		offset[1] > -size_.getValue()[1] &&
		offset[1] < size_.getValue()[1] &&
		offset[2] > -size_.getValue()[2] &&
		offset[2] < size_.getValue()[2];
}

bool ShieldSquare::tankInShield(FixedVector &offset)
{
	return inShield(offset);
}