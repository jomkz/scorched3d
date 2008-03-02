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

#include <tankai/TankAICurrentTarget.h>
#include <server/ScorchedServer.h>
#include <tank/TankContainer.h>
#include <tank/TankSort.h>
#include <tank/TankPosition.h>
#include <target/TargetLife.h>
#include <target/TargetShield.h>
#include <weapons/Shield.h>
#include <weapons/Accessory.h>
#include <common/Defines.h>
#include <XML/XMLNode.h>

TankAICurrentTarget::TankAICurrentTarget()
{
}

TankAICurrentTarget::~TankAICurrentTarget()
{
}

bool TankAICurrentTarget::parseConfig(XMLNode *node)
{
	if (!node->getNamedChild("health", health_)) return false;
	if (!node->getNamedChild("random", random_)) return false;
	if (!node->getNamedChild("score", score_)) return false;
	if (!node->getNamedChild("shield", shield_)) return false;
	if (!node->getNamedChild("repeat", repeat_)) return false;
	if (!node->getNamedChild("damagetaken", damagetaken_)) return false;
	if (!node->getNamedChild("damagedone", damagedone_)) return false;
	if (!node->getNamedChild("player", player_)) return false;
	if (!node->getNamedChild("distance", distance_)) return false;
	
	return true;
}

void TankAICurrentTarget::clear()
{
	damageGiven_.clear();
	damageTaken_.clear();
	shotAt_.clear();
}

float TankAICurrentTarget::getTotalDamageTaken()
{
	float damage = 0.0f;
	std::map<Tank *, float>::iterator itor;
	for (itor = damageTaken_.begin();
		itor != damageTaken_.end();
		itor++)
	{
		damage += itor->second;
	}
	return damage;
}

void TankAICurrentTarget::gaveDamage(Tank *tank, float damage)
{
	std::map<Tank *, float>::iterator findItor =
		damageGiven_.find(tank);
	if (findItor == damageGiven_.end())
	{
		damageGiven_[tank] = damage;
	}
	else
	{
		damageGiven_[tank] += damage;
	}
}

void TankAICurrentTarget::tookDamage(Tank *tank, float damage)
{
	std::map<Tank *, float>::iterator findItor =
		damageTaken_.find(tank);
	if (findItor == damageTaken_.end())
	{
		damageTaken_[tank] = damage;
	}
	else
	{
		damageTaken_[tank] += damage;
	}
}

void TankAICurrentTarget::shotAt(Tank *tank)
{
	std::map<Tank *, float>::iterator findItor =
		shotAt_.find(tank);
	if (findItor == shotAt_.end())
	{
		shotAt_[tank] = 1.0f;
	}
	else
	{
		shotAt_[tank] += 1.0f;
	}
}

float TankAICurrentTarget::rankPlayer(std::list<Tank *> &players, Tank *player)
{
	int position = 1;
	std::list<Tank *>::iterator itor;
	for (itor = players.begin();
		itor != players.end();
		itor++, position++)
	{
		Tank *currentTank = *itor;
		if (currentTank == player)
		{
			float score = 1.0f - (float(position * 2) / float(players.size()));
			return score;
		}
	}

	return -1.0f;
}

float TankAICurrentTarget::rankPlayer(std::multimap<float, Tank *> &players, Tank *player)
{
	int position = 1;
	std::multimap<float, Tank *>::iterator itor;
	for (itor = players.begin();
		itor != players.end();
		itor++, position++)
	{
		Tank *currentTank = itor->second;
		if (currentTank == player)
		{
			float score = (float(position * 2) / float(players.size())) - 1.0f;
			return score;
		}
	}

	return -1.0f;
}

void TankAICurrentTarget::getTargets(Tank *thisTank, std::list<Tank *> &resultTargets)
{
	std::list<Tank *> possible;
	std::multimap<float, Tank *> damageTakenSorted;
	std::multimap<float, Tank *> damageGivenSorted;
	std::multimap<float, Tank *> shotAtSorted;
	std::multimap<float, Tank *> distanceSorted;

	// Get the list of tanks we can shoot at
	std::map<unsigned int, Tank *> &tanks = 
		ScorchedServer::instance()->getTankContainer().getAllTanks();
	std::map<unsigned int, Tank *>::iterator posItor;
	for (posItor = tanks.begin();
		posItor != tanks.end();
		posItor++)
	{
		Tank *currentTank = posItor->second;
		if (currentTank == thisTank) continue;
		if (thisTank->getTeam() > 0 && 
			currentTank->getTeam() == thisTank->getTeam()) continue;
		if (!currentTank->getAlive()) continue;

		possible.push_back(currentTank);

		if (damageTaken_.find(currentTank) != damageTaken_.end())
		{
			damageTakenSorted.insert(std::pair<float, Tank *>(
				damageTaken_[currentTank], currentTank));
		}
		if (damageGiven_.find(currentTank) != damageGiven_.end())
		{
			damageGivenSorted.insert(std::pair<float, Tank *>(
				damageGiven_[currentTank], currentTank));
		}
		if (shotAt_.find(currentTank) != shotAt_.end())
		{
			shotAtSorted.insert(std::pair<float, Tank *>(
				shotAt_[currentTank], currentTank));
		}
		distanceSorted.insert(std::pair<float, Tank *>(
			((currentTank->getPosition().getTankPosition() - 
			thisTank->getPosition().getTankPosition()).Magnitude()).asFloat(), currentTank));
	}

	// Sort the tanks in score order
	std::list<Tank *> scoreSorted = possible;
	TankSort::getSortedTanks(scoreSorted, ScorchedServer::instance()->getContext());

	// Go through all possible tanks
	// and weight them according to the weightings
	std::multimap<float, Tank *> weightedTanks;
	std::list<Tank *>::iterator itor;
	for (itor = possible.begin();
		itor != possible.end();
		itor++)
	{
		Tank *currentTank = *itor;

		float healthScore = ((currentTank->getLife().getLife() * 2) 
			/ currentTank->getLife().getMaxLife()).asFloat() - 1.0f;
		float randomScore = RAND * 2.0f - 1.0f;
		float playerScore = ((currentTank->getDestinationId() == 0)?-1.0f:1.0f);
		float shieldScore = ((currentTank->getShield().getShieldPower() * 2).asFloat() 
			/ 100.0f) - 1.0f;
		float scoreScore = rankPlayer(scoreSorted, currentTank);
		float repeatScore = rankPlayer(shotAtSorted, currentTank);
		float damagetakenScore = rankPlayer(damageTakenSorted, currentTank);
		float damagedoneScore = rankPlayer(damageGivenSorted, currentTank);
		float distanceScore = rankPlayer(distanceSorted, currentTank);

		float weight = 
			healthScore * health_ +
			randomScore * random_ +
			playerScore * player_ +
			shieldScore * shield_ +
			scoreScore * score_ +
			repeatScore * repeat_ +
			damagetakenScore * damagetaken_ +
			damagedoneScore * damagedone_ +
			distanceScore * distance_;
		weightedTanks.insert(std::pair<float, Tank *>(weight, currentTank));
	}

	// Form the final list
	std::multimap<float, Tank *>::reverse_iterator weightedItor;
	for (weightedItor = weightedTanks.rbegin();
		weightedItor != weightedTanks.rend();
		weightedItor++)
	{
		resultTargets.push_back(weightedItor->second);
	}
}
