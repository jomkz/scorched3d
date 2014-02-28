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

#include <weapons/WeaponMulti.h>
#include <weapons/AccessoryStore.h>
#include <common/Defines.h>

REGISTER_ACCESSORY_SOURCE(WeaponMulti);

XMLEntryWeaponChoiceList::XMLEntryWeaponChoiceList() :
	XMLEntryList<XMLEntryWeaponChoice>("A list of weapons to fire", 1)
{
}

XMLEntryWeaponChoiceList::~XMLEntryWeaponChoiceList()
{
}

XMLEntryWeaponChoice *XMLEntryWeaponChoiceList::createXMLEntry()
{
	return new XMLEntryWeaponChoice();
}

WeaponMulti::WeaponMulti() :
	Weapon("WeaponMulti", "Used to fire off multiple sets of primitives from the same point in time and space. "
		"It is a necessity if you want to do more than one thing at a time. "),
	subWeapons_()
{
	addChildXMLEntry("subweapon", &subWeapons_);
}

WeaponMulti::~WeaponMulti()
{

}

void WeaponMulti::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	std::list<XMLEntryWeaponChoice *>::iterator itor = subWeapons_.getChildren().begin(),
		end = subWeapons_.getChildren().end();
	for (;itor!=end;++itor)
	{
		Weapon *weapon = (*itor)->getValue();
		weapon->fire(context, weaponContext, position, velocity);
	}
}
