////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2009
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

#include <tankai/TankAICurrentMoveWeapons.h>
#include <tank/Tank.h>
#include <tank/TankPosition.h>
#include <target/TargetShield.h>
#include <target/TargetParachute.h>
#include <server/ScorchedServer.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapemap/LandscapeMaps.h>
#include <weapons/Accessory.h>
#include <weapons/WeaponNapalm.h>
#include <weapons/Shield.h>

TankAICurrentMoveWeapons::TankAICurrentMoveWeapons(Tank *tank, 
	Tank *targetTank,
	TankAIWeaponSets::WeaponSet *weapons)
{
	// Check if this target has shields
	shield = 0;
	if (targetTank->getShield().getCurrentShield())
	{
		shield = (Shield *) targetTank->getShield().
			getCurrentShield()->getAction();
	}

	// Normal weapons
	small = weapons->getTankAccessoryByType(tank, "explosionsmall");
	large = weapons->getTankAccessoryByType(tank, "explosionlarge");
	roller = weapons->getTankAccessoryByType(tank, "roller");
	uncover = weapons->getTankAccessoryByType(tank, "uncover");

	// Check for being under water
	fixed waterHeight = -10;
	LandscapeTex &tex = *ScorchedServer::instance()->getLandscapeMaps().getDefinitions().getTex();
	if (tex.border->getType() == LandscapeTexType::eWater)
	{
		LandscapeTexBorderWater *water = 
			(LandscapeTexBorderWater *) tex.border;

		waterHeight = water->height;
	}
	napalm = 0;
	if (targetTank->getPosition().getTankPosition()[2] > waterHeight)
	{
		napalm = weapons->getTankAccessoryByType(tank, "napalm");
	}

	// Check for laser proof shields
	laser = 0;
	if (!shield ||
		shield->getLaserProof() == Shield::ShieldLaserProofNone)
	{
		laser = weapons->getTankAccessoryByType(tank, "laser");
	}

	// Check for having parachutes
	// Cheating!!, need a better way for this
	digger = 0;
	if (!targetTank->getParachute().getCurrentParachute())
	{
		digger = weapons->getTankAccessoryByType(tank, "digger");
	}
}

TankAICurrentMoveWeapons::~TankAICurrentMoveWeapons()
{
}

