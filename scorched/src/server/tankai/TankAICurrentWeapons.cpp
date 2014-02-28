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

#include <server/ScorchedServer.h>
#include <tankai/TankAICurrentWeapons.h>
#include <XML/XMLNode.h>
#include <stdlib.h>

TankAICurrentWeaponsEntryList::TankAICurrentWeaponsEntryList() :
	XMLEntryList<XMLEntryString>("The set of weapons that this ai has access to", 1)
{
}

TankAICurrentWeaponsEntryList::~TankAICurrentWeaponsEntryList()
{
}

XMLEntryString *TankAICurrentWeaponsEntryList::createXMLEntry(void *xmlData)
{
	return new XMLEntryString("The name of a weapon set that this ai has access to");
}

TankAICurrentWeapons::TankAICurrentWeapons() : 
	XMLEntryContainer("TankAIWeapons", "Defines the set of weapons that this tank will use.  "
		"If more than one set is provided this tank ai will choose a random set.  "
		"This random choice will then be used until the AI disconnects."),
	currentWeaponSet_(0)
{
	addChildXMLEntry("weaponset", &weaponEntryList_);
}

TankAICurrentWeapons::~TankAICurrentWeapons()
{
}

TankAIWeaponSets::WeaponSet *TankAICurrentWeapons::getCurrentWeaponSet()
{
	if (!currentWeaponSet_)
	{
		currentWeaponSet_ = weaponSets_[rand() % weaponSets_.size()];
	}
	return currentWeaponSet_;
}

bool TankAICurrentWeapons::readXML(XMLNode *node, void *xmlData)
{
	if (!XMLEntryContainer::readXML(node, xmlData)) return false;

	TankAIWeaponSets *sets = (TankAIWeaponSets *) xmlData;
	std::list<XMLEntryString *>::iterator itor = weaponEntryList_.getChildren().begin(),
		end = weaponEntryList_.getChildren().end();
	for (;itor!=end;++itor)
	{
		TankAIWeaponSets::WeaponSet *weaponSet = 
			sets->getWeaponSet((*itor)->getValue().c_str());
		if (!weaponSet) node->returnError(
			S3D::formatStringBuffer("Cannot find tank AI weapon set \"%s\"", 
			(*itor)->getValue().c_str()));
		weaponSets_.push_back(weaponSet);
	}
	if (weaponSets_.empty())
	{
		return node->returnError("Must define at least one weaponset");
	}

	return true;
}

void TankAICurrentWeapons::buyWeapons(TankAIWeaponSets::WeaponSetAccessories &tankAccessories, bool lastRound)
{
	getCurrentWeaponSet()->buyWeapons(tankAccessories, lastRound);
}
