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

#include <weapons/WeaponAimed.h>
#include <weapons/AccessoryStore.h>
#include <engine/Simulator.h>
#include <engine/ObjectGroups.h>
#include <engine/ObjectGroup.h>
#include <engine/ObjectGroupEntry.h>
#include <landscapemap/LandscapeMaps.h>
#include <tank/TankLib.h>
#include <tanket/Tanket.h>
#include <target/TargetContainer.h>
#include <target/TargetLife.h>
#include <common/Defines.h>
#include <common/OptionsTransient.h>
#include <list>
#include <map>
#include <math.h>

WeaponAimed::WeaponAimed(const char *name, const char *description) :
	Weapon(name, description),
	warHeads_("number of instances of the next primitive to create"),
	aimedWeapon_(),
	groupNames_(),
	randomWhenNoTargets_("If there are no targets in range, just fire random shots (true) or do nothing (false)", 0, true),
	selfHoming_("If there are no targets in range target the firing tank, i.e. consider the firing tank as a target", 0, true),
	maxAimedDistance_("WeaponAimed::maxAimedDistance", "Weapon will aim at targets within this distance"),
	percentageMissChance_("WeaponAimed::percentageMissChance", "Chance a projectile will miss (higher = less accurate)"),
	maxInacuracy_("WeaponAimed::maxInacuracy", "Amount of weapon innaccuracy (higher = less accurate)")
{
	addChildXMLEntry("numberwarheads", &warHeads_);
	addChildXMLEntry("aimedweapon", &aimedWeapon_);
	addChildXMLEntry("randomwhennotargets", &randomWhenNoTargets_);
	addChildXMLEntry("maxaimdistance", &maxAimedDistance_);
	addChildXMLEntry("percentagemiss", &percentageMissChance_);
	addChildXMLEntry("inaccuracy", &maxInacuracy_);
	addChildXMLEntry("selfhoming", &selfHoming_);
	addChildXMLEntry("groupname", &groupNames_);
}

WeaponAimed::~WeaponAimed()
{
}

void WeaponAimed::addWeaponSyncCheck(ScorchedContext &context,
	WeaponFireContext &weaponContext,
	FixedVector &position, FixedVector &velocity)
{
	context.getSimulator().addSyncCheck(S3D::formatStringBuffer("WeaponFire %s-%u-%s %u %s %s",
		getParent()->getName(), getParent()->getAccessoryId(), getAccessoryTypeName(),
		weaponContext.getPlayerId(),
		position.asQuickString(), velocity.asQuickString().c_str()));
}

