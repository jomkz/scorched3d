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

#include <weapons/WeaponGiveMoney.h>
#include <weapons/AccessoryStore.h>
#include <engine/ActionController.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankScore.h>
#include <common/Defines.h>
#include <common/ChannelManager.h>
#include <lang/LangResource.h>

REGISTER_ACCESSORY_SOURCE(WeaponGiveMoney);

WeaponGiveMoney::WeaponGiveMoney() :
	WeaponCallback("WeaponGiveMoney", 
		"Gives a specified amount of money to the player, can also be used to reduce the amount of money a player has."),
	money_("WeaponGiveMoney::money", "Amount of money to give to the player")
{
	addChildXMLEntry("money", &money_);
}

WeaponGiveMoney::~WeaponGiveMoney()
{

}

void WeaponGiveMoney::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	context.getActionController().addAction(
		new CallbackWeapon("WeaponGiveMoney", this, 0, 0, 
			weaponContext, position, velocity));
}

void WeaponGiveMoney::weaponCallback(
	ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity,
	unsigned int userData)
{
	Tank *tank = context.getTargetContainer().getTankById(weaponContext.getPlayerId());
	if (!tank) return;

	int money = money_.getValue(context).asInt();

	tank->getScore().setMoney(tank->getScore().getMoney() + money);

	{
		if (money > 0)
		{
			ChannelText text("combat", 
				LANG_RESOURCE_2("TANK_GET_MONEY",
				"[p:{0}] received {1}", 
				tank->getTargetName(), 
				S3D::formatMoney(money)));
			ChannelManager::showText(context, text);
		}
		else
		{
			ChannelText text("combat", 
				LANG_RESOURCE_2("TANK_LOST_MONEY",
				"[p:{0}] lost {1}", 
				tank->getTargetName(), 
				S3D::formatMoney(-money)));
			ChannelManager::showText(context, text);
		}
	}
}

