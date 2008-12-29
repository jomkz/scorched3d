////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2009
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
//    You should have received a copy of the GNU General Public License
//    along with Scorched3D; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

#include <weapons/EconomyNone.h>
#include <weapons/AccessoryStore.h>

REGISTER_CLASS_SOURCE(EconomyNone);

EconomyNone::EconomyNone()
{
}

EconomyNone::~EconomyNone()
{
}

bool EconomyNone::loadPrices()
{
	// There are no new prices to load
	return true;
}

bool EconomyNone::savePrices()
{
	// There are no new prices to save
	return true;
}

void EconomyNone::calculatePrices()
{
	// There are no new prices to calculate
}

void EconomyNone::accessoryBought(Tank *tank, 
		const char *accessoryName)
{
	// Don't need to do anything
}

void EconomyNone::accessorySold(Tank *tank, 
		const char *accessoryName)
{
	// Don't need to do anything
}

