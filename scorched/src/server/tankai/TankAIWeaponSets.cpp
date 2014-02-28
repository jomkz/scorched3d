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

#include <tankai/TankAIWeaponSets.h>
#include <tank/Tank.h>
#include <tank/TankScore.h>
#include <server/ScorchedServer.h>
#include <server/ServerSimulator.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <common/Defines.h>
#include <weapons/AccessoryStore.h>
#include <simactions/TankAccessorySimAction.h>
#include <XML/XMLFile.h>

static XMLEntryStringEnum::EnumEntry weaponSetTypeEnum[] =
{
	{ "explosionhuge" },
	{ "explosionlarge" },
	{ "explosionsmall" },
	{ "uncover" },
	{ "digger" },
	{ "roller" },
	{ "napalm" },
	{ "laser" },
	{ "shield" },
	{ "other" },
	{ "fuel" },
	{ "autodefense" },
	{ "parachute" },
	{ "" }
};

TankAIWeaponSets::WeaponSetAccessories::WeaponSetAccessories(Tanket *tanket) :
	tankAccessories(ScorchedServer::instance()->getContext())
{
	tankAccessories.setTanket(tanket);
	NetBuffer buffer;
	tanket->getAccessories().writeMessage(buffer, true);
	NetBufferReader reader(buffer);
	tankAccessories.readMessage(reader);
	if (tanket->getType() == Target::TypeTank)
	{
		tankMoney = ((Tank *) tanket)->getScore().getMoney();
	}
	else
	{
		tankMoney = 0;
	}
	tankId = tanket->getPlayerId();
}

TankAIWeaponSets::TankAIWeaponSets() :
	XMLEntryRoot<XMLEntryContainer>(S3D::eModLocation, "data/tankaiweaponsets.xml", "weaponsets",
		"TankAIWeaponSets",
		"A set of accessories that can be selected for use by a tank AI.  "
		"Each set tells the tank AI which accessories to buy (and when) and what accessories to use (and when)."
		"Different sets can be defined to give tank AIs different buying/usage personalities, or to adjust their skill level.")
{
	addChildXMLEntry("weaponset", &weaponSets_);
}

TankAIWeaponSets::~TankAIWeaponSets()
{
}

bool TankAIWeaponSets::parseConfig()
{
	if (!loadFile(true)) return false;
	return true;
}

TankAIWeaponSets::WeaponSet *TankAIWeaponSets::getWeaponSet(const char *name)
{
	std::list<WeaponSet *>::iterator itor = weaponSets_.getChildren().begin(),
		end = weaponSets_.getChildren().end();
	for (;itor!=end;++itor)
	{
		if ((*itor)->name.getValue() == name) return *itor;
	}
	return 0;
}

TankAIWeaponSets::WeaponSet::WeaponSet() :
	XMLEntryContainer("TankAIWeaponSet", 
		"A set of accessories that can be selected for use by a tank AI."),
	name("The name given to this weapon set, weapon sets are refered to by name in the tank ai definition file")
{
	addChildXMLEntry("name", &name);
	addChildXMLEntry("weapon", &weapons);
}

TankAIWeaponSets::WeaponSet::~WeaponSet()
{
}

void TankAIWeaponSets::WeaponSet::buyWeapons(WeaponSetAccessories &tankAccessories, bool lastRound)
{
	for (;;)
	{
		// Get all of the weapons we can buy
		std::multimap<unsigned int, WeaponSetEntry *> potentialWeapons;
		std::list<WeaponSetEntry *>::iterator itor;
		for (itor = weapons.getChildren().begin();
			itor != weapons.getChildren().end();
			++itor)
		{
			WeaponSetEntry &weapon = *(*itor);
			if (weapon.weaponValid(tankAccessories, lastRound))
			{
				potentialWeapons.insert(
					std::pair<const unsigned int, WeaponSetEntry *>
						(weapon.prioritybuy.getValue(), &weapon));
			}
		}
		if (potentialWeapons.empty()) break;

		// Choose one of the potential weapons
		// from the list of the highest priority
		std::vector<WeaponSetEntry *> priorityWeapons;
		std::multimap<unsigned int, WeaponSetEntry *>::reverse_iterator ritor;
		for (ritor = potentialWeapons.rbegin();
			ritor != potentialWeapons.rend();
			++ritor)
		{
			WeaponSetEntry *weapon = ritor->second;
			priorityWeapons.push_back(weapon);
			if (weapon->prioritybuy.getValue() < priorityWeapons.back()->prioritybuy.getValue()) break;
		}
		WeaponSetEntry *choosenWeapon = priorityWeapons[rand() % priorityWeapons.size()];
		Accessory *choosenAccessory = choosenWeapon->accessory;

		// Buy this weapon
		tankAccessories.tankAccessories.add(choosenAccessory, choosenAccessory->getBundle(), false);
		tankAccessories.tankMoney -= choosenAccessory->getPrice();
		if (tankAccessories.tankMoney < 0) tankAccessories.tankMoney = 0;

		ComsBuyAccessoryMessage buyMessage(
			tankAccessories.tankId, choosenAccessory->getAccessoryId(), true);
		TankAccessorySimAction *buyAction = new TankAccessorySimAction(buyMessage);
		ScorchedServer::instance()->getServerSimulator().addSimulatorAction(buyAction);
	}
}

