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

#include <weapons/WeaponAimedUnder.h>
#include <weapons/AccessoryStore.h>
#include <engine/Simulator.h>
#include <landscapemap/LandscapeMaps.h>
#include <tank/TankLib.h>
#include <tank/Tank.h>
#include <tank/TankPosition.h>
#include <common/Defines.h>
#include <list>
#include <math.h>

REGISTER_ACCESSORY_SOURCE(WeaponAimedUnder);

WeaponAimedUnder::WeaponAimedUnder() : 
	warHeads_(0), moveUnderground_(true),
	aimedWeapon_(0)
{

}

WeaponAimedUnder::~WeaponAimedUnder()
{
	delete aimedWeapon_;
	aimedWeapon_ = 0;
}

bool WeaponAimedUnder::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(context, accessoryNode)) return false;

	// Get the accessory size
	if (!accessoryNode->getNamedChild("nowarheads", warHeads_)) return false;

	// Get the next weapon
	XMLNode *subNode = 0;
	if (!accessoryNode->getNamedChild("aimedweapon", subNode)) return false;

	// Check next weapon is correct type
	AccessoryPart *accessory = context.getAccessoryStore().
		createAccessoryPart(context, parent_, subNode);
	if (!accessory || accessory->getType() != AccessoryPart::AccessoryWeapon)
	{
		return subNode->returnError("Failed to find sub weapon, not a weapon");
	}
	aimedWeapon_ = (Weapon*) accessory;
	
	// Get the accessory aimed distance
	if (!accessoryNode->getNamedChild("maxaimdistance", maxAimedDistance_)) return false;
	
	// Get the accessory percentage miss chance
	if (!accessoryNode->getNamedChild("percentagemiss", percentageMissChance_)) return false;

	// Get the accessory percentage miss chance
	if (!accessoryNode->getNamedChild("inaccuracy", maxInacuracy_)) return false;

	// Get optional moveunderground attribute
	accessoryNode->getNamedChild("moveunderground", moveUnderground_, false);

	return true;
}

void WeaponAimedUnder::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &oldvelocity)
{
	// NOTE: This code is very similar to the funky bomb code
	// except it works under ground
	if (moveUnderground_)
	{
		fixed height = context.getLandscapeMaps().getGroundMaps().
			getInterpHeight(position[0], position[1]);
		if (position[2] < height + 1)
		{
			position[2] = context.getLandscapeMaps().getGroundMaps().
				getInterpHeight(position[0], position[1]) / 2;
		}
	}

	// Get all of the distances of the tanks less than 50 away
	std::list<std::pair<fixed, Tank *> > sortedTanks;
	TankLib::getTanksSortedByDistance(
		context,
		position, 
		sortedTanks,
		0,
		maxAimedDistance_.getValue(context));

	// Add all of these distances together
	fixed totalDist = 0;
	std::list<std::pair<fixed, Tank *> >::iterator itor;
	for (itor = sortedTanks.begin();
		itor != sortedTanks.end();
		itor++)
	{
		totalDist += (*itor).first;
	}

	// Turn distance into a probablity that we will fire a the tank
	fixed maxDist = 0;
	if (sortedTanks.size() == 1)
	{
		maxDist = totalDist;
	}
	else
	{
		for (itor = sortedTanks.begin();
			itor != sortedTanks.end();
			itor++)
		{
			(*itor).first = totalDist - (*itor).first;
			maxDist += (*itor).first;
		}
	}
	
	// Add a percetage that we will not fire at any tank
	maxDist *= (percentageMissChance_.getValue(context)/ 100) + 1;

	RandomGenerator &random = context.getSimulator().getRandomGenerator();

	// For each war head
	for (int i=0; i<warHeads_; i++)
	{
		// Random probablity
		fixed dist = maxDist * random.getRandFixed("WeaponAimedUnder");

		// Find which tank fits this probability
		Tank *shootAt = 0;
		fixed distC = 0;
		for (itor = sortedTanks.begin();
			itor != sortedTanks.end();
			itor++)
		{
			distC += (*itor).first;
			if (dist < distC)
			{
				shootAt = (*itor).second;
				break;
			}
		}			

		// Calcuate the angle for the shot
		fixed angleXYDegs = random.getRandFixed("WeaponAimedUnder") * 360;
		fixed angleYZDegs = random.getRandFixed("WeaponAimedUnder") * 30 + 50;
		fixed power = 1000;
		if (shootAt)
		{
			// We have a tank to aim at
			// Aim a shot towards it
			TankLib::getSniperShotTowardsPosition(
				context,
				position, 
				shootAt->getPosition().getTankPosition(), -1, 
				angleXYDegs, angleYZDegs, power);

			angleXYDegs += (random.getRandFixed("WeaponAimedUnder") * maxInacuracy_.getValue(context)) - 
				(maxInacuracy_.getValue(context) / 2);
			angleYZDegs += (random.getRandFixed("WeaponAimedUnder") * maxInacuracy_.getValue(context)) - 
				(maxInacuracy_.getValue(context) / 2);
		}

		// Create the shot
		FixedVector &velocity = TankLib::getVelocityVector(
			angleXYDegs, angleYZDegs);
		velocity *= power;

		aimedWeapon_->fireWeapon(context, weaponContext, position, velocity);
	}
}
