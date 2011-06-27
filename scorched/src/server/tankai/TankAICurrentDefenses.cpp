////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011
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

#include <tankai/TankAICurrentDefenses.h>
#include <tank/Tank.h>
#include <tank/TankAccessories.h>
#include <weapons/Accessory.h>
#include <target/TargetLife.h>
#include <target/TargetShield.h>
#include <target/TargetParachute.h>
#include <coms/ComsDefenseMessage.h>
#include <simactions/TankDefenseSimAction.h>
#include <server/ScorchedServer.h>
#include <server/ServerSimulator.h>
#include <XML/XMLNode.h>
#include <stdlib.h>

TankAICurrentDefenses::TankAICurrentDefenses()
{
}


TankAICurrentDefenses::~TankAICurrentDefenses()
{
}

bool TankAICurrentDefenses::parseConfig(XMLNode *node)
{
	if (!node->getNamedChild("useparachutes", useParachutes_)) return false;
	if (!node->getNamedChild("useshields", useShields_)) return false;
	if (!node->getNamedChild("usebatteries", useBatteries_)) return false;
	return node->failChildren();	
}

void TankAICurrentDefenses::selectFirstShield(Tank *tank)
{
	if (tank->getShield().getCurrentShield()) return;

	std::list<Accessory *> &shields =
		tank->getAccessories().getAllAccessoriesByType(
			AccessoryPart::AccessoryShield);
	if (!shields.empty())
	{
		Accessory *shield = shields.front();
		shieldsUpDown(tank, shield->getAccessoryId());
	}
}

void TankAICurrentDefenses::selectFirstParachute(Tank *tank)
{
	if (tank->getParachute().getCurrentParachute()) return;

	std::list<Accessory *> &parachutes =
		tank->getAccessories().getAllAccessoriesByType(
			AccessoryPart::AccessoryParachute);
	if (!parachutes.empty())
	{
		Accessory *parachute = parachutes.front();
		parachutesUpDown(tank, parachute->getAccessoryId());
	}
}

void TankAICurrentDefenses::raiseDefenses(Tank *tank)
{
	// Try to enable parachutes (fails if we don't have any)
	if (useParachutes_) selectFirstParachute(tank);

	// Try to raise shields (fails if we don't have any)
	if (useShields_) selectFirstShield(tank);
}

void TankAICurrentDefenses::parachutesUpDown(Tank *tank, unsigned int paraId)
{
	ComsDefenseMessage defenseMessage(
		tank->getPlayerId(),
		(paraId!=0)?ComsDefenseMessage::eParachutesUp:ComsDefenseMessage::eParachutesDown,
		paraId);

	TankDefenseSimAction *simAction = new TankDefenseSimAction(defenseMessage);
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(simAction);
}

void TankAICurrentDefenses::shieldsUpDown(Tank *tank, unsigned int shieldId)
{
	ComsDefenseMessage defenseMessage(
		tank->getPlayerId(),
		(shieldId!=0)?ComsDefenseMessage::eShieldUp:ComsDefenseMessage::eShieldDown,
		shieldId);

	TankDefenseSimAction *simAction = new TankDefenseSimAction(defenseMessage);
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(simAction);
}
