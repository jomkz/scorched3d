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

#include <weapons/WeaponRoller.h>
#include <weapons/AccessoryStore.h>
#include <weapons/Shield.h>
#include <actions/ShotBounce.h>
#include <common/Defines.h>
#include <engine/ActionController.h>
#include <engine/Simulator.h>
#include <target/Target.h>
#include <target/TargetContainer.h>
#include <target/TargetShield.h>
#include <target/TargetLife.h>
#include <landscapemap/LandscapeMaps.h>
#include <math.h>

REGISTER_ACCESSORY_SOURCE(WeaponRoller);

WeaponRoller::WeaponRoller() : 
	Weapon("WeaponRoller", "Creates a specific number of rolling warheads which roll downhill."
		"When the rollers time out or collide with something they trigger a collisionaction."),
	shieldHurtFactorExp_("WeaponRoller::shieldHurtFactorExp", "Amount of damage the roller does during shield collisions.  "
		"1 = 100%, 2 = 200%, 0 = no damage, etc", 0, "0"), 
	dampenVelocityExp_("WeaponRoller::dampenVelocityExp", "If maintainvelocity=true, multiplies the velocity by this factor to control the amount of velocity retained by the rollers."
		"0 = none, 1.0 = full", 0, "1"), 
	timeExp_("WeaponRoller::timeExp", "Number of seconds the rollers will roll before exploding"),
	timeoutExp_("WeaponRoller::timeoutExp", "The amount of time until the weapon duds (0 is no dud)", 0, "0"),
	numberRollers_("WeaponRoller::numberRollers", "Number of rollers to create"),
	maintainVelocity_("If true, the roller will keep its momentum when it spawns", 0, false), 
	roll_("Whether or not the roller model will roll", 0, true),
	stepSize_("How long should the update between each roll be", 0, fixed(true, 100)),
	noCameraTrack_("Consider this projectile as an action camera target", 0, false)
{
	addChildXMLEntry("time", &timeExp_);
	addChildXMLEntry("timeout", &timeoutExp_);
	addChildXMLEntry("numberrollers", &numberRollers_);
	addChildXMLEntry("collisionaction", &collisionAction_);
	addChildXMLEntry("rollermodel", &rollerModelId_);
	addChildXMLEntry("shieldhurtfactor", &shieldHurtFactorExp_);
	addChildXMLEntry("dampenvelocity", &dampenVelocityExp_);
	addChildXMLEntry("maintainvelocity", &maintainVelocity_);
	addChildXMLEntry("roll", &roll_);
	addChildXMLEntry("stepsize", &stepSize_);
	addChildXMLEntry("nocameratrack", &noCameraTrack_);
	addChildXMLEntry("localgroupname", &localGroups_);
	addChildXMLEntry("globalgroupname", &globalGroups_);
	particleDefinition_.addAllEntries(*this);
}

WeaponRoller::~WeaponRoller()
{
}

void WeaponRoller::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &oldposition, FixedVector &velocity)
{
	fixed minHeight = context.getLandscapeMaps().getGroundMaps().getInterpHeight(
		oldposition[0], oldposition[1]);

	// Make sure position is not underground
	if (oldposition[2] < minHeight)
	{
		if (minHeight - oldposition[2] > 10) // Give room for shields as well
		{
			return;
		}
	}

	RandomGenerator &random = context.getSimulator().getRandomGenerator();
	int numberRollers = numberRollers_.getValue(context).asInt();
	for (int i=0; i<numberRollers; i++)
	{
		FixedVector position = oldposition;
		position[2] += fixed(true, 1500);
		
		// Make a slightly different starting position
		position[0] += random.getRandFixed("WeaponRoller") * 2 - 1;
		position[1] += random.getRandFixed("WeaponRoller") * 2 - 1;
		fixed minHeight = context.getLandscapeMaps().getGroundMaps().getInterpHeight(
			position[0], position[1]) + 1;
		if (position[2] < minHeight) position[2] = minHeight;
				
		// Check if we have hit the roof (quite litteraly)
		{
			fixed maxHeight = context.getLandscapeMaps().getRoofMaps().getInterpRoofHeight(
				position[0], position[1]);
			if (position[2] > maxHeight - 1)
			{
				position[2] = maxHeight - 1;
			}
		}

		// Make sure this new position is not inside a tank's shields
		bool ok = false;
		while (!ok)
		{
			ok = true;
			std::map<unsigned int, Target *> &targets = 
				context.getTargetContainer().getTargets();
			std::map<unsigned int, Target *>::iterator itor;
			for (itor = targets.begin();
				itor != targets.end();
				++itor)
			{
				Target *current = (*itor).second;
				FixedVector &tankPos = 
					current->getLife().getTargetPosition();
				Accessory *accessory = 
					current->getShield().getCurrentShield();
				if (accessory)
				{
					Shield *shield = (Shield *) accessory->getAction();
					FixedVector offset = position - tankPos;
					if (shield->inShield(offset))
					{
						ok = false;
						position[2] += 1;
					}
				}
			}
		}
		
		addRoller(context, weaponContext, position, velocity);
	}
}

void WeaponRoller::addRoller(ScorchedContext &context,
	WeaponFireContext &weaponContext,
	FixedVector &position, FixedVector &velocity)
{
	RandomGenerator &random = context.getSimulator().getRandomGenerator();

	FixedVector newVelocity;
	if (maintainVelocity_.getValue())
	{
		newVelocity = velocity * dampenVelocityExp_.getValue(context);
	}
	else
	{
		newVelocity[0] = random.getRandFixed("WeaponRoller") - fixed(true, 5000);
		newVelocity[1] = random.getRandFixed("WeaponRoller") - fixed(true, 5000);
		newVelocity[2] = random.getRandFixed("WeaponRoller") * 2;
	}
	
	context.getActionController().addAction(
		new ShotBounce(this, position, newVelocity, weaponContext));
}
