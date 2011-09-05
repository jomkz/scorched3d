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

#include <weapons/WeaponAimedOver.h>
#include <weapons/AccessoryStore.h>
#include <engine/Simulator.h>
#include <landscapemap/LandscapeMaps.h>
#include <tank/TankLib.h>
#include <tanket/TanketContainer.h>
#include <target/TargetLife.h>
#include <common/Defines.h>
#include <common/OptionsTransient.h>
#include <list>
#include <math.h>

REGISTER_ACCESSORY_SOURCE(WeaponAimedOver);

WeaponAimedOver::WeaponAimedOver() :
	warHeads_(0),
	aimedWeapon_(0),
	randomWhenNoTargets_(true)
{

}

WeaponAimedOver::~WeaponAimedOver()
{
	delete aimedWeapon_;
	aimedWeapon_ = 0;
}

bool WeaponAimedOver::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(context, accessoryNode)) return false;

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

	// Get the accessory warheads
	if (!accessoryNode->getNamedChild("nowarheads", warHeads_)) return false;
	
	// Get the accessory aimed distance
	if (!accessoryNode->getNamedChild("maxaimdistance", maxAimedDistance_)) return false;
	
	// Get the accessory percentage miss chance
	if (!accessoryNode->getNamedChild("percentagemiss", percentageMissChance_)) return false;

	// Get the accessory percentage miss chance
	if (!accessoryNode->getNamedChild("inaccuracy", maxInacuracy_)) return false;

	accessoryNode->getNamedChild("groupname", groupName_, false);
	accessoryNode->getNamedChild("randomwhennotargets", randomWhenNoTargets_, false);

	return true;
}

void WeaponAimedOver::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &sentPosition, FixedVector &oldvelocity)
{
	FixedVector position = sentPosition;

	// Make sure that this position is above ground
	fixed minHeight = context.getLandscapeMaps().getGroundMaps().getInterpHeight(
		position[0], position[1]);
	if (position[2] < minHeight + fixed(true, 5000))
	{
		position[2] = minHeight + fixed(true, 5000);
	}

	bool ceiling = false;
	{
		// This will return MAX_FLT when there is no roof
		fixed maxHeight = context.getLandscapeMaps().getRoofMaps().getInterpRoofHeight(
			position[0], position[1]);
		if (position[2] > maxHeight - 1)
		{
			ceiling = true;
			position[2] = maxHeight - 1;
		}
	}

	// Get all of the distances of the tanks less than maxAimedDistance_ away
	std::list<std::pair<fixed, Target *> > sortedTanks;
	std::list<Target *> targets;
	if (groupName_.empty())
	{
		std::map<unsigned int, Tanket *> &allTankets =
			context.getTanketContainer().getAllTankets();
		std::map<unsigned int, Tanket *>::iterator itor;
		for (itor = allTankets.begin();
			itor != allTankets.end();
			++itor)
		{
			targets.push_back(itor->second);
		}
	}
	else
	{
		TargetGroupsSetEntry *groupEntry = context.getLandscapeMaps().getGroundMaps().getGroups().
			getGroup(groupName_.c_str());
		if (groupEntry) 
		{
			std::map<unsigned int, TargetGroup *> &objects = groupEntry->getObjects();
			std::map<unsigned int, TargetGroup *>::iterator itor;
			for (itor = objects.begin();
				itor != objects.end();
				++itor)
			{
				targets.push_back(itor->second->getTarget());
			}			
		}
	}
	TankLib::getTargetsSortedByDistance(
		position, 
		targets,
		sortedTanks,
		0,
		maxAimedDistance_.getValue(context));

	// Add all of these distances together
	fixed totalDist = 0;
	std::list<std::pair<fixed, Target *> >::iterator itor;
	for (itor = sortedTanks.begin();
		itor != sortedTanks.end();
		++itor)
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
			++itor)
		{
			(*itor).first = totalDist - (*itor).first;
			maxDist += (*itor).first;
		}
	}

	RandomGenerator &random = context.getSimulator().getRandomGenerator();
	
	// Add a percetage that we will not fire at any tank
	maxDist *= (percentageMissChance_.getValue(context) / 100) + 1;

	// For each war head
	for (int i=0; i<warHeads_; i++)
	{
		// Random probablity
		fixed dist = maxDist * random.getRandFixed("WeaponAimedOver");

		// Find which tank fits this probability
		Target *shootAt = 0;
		fixed distC = 0;
		for (itor = sortedTanks.begin();
			itor != sortedTanks.end();
			++itor)
		{
			distC += (*itor).first;
			if (dist < distC)
			{
				shootAt = (*itor).second;
				break;
			}
		}			

		// Calcuate the angle for the shot
		fixed angleXYDegs = random.getRandFixed("WeaponAimedOver") * 360;
		fixed angleYZDegs = random.getRandFixed("WeaponAimedOver") * 30 + 50;
		fixed power = random.getRandFixed("WeaponAimedOver") * 300 + 150;
		if (shootAt)
		{
			// We have a tank to aim at
			// Aim a shot towards it
			TankLib::getShotTowardsPosition(
				context,
				random,
				position, 
				shootAt->getLife().getTargetPosition(), 
				angleXYDegs, angleYZDegs, power);
			power *= fixed(true, 6000);

			angleXYDegs += (random.getRandFixed("WeaponAimedOver") * maxInacuracy_.getValue(context)) - 
				(maxInacuracy_.getValue(context) / 2);
			angleYZDegs += (random.getRandFixed("WeaponAimedOver") * maxInacuracy_.getValue(context)) - 
				(maxInacuracy_.getValue(context) / 2);
		}
		else if (!randomWhenNoTargets_)
		{
			continue;
		}
		if (ceiling) angleYZDegs += 180;

		// Create the shot
		FixedVector &velocity = TankLib::getVelocityVector(
			angleXYDegs, angleYZDegs);
		velocity *= power;

		aimedWeapon_->fireWeapon(
			context, weaponContext, position, velocity);	
	}
}
