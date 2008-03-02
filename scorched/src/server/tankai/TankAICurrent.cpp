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

#include <tankai/TankAICurrent.h>
#include <tank/TankContainer.h>
#include <server/ScorchedServer.h>
#include <common/OptionsTransient.h>
#include <XML/XMLNode.h>

TankAICurrent::TankAICurrent() : tank_(0)
{
}

TankAICurrent::~TankAICurrent()
{
}

TankAI *TankAICurrent::createCopy(Tank *tank)
{
	TankAICurrent *result = new TankAICurrent(*this);
	result->setTank(tank);
	return result;
}

void TankAICurrent::setTank(Tank *tank)
{
	tank_ = tank;
}

bool TankAICurrent::parseConfig(XMLNode *node)
{
	if (!TankAI::parseConfig(node)) return false;

	{
		XMLNode *weapons = 0;
		if (!node->getNamedChild("weapons", weapons)) return false;
		if (!wantedWeapons_.parseConfig(weapons)) return false;
	}
	{
		XMLNode *defense = 0;
		if (!node->getNamedChild("defense", defense)) return false;
		if (!defenses_.parseConfig(defense)) return false;
	}
	{
		XMLNode *attack = 0;
		if (!node->getNamedChild("attack", attack)) return false;
		if (!move_.parseConfig(attack)) return false;
	}

	return node->failChildren();	
}

void TankAICurrent::newMatch()
{
	newGame();
}

void TankAICurrent::newGame()
{
	move_.getTargets().clear();
	move_.clear();
}

void TankAICurrent::playMove()
{
	// Raise any defenses
	defenses_.raiseDefenses(tank_);

	// Make the move
	move_.playMove(tank_, 
		wantedWeapons_.getCurrentWeaponSet(),
		defenses_.getUseBatteries());
}

void TankAICurrent::autoDefense()
{
	defenses_.raiseDefenses(tank_);
}

void TankAICurrent::buyAccessories()
{
	bool lastRound = 
		(ScorchedServer::instance()->getOptionsTransient().getCurrentRoundNo() >=
		ScorchedServer::instance()->getOptionsGame().getNoRounds());

	wantedWeapons_.buyWeapons(tank_, lastRound);
}

void TankAICurrent::tankHurt(Weapon *weapon, float damage, 
	unsigned int damaged, unsigned int fired)
{
	if (damaged == tank_->getPlayerId())
	{
		Tank *firedTank = ScorchedServer::instance()->
			getTankContainer().getTankById(fired);
		if (firedTank)
		{
			move_.getTargets().tookDamage(firedTank, damage);		
		}
	}
	else if (fired == tank_->getPlayerId())
	{
		Tank *damagedTank = ScorchedServer::instance()->
			getTankContainer().getTankById(damaged);
		if (damagedTank)
		{
			move_.getTargets().gaveDamage(damagedTank, damage);		
		}
	}
}

void TankAICurrent::shotLanded(ScorchedCollisionId collision,
		Weapon *weapon, unsigned int firer, 
		Vector &position)
{
}
