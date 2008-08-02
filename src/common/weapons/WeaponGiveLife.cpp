////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
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

#include <weapons/WeaponGiveLife.h>
#include <weapons/AccessoryStore.h>
#include <engine/ActionController.h>
#include <tank/TankContainer.h>
#include <target/TargetLife.h>
#include <common/Defines.h>
#include <common/ChannelManager.h>

REGISTER_ACCESSORY_SOURCE(WeaponGiveLife);

WeaponGiveLife::WeaponGiveLife()
{

}

WeaponGiveLife::~WeaponGiveLife()
{

}

bool WeaponGiveLife::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(context, accessoryNode)) return false;

	if (!accessoryNode->getNamedChild("life", life_)) return false;
	if (!accessoryNode->getNamedChild("exceedmax", exceedMax_)) return false;

	return true;
}

void WeaponGiveLife::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	context.getActionController().addAction(
		new CallbackWeapon("WeaponGiveLife", this, 0, 0, 
			weaponContext, position, velocity));
}

void WeaponGiveLife::weaponCallback(
	ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity,
	unsigned int userData)
{
	Tank *tank = context.getTankContainer().getTankById(weaponContext.getPlayerId());
	if (!tank) return;

	fixed life = life_.getValue(context);
	if (life > 0)
	{
		if (life > tank->getLife().getMaxLife() &&
			exceedMax_)
		{
			tank->getLife().setMaxLife(life);
		}
		tank->getLife().setLife(
			tank->getLife().getLife() + life);

		{
			ChannelText text("combat", 
				S3D::formatStringBuffer("[p:%s] received %.0f life", 
				tank->getName(), life.asFloat()));
			//info.setPlayerId(weaponContext.getPlayerId());
			ChannelManager::showText(context, text);
		}
	}
	else
	{
		if (tank->getLife().getLife() + life <= 0)
		{
			tank->getLife().setLife(
				MIN(fixed(1), tank->getLife().getLife()));
		}
		else
		{
			tank->getLife().setLife(
				tank->getLife().getLife() + life);
		}

		{
			ChannelText text("combat", 
				S3D::formatStringBuffer("[p:%s] lost %.0f life", 
				tank->getName(), -life.asFloat()));
			//info.setPlayerId(weaponContext.getPlayerId());
			ChannelManager::showText(context, text);
		}
	}
}

