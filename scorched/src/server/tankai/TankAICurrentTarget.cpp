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

#include <tankai/TankAICurrentTarget.h>
#include <server/ScorchedServer.h>
#include <tank/Tank.h>
#include <tank/TankScore.h>
#include <target/TargetContainer.h>
#include <tanket/TanketShotInfo.h>
#include <target/TargetLife.h>
#include <target/TargetShield.h>
#include <weapons/Shield.h>
#include <weapons/Accessory.h>
#include <common/Defines.h>
#include <XML/XMLNode.h>

TankAICurrentTarget::TankAICurrentTarget() :
	XMLEntryContainer("TankAITarget", "Weighting factors use to determine which tank to fire at."
		"Targets are all ranked using these factors. The top weighted targets are selected to be fired at 1st."
		"Each factor is multiplied by the weight and then all factors are added to give the total weighting."
		"All factors are in the range -1.0 to 1.0 before weighting."),
	health_("Health weighting, multiplied by the 100% health the tank has left."),
	random_("Random weighting, used to randomize the chosen tank."),
	score_("Score weighting, multiplied by the current position of the tank ranked by current score"),
	damagedone_("Damage done weighting, multiplied by the current position of the tank ranked by damage done to the ai"),
	damagetaken_("Damage taken weighting, multiplied by the current position of the tank ranked by damage taken by the ai"),
	shield_("Shield weighting, multiplied by the current position of the tank ranked by the level of the current shield"),
	repeat_("Repeat weighting, multiplied by the number of times this tank has been chosen as a target"),
	distance_("Distance weighting, multiplied by the closeness of the tank (closer is higher)"),
	player_("Player weighting, multiplied by the human/ai type of the tank (Human:-1, AI:1)")
{
	addChildXMLEntry(
		"health", &health_, 
		"random", &random_,
		"score", &score_,
		"damagedone", &damagedone_,
		"damagetaken", &damagetaken_,
		"shield", &shield_,
		"repeat", &repeat_,
		"distance", &distance_,
		"player", &player_);
}

TankAICurrentTarget::~TankAICurrentTarget()
{
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
	std::map<Tanket *, float>::iterator itor;
	for (itor = damageTaken_.begin();
		itor != damageTaken_.end();
		++itor)
	{
		damage += itor->second;
	}
	return damage;
}

void TankAICurrentTarget::gaveDamage(Tanket *tanket, float damage)
{
	std::map<Tanket *, float>::iterator findItor =
		damageGiven_.find(tanket);
	if (findItor == damageGiven_.end())
	{
		damageGiven_[tanket] = damage;
	}
	else
	{
		damageGiven_[tanket] += damage;
	}
}

void TankAICurrentTarget::tookDamage(Tanket *tanket, float damage)
{
	std::map<Tanket *, float>::iterator findItor =
		damageTaken_.find(tanket);
	if (findItor == damageTaken_.end())
	{
		damageTaken_[tanket] = damage;
	}
	else
	{
		damageTaken_[tanket] += damage;
	}
}

void TankAICurrentTarget::shotAt(Tanket *tanket)
{
	std::map<Tanket *, float>::iterator findItor =
		shotAt_.find(tanket);
	if (findItor == shotAt_.end())
	{
		shotAt_[tanket] = 1.0f;
	}
	else
	{
		shotAt_[tanket] += 1.0f;
	}
}

float TankAICurrentTarget::rankPlayer(std::list<Tanket *> &players, Tanket *player)
{
	int position = 1;
	std::list<Tanket *>::iterator itor;
	for (itor = players.begin();
		itor != players.end();
		++itor, position++)
	{
		Tanket *currentTank = *itor;
		if (currentTank == player)
		{
			float score = 1.0f - (float(position * 2) / float(players.size()));
			return score;
		}
	}

	return -1.0f;
}

float TankAICurrentTarget::rankPlayer(std::multimap<float, Tanket *> &players, Tanket *player)
{
	int position = 1;
	std::multimap<float, Tanket *>::iterator itor;
	for (itor = players.begin();
		itor != players.end();
		++itor, position++)
	{
		Tanket *currentTank = itor->second;
		if (currentTank == player)
		{
			float score = (float(position * 2) / float(players.size())) - 1.0f;
			return score;
		}
	}

	return -1.0f;
}

