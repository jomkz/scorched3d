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

#include <weapons/WeaponGiveAccessory.h>
#include <weapons/AccessoryStore.h>
#include <engine/ActionController.h>
#include <target/TargetContainer.h>
#include <tanket/TanketAccessories.h>
#include <tank/Tank.h>
#include <tank/TankScore.h>
#include <common/Defines.h>
#include <common/ChannelManager.h>
#include <lang/LangResource.h>

WeaponGiveAccessoryList::WeaponGiveAccessoryList() :
	XMLEntryList<XMLEntryString>("A list of accessory names that will be given to the target player", 1)
{
}

WeaponGiveAccessoryList::~WeaponGiveAccessoryList()
{
}

XMLEntryString *WeaponGiveAccessoryList::createXMLEntry(void *xmlData)
{
	return new XMLEntryString("The name of an accessory that will be given to the player");
}

REGISTER_ACCESSORY_SOURCE(WeaponGiveAccessory);

WeaponGiveAccessory::WeaponGiveAccessory() :
	WeaponCallback("WeaponGiveAccessory",
		"Gives a specified number of the specified accessory to the player. "
		"It can also be used to take away a specified amount of that accessory if you supply a negative number."),
	number_("Amount of this accessory to give to the player"),
	giveAccessories_()
{
	addChildXMLEntry("number", &number_);
	addChildXMLEntry("giveaccessory", &giveAccessories_);
}

WeaponGiveAccessory::~WeaponGiveAccessory()
{

}

bool WeaponGiveAccessory::readXML(XMLNode *node, void *xmlData)
{
	if (!WeaponCallback::readXML(node, xmlData)) return false;

	std::list<XMLEntryString *>::iterator itor = giveAccessories_.getChildren().begin(),
		end = giveAccessories_.getChildren().end();
	for (;itor!=end;++itor)
	{
		AccessoryCreateContext *context = (AccessoryCreateContext *) xmlData;
		if (context)
		{
			Accessory *accessory = context->getAccessoryStore().
				findByPrimaryAccessoryName((*itor)->getValue().c_str());
			if (!accessory)
			{
				return node->returnError(
					S3D::formatStringBuffer("Failed to find accessory named %s",
						(*itor)->getValue().c_str()));
			}
			giveAccessoriesList_.push_back(accessory);
		}
	}
	return true;
}

void WeaponGiveAccessory::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	context.getActionController().addAction(
		new CallbackWeapon("WeaponGiveAccessory", this, 0, 0, 
			weaponContext, position, velocity));
}

void WeaponGiveAccessory::weaponCallback(
	ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity,
	unsigned int userData)
{
	Tank *tank = context.getTargetContainer().getTankById(weaponContext.getPlayerId());
	if (!tank) return;

	std::vector<Accessory *>::iterator itor;
	for (itor = giveAccessoriesList_.begin();
		itor != giveAccessoriesList_.end();
		++itor)
	{
		Accessory *accessory = (*itor);

		if (number_.getValue() > 0)
		{
			if (tank->getAccessories().accessoryAllowed(accessory, number_.getValue()))
			{
				tank->getAccessories().add(accessory, number_.getValue());
				{
					ChannelText text("combat", 
						LANG_RESOURCE_3("TANK_GET_ACCESSORY",
						"[p:{0}] received {1} * [w:{2}]", 
						tank->getTargetName(),
						S3D::formatStringBuffer("%i", number_), 
						accessory->getName()));
					ChannelManager::showText(context, text);
				}
			}
			else
			{
				int money = accessory->getSellPrice() * number_.getValue();
				tank->getScore().setMoney(tank->getScore().getMoney() + money);
				{
					ChannelText text("combat", 
						LANG_RESOURCE_2("TANK_GET_MONEY",
						"[p:{0}] received {1}", 
						tank->getTargetName(), 
						S3D::formatMoney(money)));
					ChannelManager::showText(context, text);
				}
			}
		}
		else
		{
			int count = tank->getAccessories().getAccessoryCount(accessory);
			if (count > 0)
			{
				int loose = S3D_MIN(count, -number_.getValue());

				tank->getAccessories().rm(accessory, loose);
				{
					ChannelText text("combat", 
						LANG_RESOURCE_3("TANK_LOST_ACCESSORY",
						"[p:{0}] lost {1} * [w:{2}]", 
						tank->getTargetName(),
						S3D::formatStringBuffer("%i", loose), 
						accessory->getName()));
					ChannelManager::showText(context, text);
				}
			}
		}
	}
}
