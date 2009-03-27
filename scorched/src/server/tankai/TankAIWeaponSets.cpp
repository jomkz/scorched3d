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

#include <tankai/TankAIWeaponSets.h>
#include <tank/Tank.h>
#include <tank/TankScore.h>
#include <tank/TankAccessories.h>
#include <server/ScorchedServer.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <weapons/AccessoryStore.h>
#include <XML/XMLFile.h>

TankAIWeaponSets *TankAIWeaponSets::instance()
{
	static TankAIWeaponSets instance;
	return &instance;
}

TankAIWeaponSets::TankAIWeaponSets()
{
	parseConfig();
}

TankAIWeaponSets::~TankAIWeaponSets()
{
}

bool TankAIWeaponSets::parseConfig()
{
	XMLFile file;
	std::string fileName = S3D::getModFile("data/tankaiweaponsets.xml");
	if (!file.readFile(fileName.c_str()))
	{
		S3D::dialogMessage("TankAIWeaponSets", S3D::formatStringBuffer(
			"Failed to parse \"%s\":%s\n", 
			fileName.c_str(),
			file.getParserError()));
		return false;
	}
	if (!file.getRootNode())
	{
		S3D::dialogMessage("TankAIWeaponSets", S3D::formatStringBuffer(
			"Failed to weapon sets definition file \"%s\"",
				fileName.c_str()));
		return false;		
	}

	XMLNode *weaponsetNode = 0;
	while (file.getRootNode()->getNamedChild("weaponset", weaponsetNode, false))
	{
		WeaponSet weaponSet;
		if (!weaponSet.parseConfig(weaponsetNode)) return false;
		weaponSets_[weaponSet.name] = weaponSet;
	}

	return file.getRootNode()->failChildren();
}

TankAIWeaponSets::WeaponSet *TankAIWeaponSets::getWeaponSet(const char *name)
{
	std::map<std::string, WeaponSet>::iterator findItor =
		weaponSets_.find(name);
	if (findItor == weaponSets_.end()) return 0;

	return &findItor->second;
}

bool TankAIWeaponSets::WeaponSet::parseConfig(XMLNode *node)
{
	if (!node->getNamedChild("name", name)) return false;
	XMLNode *setNode = 0, *weaponNode = 0;
	if (!node->getNamedChild("set", setNode)) return false;
	while (setNode->getNamedChild("weapon", weaponNode, false))
	{
		WeaponSetEntry weapon;
		if (!weapon.parseConfig(weaponNode)) return false;
		weapons.push_back(weapon);
	}

	return node->failChildren();
}

void TankAIWeaponSets::WeaponSet::buyWeapons(Tank *tank, bool lastRound)
{
	for (;;)
	{
		// Get all of the weapons we can buy
		std::multimap<unsigned int, WeaponSetEntry *> potentialWeapons;
		std::vector<WeaponSetEntry>::iterator itor;
		for (itor = weapons.begin();
			itor != weapons.end();
			itor++)
		{
			WeaponSetEntry &weapon = *itor;
			if (weapon.weaponValid(tank, lastRound))
			{
				potentialWeapons.insert(
					std::pair<unsigned int, WeaponSetEntry *>
						(weapon.prioritybuy, &weapon));
			}
		}
		if (potentialWeapons.empty()) break;

		// Choose one of the potential weapons
		// from the list of the highest priority
		std::vector<WeaponSetEntry *> priorityWeapons;
		std::multimap<unsigned int, WeaponSetEntry *>::reverse_iterator ritor;
		for (ritor = potentialWeapons.rbegin();
			ritor != potentialWeapons.rend();
			ritor++)
		{
			WeaponSetEntry *weapon = ritor->second;
			priorityWeapons.push_back(weapon);
			if (weapon->prioritybuy < priorityWeapons.back()->prioritybuy) break;
		}
		WeaponSetEntry *choosenWeapon = priorityWeapons[rand() % priorityWeapons.size()];
		Accessory *choosenAccessory = choosenWeapon->accessory;

		// Buy this weapon
		tank->getAccessories().add(choosenAccessory, choosenAccessory->getBundle());
		tank->getScore().setMoney(tank->getScore().getMoney() - 
			choosenAccessory->getPrice());
	}
}

