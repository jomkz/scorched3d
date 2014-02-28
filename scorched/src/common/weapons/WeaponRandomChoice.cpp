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

#include <weapons/WeaponRandomChoice.h>
#include <weapons/AccessoryStore.h>
#include <engine/Simulator.h>
#include <common/Defines.h>
#include <stdlib.h>

REGISTER_ACCESSORY_SOURCE(WeaponRandomChoice);

WeaponRandomChoiceItem::WeaponRandomChoiceItem() :
	XMLEntryContainer("WeaponRandomChoiceItem", 
		"A weapon definition with a weight, the weight defines the probability that this weapon will be chosen"),
	weight_("The probability weight")
{
	addChildXMLEntry("weapon", &weapon_);
	addChildXMLEntry("weight", &weight_);
}

WeaponRandomChoiceItem::~WeaponRandomChoiceItem()
{
}

WeaponRandomChoiceItemList::WeaponRandomChoiceItemList() :
	XMLEntryList<WeaponRandomChoiceItem>(
		"A list of WeaponRandomChoiceItem items, only one will be chosen based on a weighted probability", 1)
{
}

WeaponRandomChoiceItemList::~WeaponRandomChoiceItemList()
{
}

WeaponRandomChoiceItem *WeaponRandomChoiceItemList::createXMLEntry(void *xmlData)
{
	return new WeaponRandomChoiceItem();
}

WeaponRandomChoice::WeaponRandomChoice() : 
	Weapon("WeaponRandomChoice", "Used to randomly choose between different primitives."),
	totalWeight_(0)
{
	addChildXMLEntry("weaponchoice", &itemList_);
}

WeaponRandomChoice::~WeaponRandomChoice()
{

}

bool WeaponRandomChoice::readXML(XMLNode *node, void *xmlData)
{
	if (!Weapon::readXML(node, xmlData)) return false;
	std::list<WeaponRandomChoiceItem *>::iterator itor = itemList_.getChildren().begin(),
		end = itemList_.getChildren().end();
	for (;itor!=end;++itor)
	{
		totalWeight_ += (*itor)->weight_.getValue();
	}
	return true;
}

void WeaponRandomChoice::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	RandomGenerator &random = context.getSimulator().getRandomGenerator();
	unsigned int randWeight = random.getRandUInt("WeaponRandomChoice") % totalWeight_;
	unsigned int currentWeight = 0;

	std::list<WeaponRandomChoiceItem *>::iterator itor = itemList_.getChildren().begin(),
		end = itemList_.getChildren().end();
	for (;itor!=end;++itor)
	{
		WeaponRandomChoiceItem *weightEntry = *itor;
		currentWeight += weightEntry->weight_.getValue();

		if (currentWeight >= randWeight)
		{
			Weapon *weapon = weightEntry->weapon_.getValue();
			weapon->fire(context, weaponContext, position, velocity);
			break;
		}
	}
}