Accessory *TankAIWeaponSets::WeaponSet::getTankAccessoryByType(
	Tanket *tanket, const char *getType)
{
	WeaponSetEntry *result = 0;

	std::list<WeaponSetEntry *>::iterator itor;
	for (itor = weapons.getChildren().begin();
		itor != weapons.getChildren().end();
		++itor)
	{
		WeaponSetEntry &current = *(*itor);
		if (current.type.getValue() == getType)
		{
			if ((10 - current.accessory->getArmsLevel()) <=
				ScorchedServer::instance()->getOptionsTransient().getArmsLevel() ||
				ScorchedServer::instance()->getOptionsGame().getGiveAllWeapons())
			{
				if (tanket->getAccessories().canUse(current.accessory))
				{
					if (!result ||
						result->priorityuse.getValue() < current.priorityuse.getValue())
					{
						result = &current;
					}
				}
			}
		}
	}

	return (result?result->accessory:0);
}

TankAIWeaponSets::WeaponSetEntry::WeaponSetEntry() :
	XMLEntryContainer("TankAIWeaponSetAccessory", 
		"The definition of an accessory that a tank AI can use, and under which criteria it will buy and use the weapon"),
	accessoryName("The name of an accessory, as defined in the accessories.xml file"),
	buymin("This accessory will only be bought if the tank a count of this accessory lower than this value. "
		"i.e. stop buying this accessory once a certain minimum amount has been reached"),
	buymax("Not used"),
	moneymin("This accessory will only be bought if the tank has more money than this value."
		"i.e. only buy this accessory if we have a certain minimum amount of money"),
	moneymax("This accessory will only be bought if the tank has less money than this value."
		"i.e. only buy this accessory if we have a lower than a maximum amount of money"),
	prioritybuy("If there is more than one weapon that matches the other criteria, this is the priority this weapon will be bought"),
	priorityuse("If there is more than one weapon that matches the other criteria, this is the priority this weapon will be used/fired"),
	type("How this accessory should be treated/used.  The game can't automaticaly tell what type of accessory each accessory is",
		weaponSetTypeEnum)
{
	addChildXMLEntry("name", &accessoryName);
	addChildXMLEntry("buymin", &buymin);
	addChildXMLEntry("buymax", &buymax);
	addChildXMLEntry("moneymin", &moneymin);
	addChildXMLEntry("moneymax", &moneymax);
	addChildXMLEntry("prioritybuy", &prioritybuy);
	addChildXMLEntry("priorityuse", &priorityuse);
	addChildXMLEntry("type", &type);
}

TankAIWeaponSets::WeaponSetEntry::~WeaponSetEntry()
{
}

bool TankAIWeaponSets::WeaponSetEntry::readXML(XMLNode *node, void *xmlData)
{
	if (!XMLEntryContainer::readXML(node, xmlData)) return false;
	accessory = ScorchedServer::instance()->getAccessoryStore().
		findByPrimaryAccessoryName(accessoryName.getValue().c_str());
	if (!accessory)
	{
		return node->returnError(
			S3D::formatStringBuffer("Unknown accessory %s", accessoryName.getValue().c_str()));
	}
	return true;
}

bool TankAIWeaponSets::WeaponSetEntry::weaponValid(
	WeaponSetAccessories &tankAccessories, bool lastRound)
{
	if (!tankAccessories.tankAccessories.accessoryAllowed(accessory, accessory->getBundle()))
	{
		return false;
	}

	int currentMoney = tankAccessories.tankMoney;
	if (currentMoney < accessory->getPrice()) return false;

	if (type.getValue() == "autodefense")
	{
		// Don't buy autodefense when simultaneous
		if (ScorchedServer::instance()->getOptionsGame().getTurnType() == 
			OptionsGame::TurnSimultaneous)
		{
			return false;
		}
	}

	int currentCount = tankAccessories.tankAccessories.getAccessoryCount(accessory);
	if (!lastRound)
	{
		if (currentCount > buymin.getValue()) return false;
		if ((currentMoney < moneymin.getValue() && moneymin.getValue() != 0) || 
			(currentMoney > moneymax.getValue() && moneymax.getValue() != 0)) return false;
	}
	
	return true;
}

TankAIWeaponSets::WeaponSetEntryList::WeaponSetEntryList() :
	XMLEntryList<TankAIWeaponSets::WeaponSetEntry>("The collection of weapons in this weapon set", 0)
{
}

TankAIWeaponSets::WeaponSetEntryList::~WeaponSetEntryList()
{
}

TankAIWeaponSets::WeaponSetEntry *TankAIWeaponSets::WeaponSetEntryList::createXMLEntry(void *xmlData)
{
	return new TankAIWeaponSets::WeaponSetEntry();
}

TankAIWeaponSets::WeaponSetList::WeaponSetList() :
	XMLEntryList<TankAIWeaponSets::WeaponSet>("The collection of weaponsets", 1)
{
}

TankAIWeaponSets::WeaponSetList::~WeaponSetList()
{
}

TankAIWeaponSets::WeaponSet *TankAIWeaponSets::WeaponSetList::createXMLEntry(void *xmlData)
{
	return new TankAIWeaponSets::WeaponSet();
}