Accessory *TankAIWeaponSets::WeaponSet::
	getTankAccessoryByType(Tank *tank, const char *getType)
{
	DIALOG_ASSERT(WeaponSetEntry::checkType(getType));

	WeaponSetEntry *result = 0;

	std::vector<WeaponSetEntry>::iterator itor;
	for (itor = weapons.begin();
		itor != weapons.end();
		itor++)
	{
		WeaponSetEntry &current = *itor;
		if (current.type == getType)
		{
			if ((10 - current.accessory->getArmsLevel()) <=
				ScorchedServer::instance()->getOptionsTransient().getArmsLevel() ||
				ScorchedServer::instance()->getOptionsGame().getGiveAllWeapons())
			{
				if (tank->getAccessories().canUse(current.accessory))
				{
					if (!result ||
						result->priorityuse < current.priorityuse)
					{
						result = &current;
					}
				}
			}
		}
	}

	return (result?result->accessory:0);
}

bool TankAIWeaponSets::WeaponSetEntry::parseConfig(XMLNode *node)
{
	std::string name;
	if (!node->getNamedChild("name", name)) return false;
	accessory = ScorchedServer::instance()->getAccessoryStore().
		findByPrimaryAccessoryName(name.c_str());
	if (!accessory)
	{
		return node->returnError(
			S3D::formatStringBuffer("Unknown accessory %s", name.c_str()));
	}

	if (!node->getNamedChild("buymin", buymin)) return false;
	if (!node->getNamedChild("buymax", buymax)) return false;
	if (!node->getNamedChild("moneymin", moneymin)) return false;
	if (!node->getNamedChild("moneymax", moneymax)) return false;
	if (!node->getNamedChild("prioritybuy", prioritybuy)) return false;
	if (!node->getNamedChild("priorityuse", priorityuse)) return false;
	if (!node->getNamedChild("type", type)) return false;

	if (!checkType(type.c_str()))
	{
		return node->returnError(
			S3D::formatStringBuffer("Unknown type %s", type.c_str()));
	}

	return node->failChildren();
}

bool TankAIWeaponSets::WeaponSetEntry::checkType(const char *type)
{
	if (0 != strcmp(type, "explosionhuge") &&
		0 != strcmp(type, "explosionlarge") &&
		0 != strcmp(type, "explosionsmall") &&
		0 != strcmp(type, "uncover") &&
		0 != strcmp(type, "digger") &&
		0 != strcmp(type, "roller") &&
		0 != strcmp(type, "napalm") &&
		0 != strcmp(type, "laser") &&
		0 != strcmp(type, "shield") &&
		0 != strcmp(type, "other") &&
		0 != strcmp(type, "fuel") &&
		0 != strcmp(type, "autodefense") &&
		0 != strcmp(type, "parachute"))
	{
		return false;
	}
	return true;
}

bool TankAIWeaponSets::WeaponSetEntry::weaponValid(Tank *tank, bool lastRound)
{
	if (accessory->getNoBuy()) return false;

	int currentCount = tank->getAccessories().getAccessoryCount(accessory);
	int currentMoney = tank->getScore().getMoney();

	int maxCount = accessory->getMaximumNumber();
	if (currentCount >= maxCount) return false;

	if (currentCount < 0) return false;
	if (currentMoney < accessory->getPrice()) return false;

	if ((10 - accessory->getArmsLevel()) <=
		ScorchedServer::instance()->getOptionsTransient().getArmsLevel() ||
		ScorchedServer::instance()->getOptionsGame().getGiveAllWeapons()) {}
	else return false;

	if (type == "autodefense")
	{
		// Don't buy autodefense when simultaneous
		if (ScorchedServer::instance()->getOptionsGame().getTurnType() == 
			OptionsGame::TurnSimultaneous)
		{
			return false;
		}
	}

	if (!lastRound)
	{
		if (currentCount > buymin) return false;
		if ((currentMoney < moneymin && moneymin != 0) || 
			(currentMoney > moneymax && moneymax != 0)) return false;
	}
	
	return true;
}