void WeaponAimed::fireAimedWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, bool invert)
{
	// Get a list of possible destinations
	std::list<FixedVector *> positions;
	if (groupNames_.getChildren().empty())
	{
		std::map<unsigned int, Tanket *> &allTankets =
			context.getTargetContainer().getTankets();
		std::map<unsigned int, Tanket *>::iterator itor;
		for (itor = allTankets.begin();
			itor != allTankets.end();
			++itor)
		{
			Tanket *tanket = itor->second;
			if (tanket->getAlive())
			{
				if (!selfHoming_.getValue() && weaponContext.getPlayerId() == tanket->getPlayerId()) continue;
				positions.push_back(&itor->second->getLife().getTargetPosition());
			}
		}
	}
	else
	{
		std::list<XMLEntryString *>::iterator itor = groupNames_.getChildren().begin(),
			end = groupNames_.getChildren().end();
		for (;itor!=end;++itor)
		{
			const char *groupName = (*itor)->getValue().c_str();
			ObjectGroup *objectGroup = weaponContext.getInternalContext().getLocalGroups().getGroup(groupName);
			if (!objectGroup) objectGroup = context.getObjectGroups().getGroup(groupName);
			if (objectGroup) 
			{
				ObjectGroup::ObjectGroupEntryHolderIterator iterator(objectGroup);
				ObjectGroupEntry *entry;
				while (entry = iterator.getNext())
				{
					positions.push_back(&entry->getPosition());
				}			
			}
		}
	}

	// Iterate 
	fixed totalDist = 0;
	std::list<std::pair<fixed, FixedVector *> > distances;
	{
		fixed maxAimedDistance = maxAimedDistance_.getValue(context);
		std::list<FixedVector *>::iterator itor;
		for (itor = positions.begin();
			itor != positions.end();
			++itor)
		{
			fixed distance = (*(*itor) - position).Magnitude();
			if (distance < maxAimedDistance)
			{
				totalDist += distance;
				distances.push_back(std::pair<fixed, FixedVector *>(distance, *itor));
			}
		}
	}

	// Turn distance into a probablity that we will fire a the tank
	fixed maxDist = 0;
	{
		if (distances.size() <= 1)
		{
			maxDist = totalDist;
		}
		else
		{
			std::list<std::pair<fixed, FixedVector *> >::iterator itor;
			for (itor = distances.begin();
				itor != distances.end();
				++itor)
			{
				maxDist += totalDist - itor->first;
				itor->first = totalDist - itor->first;
			}
		}
	}

	if (context.getOptionsGame().getWeaponSyncCheck())
	{
		std::string buffer("WeaponAimedTargets ");
		std::list<std::pair<fixed, FixedVector *> >::iterator itor;
		for (itor = distances.begin();
			itor != distances.end();
			++itor)
		{
			buffer += S3D::formatStringBuffer("%s %s, ", 
				itor->first.asQuickString().c_str(), 
				itor->second->asQuickString().c_str());
		}
		context.getSimulator().addSyncCheck(buffer);
	}

	RandomGenerator &random = context.getSimulator().getRandomGenerator();
	
	// Add a percetage that we will not fire at any tank
	maxDist *= (percentageMissChance_.getValue(context) / 100) + 1;

	// For each war head
	for (int i=0; i<warHeads_.getValue(); i++)
	{
		// Random probablity
		fixed dist = maxDist * random.getRandFixed("WeaponAimed");

		// Find which tank fits this probability
		FixedVector *shootAt = 0;
		fixed distC = 0;
		std::list<std::pair<fixed, FixedVector *> >::iterator itor;
		for (itor =  distances.begin();
			itor !=  distances.end();
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
		fixed angleXYDegs = random.getRandFixed("WeaponAimed") * 360;
		fixed angleYZDegs = random.getRandFixed("WeaponAimed") * 30 + 50;
		fixed power = random.getRandFixed("WeaponAimed") * 300 + 150;
		if (shootAt)
		{
			if (context.getOptionsGame().getWeaponSyncCheck())
			{
				context.getSimulator().addSyncCheck(S3D::formatStringBuffer("WeaponAimed %s,%s,%s %s",
					angleXYDegs.asQuickString().c_str(), 
					angleYZDegs.asQuickString().c_str(),
					power.asQuickString().c_str(),
					shootAt->asQuickString().c_str()));
			}

			// We have a tank to aim at
			// Aim a shot towards it
			aimShot(
				context,
				random,
				position, 
				*shootAt, 
				angleXYDegs, 
				angleYZDegs, 
				power);

			angleXYDegs += (random.getRandFixed("WeaponAimed") * maxInacuracy_.getValue(context)) - 
				(maxInacuracy_.getValue(context) / 2);
			angleYZDegs += (random.getRandFixed("WeaponAimed") * maxInacuracy_.getValue(context)) - 
				(maxInacuracy_.getValue(context) / 2);
		}
		else if (!randomWhenNoTargets_.getValue())
		{
			continue;
		}
		if (invert) angleYZDegs += 180;

		// Create the shot
		FixedVector velocity;
		TankLib::getVelocityVector(velocity, angleXYDegs, angleYZDegs);
		velocity *= power;

		aimedWeapon_.getValue()->fire(
			context, weaponContext, position, velocity);	
	}
}
