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

#include <weapons/ShieldRound.h>
#include <common/VectorLib.h>
#include <common/Defines.h>
#include <math.h>

REGISTER_ACCESSORY_SOURCE(ShieldRound);

ShieldRound::ShieldRound() :
	Shield("ShieldRound", "Normal shields protect the player by simply absorbing the damage."),
	radius_("The circumference of the shield")
{
	addChildXMLEntry("radius", &radius_);
}

ShieldRound::ShieldRound(const char *typeName, const char *description) :
	Shield(typeName, description),
	radius_("The circumference of the shield")
{
	addChildXMLEntry("radius", &radius_);
}

ShieldRound::~ShieldRound()
{
}

Shield::ShieldType ShieldRound::getShieldType()
{
	return ShieldTypeRoundNormal;
}

bool ShieldRound::inShield(FixedVector &offset)
{
	if (offset.Magnitude() <= radius_.getValue())
	{
		return true;
	}
	return false;
}

bool ShieldRound::tankInShield(FixedVector &offset)
{
	if (offset.Magnitude() <= radius_.getValue())
	{
		return true;
	}
	return false;
}