void TankAICurrentTarget::getTargets(Tanket *thisTanket, std::list<Tanket *> &resultTargets)
{
	std::list<Tanket *> possible;
	std::multimap<float, Tanket *> damageTakenSorted;
	std::multimap<float, Tanket *> damageGivenSorted;
	std::multimap<float, Tanket *> shotAtSorted;
	std::multimap<float, Tanket *> distanceSorted;
	std::multimap<float, Tanket *> scoreSorted;

	// Get the list of tanks we can shoot at
	std::map<unsigned int, Tanket *> &tanks = 
		ScorchedServer::instance()->getTargetContainer().getTankets();
	std::map<unsigned int, Tanket *>::iterator posItor;
	for (posItor = tanks.begin();
		posItor != tanks.end();
		++posItor)
	{
		Tanket *currentTanket = posItor->second;
		if (currentTanket == thisTanket) continue;
		if (thisTanket->getTeam() > 0 && 
			currentTanket->getTeam() == thisTanket->getTeam()) continue;
		if (!currentTanket->getAlive()) continue;

		possible.push_back(currentTanket);

		if (currentTanket->getType() == Target::TypeTank) {
			Tank *currentTank = (Tank *) currentTanket;
			scoreSorted.insert(std::pair<float, Tanket *>((float) currentTank->getScore().getScore(), currentTank));
		}

		if (damageTaken_.find(currentTanket) != damageTaken_.end())
		{
			damageTakenSorted.insert(std::pair<float, Tanket *>(
				damageTaken_[currentTanket], currentTanket));
		}
		if (damageGiven_.find(currentTanket) != damageGiven_.end())
		{
			damageGivenSorted.insert(std::pair<float, Tanket *>(
				damageGiven_[currentTanket], currentTanket));
		}
		if (shotAt_.find(currentTanket) != shotAt_.end())
		{
			shotAtSorted.insert(std::pair<float, Tanket *>(
				shotAt_[currentTanket], currentTanket));
		}
		distanceSorted.insert(std::pair<float, Tanket *>(
			((currentTanket->getLife().getTargetPosition() - 
			thisTanket->getLife().getTargetPosition()).Magnitude()).asFloat(), currentTanket));
	}

	// Go through all possible tanks
	// and weight them according to the weightings
	std::multimap<float, Tanket *> weightedTanks;
	std::list<Tanket *>::iterator itor;
	for (itor = possible.begin();
		itor != possible.end();
		++itor)
	{
		Tanket *currentTank = *itor;

		float healthScore = ((currentTank->getLife().getLife() * 2) 
			/ currentTank->getLife().getMaxLife()).asFloat() - 1.0f;
		float randomScore = S3D_RAND * 2.0f - 1.0f;
		float playerScore = ((currentTank->getTankAI() != 0)?-1.0f:1.0f);
		float shieldScore = ((currentTank->getShield().getShieldPower() * 2).asFloat() 
			/ 100.0f) - 1.0f;
		float scoreScore = rankPlayer(scoreSorted, currentTank);
		float repeatScore = rankPlayer(shotAtSorted, currentTank);
		float damagetakenScore = rankPlayer(damageTakenSorted, currentTank);
		float damagedoneScore = rankPlayer(damageGivenSorted, currentTank);
		float distanceScore = rankPlayer(distanceSorted, currentTank);

		float weight = 
			healthScore * health_.getValue().asFloat() +
			randomScore * random_.getValue().asFloat() +
			playerScore * player_.getValue().asFloat() +
			shieldScore * shield_.getValue().asFloat() +
			scoreScore * score_.getValue().asFloat() +
			repeatScore * repeat_.getValue().asFloat() +
			damagetakenScore * damagetaken_.getValue().asFloat() +
			damagedoneScore * damagedone_.getValue().asFloat() +
			distanceScore * distance_.getValue().asFloat();
		weightedTanks.insert(std::pair<float, Tanket *>(weight, currentTank));
	}

	// Form the final list
	std::multimap<float, Tanket *>::reverse_iterator weightedItor;
	for (weightedItor = weightedTanks.rbegin();
		weightedItor != weightedTanks.rend();
		++weightedItor)
	{
		resultTargets.push_back(weightedItor->second);
	}
}
