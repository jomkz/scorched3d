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

#include <weapons/WeaponGiveScore.h>
#include <weapons/AccessoryStore.h>
#include <engine/ActionController.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankTeamScore.h>
#include <tank/TankScore.h>
#include <common/Defines.h>
#include <common/ChannelManager.h>
#include <lang/LangResource.h>

REGISTER_ACCESSORY_SOURCE(WeaponGiveScore);

WeaponGiveScore::WeaponGiveScore() :
	WeaponCallback("WeaponGiveScore", 
		"Gives a specified amount of score points to the player. It can also be used to take score away from the player for doing something they shouldn't do. "),
	score_("Amount of points to give to (or take away from) the player")
{
	addChildXMLEntry("score", &score_);
}

WeaponGiveScore::~WeaponGiveScore()
{

}

void WeaponGiveScore::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	context.getActionController().addAction(
		new CallbackWeapon("WeaponGiveScore", this, 0, 0, 
			weaponContext, position, velocity));
}

void WeaponGiveScore::weaponCallback(
	ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity,
	unsigned int userData)
{
	Tank *tank = context.getTargetContainer().getTankById(weaponContext.getPlayerId());
	if (!tank) return;

	int score = score_.getValue();
	tank->getScore().setScore(tank->getScore().getScore() + score_.getValue());
	if (tank->getTeam() > 0)
	{
		context.getTankTeamScore().addScore(score, tank->getTeam());
	}

	{
		if (score > 0)
		{
			ChannelText text("combat", 
				LANG_RESOURCE_2("TANK_GET_SCORE",
				"[p:{0}] received {1} bonus score", 
				tank->getTargetName(), 
				S3D::formatStringBuffer("%i", score)));
			ChannelManager::showText(context, text);
		}
		else
		{
			ChannelText text("combat", 
				LANG_RESOURCE_2("TANK_LOST_SCORE",
				"[p:{0}] lost {1} bonus score", 
				tank->getTargetName(), 
				S3D::formatStringBuffer("%i", -score)));
			ChannelManager::showText(context, text);
		}
	}
}

