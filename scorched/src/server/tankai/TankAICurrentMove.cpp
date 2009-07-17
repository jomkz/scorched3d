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

#include <tankai/TankAICurrentMove.h>
#include <tankai/TankAIAimGuesser.h>
#include <tankai/TankAISniperGuesser.h>
#include <coms/ComsPlayedMoveMessage.h>
#include <coms/ComsDefenseMessage.h>
#include <common/Logger.h>
#include <server/ServerShotHolder.h>
#include <server/ServerDefenseHandler.h>
#include <server/ScorchedServer.h>
#include <tank/Tank.h>
#include <tank/TankLib.h>
#include <tank/TankPosition.h>
#include <tank/TankAccessories.h>
#include <target/TargetLife.h>
#include <target/TargetShield.h>
#include <target/TargetParachute.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapemap/MovementMap.h>
#include <landscapemap/GroundMaps.h>
#include <weapons/AccessoryStore.h>
#include <weapons/Shield.h>
#include <weapons/WeaponMoveTank.h>
#include <XML/XMLNode.h>

TankAICurrentMove::TankAICurrentMove() : 
	useResign_(true), useFuel_(true), 
	totalDamageBeforeMove_(0.0f),
	movementDamage_(300.0f), movementDamageChance_(0.3f), movementLife_(75.0f),
	movementRandom_(10.0f), movementCloseness_(15.0f),
	groupShotSize_(2), groupShotChance_(0.7f), groupTargetDistance_(25.0f),
	resignLife_(10.0f), // Min life before resigning
	largeWeaponUseDistance_(10.0f), // The distance under which large weapons will be used
	sniperUseDistance_(80.0f), // The max distance to allow sniper shots
	sniperStartDistance_(10.0f), sniperEndDistance_(0.0f),
	sniperMinDecrement_(2.0f), sniperMaxDecrement_(5.0f),
	sniperMovementFactor_(4.0f),
	projectileStartDistance_(10.0f), projectileEndDistance_(5.0f),
	projectileMinDecrement_(1.0f), projectileMaxDecrement_(4.0f),
	projectileMovementFactor_(10.0f), projectileMinDistance_(10.0f)
{
}

TankAICurrentMove::~TankAICurrentMove()
{
}

bool TankAICurrentMove::parseConfig(XMLNode *node)
{
	{
		XMLNode *targets = 0;
		if (!node->getNamedChild("targets", targets)) return false;
		if (!targets_.parseConfig(targets)) return false;
	}
	{
		XMLNode *resign = 0;
		if (!node->getNamedChild("resign", resign)) return false;
		if (!resign->getNamedChild("useresign", useResign_)) return false;
		if (!resign->getNamedChild("resignlife", resignLife_)) return false;
		if (!resign->failChildren()) return false;
	}
	{
		XMLNode *movement = 0;
		if (!node->getNamedChild("movement", movement)) return false;
		if (!movement->getNamedChild("usefuel", useFuel_)) return false;
		if (!movement->getNamedChild("movementdamage", movementDamage_)) return false;
		if (!movement->getNamedChild("movementdamagechance", movementDamageChance_)) return false;
		if (!movement->getNamedChild("movementlife", movementLife_)) return false;
		if (!movement->getNamedChild("movementrandom", movementRandom_)) return false;
		if (!movement->getNamedChild("movementcloseness", movementCloseness_)) return false;
		if (!movement->failChildren()) return false;
	}
	{
		XMLNode *groupshot = 0;
		if (!node->getNamedChild("groupshot", groupshot)) return false;
		if (!groupshot->getNamedChild("groupshotsize", groupShotSize_)) return false;
		if (!groupshot->getNamedChild("groupshotchance", groupShotChance_)) return false;
		if (!groupshot->getNamedChild("grouptargetdistance", groupTargetDistance_)) return false;
		if (!groupshot->failChildren()) return false;
	}
	{
		XMLNode *sniper = 0;
		if (!node->getNamedChild("sniper", sniper)) return false;
		if (!sniper->getNamedChild("snipermovementfactor", sniperMovementFactor_)) return false;
		if (!sniper->getNamedChild("sniperusedistance", sniperUseDistance_)) return false;
		if (!sniper->getNamedChild("sniperstartdistance", sniperStartDistance_)) return false;
		if (!sniper->getNamedChild("sniperenddistance", sniperEndDistance_)) return false;
		if (!sniper->getNamedChild("snipermindecrement", sniperMinDecrement_)) return false;
		if (!sniper->getNamedChild("snipermaxdecrement", sniperMaxDecrement_)) return false;
		if (!sniper->failChildren()) return false;
	}
	{
		XMLNode *projectile = 0;
		if (!node->getNamedChild("projectile", projectile)) return false;
		if (!projectile->getNamedChild("projectilemindistance", projectileMinDistance_)) return false;
		if (!projectile->getNamedChild("projectilemovementfactor", projectileMovementFactor_)) return false;
		if (!projectile->getNamedChild("projectilestartdistance", projectileStartDistance_)) return false;
		if (!projectile->getNamedChild("projectileenddistance", projectileEndDistance_)) return false;
		if (!projectile->getNamedChild("projectilemindecrement", projectileMinDecrement_)) return false;
		if (!projectile->getNamedChild("projectilemaxdecrement", projectileMaxDecrement_)) return false;
		if (!projectile->getNamedChild("largeweaponusedistance", largeWeaponUseDistance_)) return false;
		if (!projectile->failChildren()) return false;
	}

	return node->failChildren();
}

void TankAICurrentMove::clear()
{
	totalDamageBeforeMove_ = 0.0f;
	shotRecords_.clear();
}

void TankAICurrentMove::playMove(Tank *tank, 
	TankAIWeaponSets::WeaponSet *weapons, bool useBatteries)
{
	std::list<Tank *> sortedTanks;	

	// Find the list of tanks we can shoot at 
	// In the order we want to shoot at them
	targets_.getTargets(tank, sortedTanks);

	// Check if we have taken a lot of damage and we can move
	float totalDamage = 
		targets_.getTotalDamageTaken() - totalDamageBeforeMove_;
	if (totalDamage > movementDamage_ &&
		RAND <= movementDamageChance_)
	{
		// Bring the health back up
		if (useBatteries)
		{
			useAvailableBatteries(tank);
		}

		if (tank->getLife().getLife().asFloat() > movementLife_)
		{
			// Try to move
			if (makeMoveShot(tank, weapons, sortedTanks)) return;
		}
	}

	// Check to see if we can make a huge shot at a number of tanks
	if (RAND <= groupShotChance_)
	{
		if (makeGroupShot(tank, weapons, sortedTanks)) return;
	}

	// Try to shoot at each tank in turn
	while (!sortedTanks.empty())
	{
		// Get the first tank
		Tank *targetTank = sortedTanks.front();
		sortedTanks.pop_front();

		// Get the list of weapons that might make sense here
		TankAICurrentMoveWeapons moveWeapons(tank, targetTank, weapons);

		// Try to shoot at it
		if (shootAtTank(tank, targetTank, moveWeapons)) return;

		// Keeping trying to shoot at the tanks until we make a shot
		// or run out of tanks

		// Check if we can use batteries, as perhaps we couldn't shoot 
		// due to a lack of power
		if (useBatteries &&
			tank->getLife().getLife() < tank->getLife().getMaxLife())
		{
			// Bring the health back up
			if (useAvailableBatteries(tank))
			{
				// Perhaps we can reach now so do this tank again
				sortedTanks.push_front(targetTank);
			}
		}
	}

	// Try to move so we can get a better shot at the targets
	// Only move if we have a hope of hitting them
	if (tank->getLife().getLife().asFloat() > movementLife_)
	{
		targets_.getTargets(tank, sortedTanks);
		if (makeMoveShot(tank, weapons, sortedTanks)) return;
	}

	// Is there any point in making a move
	// Done after select weapons to allow shields to be raised
	if (useResign_ &&
		tank->getLife().getLife().asFloat() < resignLife_) 
	{
		resign(tank);
		return;
	}

	// By default skip this move if we can't find anything to do
	// Perhaps we are burried etc...
	skipMove(tank);
}

bool TankAICurrentMove::shootAtTank(Tank *tank, Tank *targetTank, 
	TankAICurrentMoveWeapons &weapons)
{
	// Try to make a sniper shot
	if (makeSniperShot(tank, targetTank, weapons)) return true;

	// Try to make a laser shot
	if (makeLaserSniperShot(tank, targetTank, weapons)) return true;

	// Then a projectile shot
	if (makeProjectileShot(tank, targetTank, weapons)) return true;

	// Check if we are burried
	if (makeBurriedShot(tank, targetTank, weapons)) return true;

	return false;
}

bool TankAICurrentMove::makeProjectileShot(Tank *tank, Tank *targetTank, 
	TankAICurrentMoveWeapons &weapons)
{
	// Check we have any weapons to fire
	if (!weapons.roller &&
		!weapons.digger &&
		!weapons.napalm &&
		!weapons.large &&
		!weapons.small) return false;

	// Get the place we want to shoot at
	Vector directTarget = targetTank->getPosition().getTankPosition().asVector();

	// Check if the person is in a hole
	bool inhole = false;
	if (weapons.roller &&
		inHole(directTarget))
	{
		inhole = true;

		// Check for reflective shields
		if (weapons.shield &&
			(weapons.shield->getShieldType() == Shield::ShieldTypeRoundReflective ||
			weapons.shield->getShieldType() == Shield::ShieldTypeSquareReflective))
		{
			// Pick an area outside the shield
			// and make sure its downhill if we can
			directTarget = lowestHighest(weapons, directTarget, false);
		}
	}
	else
	{
		// Check for reflective shields
		if (weapons.shield &&
			(weapons.shield->getShieldType() == Shield::ShieldTypeRoundReflective ||
			weapons.shield->getShieldType() == Shield::ShieldTypeSquareReflective))
		{
			// Pick an area outside the shield
			// and make sure its uphill if we can
			directTarget = lowestHighest(weapons, directTarget, true);
		}
	}

	// Get the distance to get to this tank
	float tankAimDistance = getShotDistance(targetTank, true);
	tankAimDistance -= 5.0f;
	if (tankAimDistance < 0.0f) tankAimDistance = 0.0f;
	//Logger::log(S3D::formatStringBuffer("Aim Distance %.2f", tankAimDistance));

	// Find a place where we will hit
	Vector aimPosition = directTarget;
	float a = RAND * 3.14f * 2.0f;
	aimPosition[0] += sinf(a) * tankAimDistance;
	aimPosition[1] += cosf(a) * tankAimDistance;
	float aimDistance = MIN(tankAimDistance + 5.0f, 15.0f);

	// Check for all angles to see if we can shoot at this tank
	for (float degs=45.0f; degs<=85.0f; degs+=8.0f)
	{
		// Check this angle
		Vector actualPosition;
		TankAIAimGuesser aimGuesser;
		if (aimGuesser.guess(tank, aimPosition, degs, aimDistance, actualPosition))
		{	
			// Check we are not firing too close to us
			float distanceFromTarget = 
				(actualPosition - directTarget).Magnitude();
			float distanceFromUs = 
				(actualPosition - tank->getPosition().getTankPosition().asVector()).Magnitude();
			if (distanceFromUs < projectileMinDistance_) continue;

			// We can fire at this tank
			// ...
			// Check how close we are
			if (distanceFromTarget < largeWeaponUseDistance_)
			{
				// Check if the tank is in a hole
				if (inhole)
				{
					setWeapon(tank, weapons.roller);
				}
				else
				{
					// We are close
					// Choose a suitably good weapon
					if (weapons.shield)
					{
						// A shield beating weapon
						if (weapons.digger) setWeapon(tank, weapons.digger);
						else if (weapons.napalm) setWeapon(tank, weapons.napalm);
						else if (weapons.large) setWeapon(tank, weapons.large);
						else return false;
					}
					else
					{
						
						// A normal weapon
						if (weapons.digger) setWeapon(tank, weapons.digger);
						else if (weapons.large) setWeapon(tank, weapons.large);		
						else if (weapons.small) setWeapon(tank, weapons.small);					
						else return false;
					}
				}
			}
			else
			{
				// We are not close, choose a cheap weapon
				if (weapons.small) setWeapon(tank, weapons.small);	
				else if (weapons.large) setWeapon(tank, weapons.large);
				else return false;
			}

			// Fire the shot
			shotAtTank(targetTank, true, distanceFromTarget);
			fireShot(tank);
			return true;
		}
	}

	return false;
}

bool TankAICurrentMove::makeSniperShot(Tank *tank, Tank *targetTank, 
	TankAICurrentMoveWeapons &weapons)
{
	// Check if we have any weapons we can use for sniper
	if (!weapons.digger &&
		!weapons.laser &&
		!weapons.large &&
		!weapons.small) return false;

	// Get the place we want to shoot at
	Vector directTarget = targetTank->getPosition().getTankPosition().asVector();

	// First check to see if we can make a sniper shot that carries all the way
	// as this is generaly an easier shot
	float offset = getShotDistance(targetTank, false);
	TankAISniperGuesser sniperGuesser;
	if (sniperGuesser.guess(tank, directTarget, sniperUseDistance_, true, offset))
	{
		// We can make a ordinary sniper shot

		// Does this target have a bouncy shield
		if (!weapons.shield ||
			(weapons.shield->getShieldType() != Shield::ShieldTypeRoundReflective &&
			weapons.shield->getShieldType() != Shield::ShieldTypeSquareReflective))
		{
			// This is good, use a normal sniper shot
			if (weapons.shield)
			{
				// Use a shield beating weapon			
				if (weapons.digger) setWeapon(tank, weapons.digger);
				else if (weapons.laser) setWeapon(tank, weapons.laser);
				else if (weapons.large) setWeapon(tank, weapons.large);
				else return false;
			}
			else
			{
				// Just use an ordinary weapon
				if (weapons.digger) setWeapon(tank, weapons.digger);
				else if (weapons.large) setWeapon(tank, weapons.large);
				else if (weapons.small) setWeapon(tank, weapons.small);
				else return false;
			}

			// Fire the shot
			shotAtTank(targetTank, false, 0.0f);
			fireShot(tank);
			return true;
		}
		else if (weapons.laser)
		{
			// They have a reflective shield but we can use a laser
			// Set and fire the laser
			shotAtTank(targetTank, false, 0.0f);
			fireShot(tank);
			return true;
		}
	}

	return false;
}

bool TankAICurrentMove::makeLaserSniperShot(Tank *tank, Tank *targetTank, 
	TankAICurrentMoveWeapons &weapons)
{
	// Check if we have any lasers to fire
	if (!weapons.laser) return false;

	// Get the place we want to shoot at
	Vector directTarget = targetTank->getPosition().getTankPosition().asVector();
	
	// Second check to see if we can make a sniper shot that is obstructed
	// but could use a laser
	float offset = getShotDistance(targetTank, false);
	TankAISniperGuesser sniperGuesser;
	if (sniperGuesser.guess(tank, directTarget, sniperUseDistance_, false, offset))
	{
		if (weapons.laser)
		{
			// Set and fire the laser
			shotAtTank(targetTank, false, 0.0f);
			setWeapon(tank, weapons.laser);
			fireShot(tank);
			return true;
		}
	}

	return false;
}

bool TankAICurrentMove::makeBurriedShot(Tank *tank, Tank *targetTank, 
	TankAICurrentMoveWeapons &weapons)
{
	// Don't check if we can't uncover
	if (!weapons.uncover) return false;

	// Find a shot towards a target
	fixed xy, yz, power;
	TankLib::getSniperShotTowardsPosition(
		ScorchedServer::instance()->getContext(),
		tank->getPosition().getTankPosition(), targetTank->getPosition().getTankPosition(),
		100000, xy, yz, power);

	// Check if this shot is burried
	if (TankLib::intersection(
		ScorchedServer::instance()->getContext(), 
		tank->getPosition().getTankGunPosition(), 
		xy, yz, power, 2))
	{
		// Try to uncover the tank
		if (weapons.uncover)
		{
			tank->getPosition().rotateGunXY(xy, false);
			tank->getPosition().rotateGunYZ(yz, false);
			tank->getPosition().changePower(power, false);

			setWeapon(tank, weapons.uncover);
			fireShot(tank);
			return true;
		}
	}

	return false;
}

bool TankAICurrentMove::inHole(Vector &position)
{
	// Find the lowest pos around
	Vector pos = position;
	for (;;)
	{
		Vector lowest = pos;
		for (float a=0.0f; a<360.0f; a+=45.0f)
		{
			float offSetX = sinf(a / 180.0f * PI) * 1.25f;
			float offSetY = cosf(a / 180.0f * PI) * 1.25f;

			Vector newPos(
				pos[0] + offSetX,
				pos[1] + offSetY);
			newPos[2] =
				ScorchedServer::instance()->getLandscapeMaps().
					getGroundMaps().getInterpHeight(
						fixed::fromFloat(newPos[0]), 
						fixed::fromFloat(newPos[1])).asFloat();
			if (newPos[2] < lowest[2]) lowest = newPos;
		}

		if (lowest[2] < pos[2])
		{
			pos = lowest;
			Vector direction = pos - position;
			float dist =
				float(sqrt(direction[0]*direction[0] + direction[1]*direction[1]));
			if (dist > 6.0f) return false;
		}
		else
		{
			break;
		}
	}

	// Then see if this is in a hole
	for (float a=0.0f; a<360.0f; a+=22.5f)
	{
		bool ok = false;
		for (float radius=2.0f; radius<10.0f; radius+=1.0f)
		{
			float offSetX = sinf(a / 180.0f * PI) * radius;
			float offSetY = cosf(a / 180.0f * PI) * radius;
			
			Vector newPos(
				pos[0] + offSetX,
				pos[1] + offSetY);
			newPos[2] =
				ScorchedServer::instance()->getLandscapeMaps().
					getGroundMaps().getInterpHeight(
					fixed::fromFloat(newPos[0]), 
					fixed::fromFloat(newPos[1])).asFloat();

			float heightDiff = newPos[2] - pos[2];
			if (heightDiff < -2.0f) 
			{
				return false; // Its lower
			}
			if (heightDiff > 2.0f) 
			{
				ok = true;
				break;
			}
		}
		if (!ok) return false;
	}

	return true;
}

bool TankAICurrentMove::makeMoveShot(Tank *tank, 
	TankAIWeaponSets::WeaponSet *weapons,
	std::list<Tank *> &sortedTanks)
{
	if (!useFuel_) return false;
	if (sortedTanks.empty()) return false;

	Accessory *fuel = weapons->getTankAccessoryByType(tank, "fuel");
	if (!fuel) return false;

	ScorchedContext &context = ScorchedServer::instance()->getContext();
	WeaponMoveTank *moveWeapon = (WeaponMoveTank *)
		context.getAccessoryStore().findAccessoryPartByAccessoryId(
			fuel->getAccessoryId(), "WeaponMoveTank");
	if (moveWeapon)
	{
		// Try to find a position to move to that we want to move to
		// For the moment, just use the 1st target
		Tank *target = sortedTanks.front();
		Vector targetPos = target->getPosition().getTankPosition().asVector();
		Vector tankPos = tank->getPosition().getTankPosition().asVector();
		float totalDistance = MAX(100.0f, MIN(500.0f, (targetPos - tankPos).Magnitude() * 2.0f));

		// Can we move to this target at all?
		MovementMap mmap(
			tank, 
			context);
		if (!mmap.calculatePosition(FixedVector::fromVector(targetPos), 
			fixed::fromFloat(totalDistance))) return false;
		float totalFuel = mmap.getFuel(moveWeapon).asFloat();
		if (totalFuel <= 5) return false; // Stop it from moving very small amounts

		// Calculate the path
		MovementMap::MovementMapEntry entry =
			mmap.getEntry((int) targetPos[0], (int) targetPos[1]);
		if (entry.type != MovementMap::eMovement) return false;

		// Work backward to the source point finding the nearest point we
		// can actualy move to
		while (entry.srcEntry)
		{
			unsigned int pt = entry.srcEntry;
			unsigned int x = pt >> 16;
			unsigned int y = pt & 0xffff;

			Vector position((float) x, (float) y,
				ScorchedServer::instance()->getLandscapeMaps().getGroundMaps().getHeight(
					(int) x, (int) y).asFloat());
			float distance = (position - targetPos).Magnitude();
			if (distance > movementCloseness_)
			{
				if (entry.dist.asFloat() < totalFuel)
				{
					// Move
					totalDamageBeforeMove_ = targets_.getTotalDamageTaken();

					// Move
					tank->getPosition().setSelectPosition((int) x, (int) y);
					setWeapon(tank, fuel);
					fireShot(tank);

					return true;
				}
			}

			entry = mmap.getEntry(x, y);
		}	
	}

	return false;
}

struct GroupingEntry
{
	Vector position;
	std::list<Tank *> targets;
	float totalDistance;
};

bool TankAICurrentMove::makeGroupShot(Tank *tank, 
	TankAIWeaponSets::WeaponSet *weapons,
	std::list<Tank *> &sortedTanks)
{
	if (groupShotSize_ == 0) return false;
	Accessory *explosionhuge = weapons->getTankAccessoryByType(tank, "explosionhuge");
	if (!explosionhuge) return false;
	
	std::list<GroupingEntry> foundEntries;
	HeightMap &map = 
		ScorchedServer::instance()->getLandscapeMaps().getGroundMaps().getHeightMap();

	int arenaX = ScorchedServer::instance()->getLandscapeMaps().
		getGroundMaps().getArenaX();
	int arenaY = ScorchedServer::instance()->getLandscapeMaps().
		getGroundMaps().getArenaY();
	int arenaWidth = ScorchedServer::instance()->getLandscapeMaps().
		getGroundMaps().getArenaWidth();
	int arenaHeight = ScorchedServer::instance()->getLandscapeMaps().
		getGroundMaps().getArenaHeight();

	// Braindead way of finding groupings
	// For each landscape square
	for (int y=arenaY; y<arenaY + arenaHeight; y+=4)
	{
		for (int x=arenaX; x<arenaX + arenaWidth; x+=4)
		{
			GroupingEntry entry;
			entry.position = Vector(float(x), float(y), map.getHeight(x, y).asFloat());
			entry.totalDistance = 0.0f;

			// Check this is not too near to us!
			float distance = 
				(tank->getPosition().getTankPosition().asVector() - 
				entry.position).Magnitude();
			if (distance < groupTargetDistance_ * 2.0f) continue;
			
			// Find all tanks near this position			
			std::list<Tank *>::iterator toItor;
			for (toItor = sortedTanks.begin();
				toItor != sortedTanks.end();
				toItor++)
			{
				Tank *to = *toItor;

				distance = 
					(to->getPosition().getTankPosition().asVector() - 
					entry.position).Magnitude();
				if (distance < groupTargetDistance_)
				{
					entry.totalDistance += distance;
					entry.targets.push_back(to);
				}
			}

			// Are there enough to warrent a shot
			if (entry.targets.size() >= (unsigned int) groupShotSize_)
			{
				foundEntries.push_back(entry);
			}
		}
	}

	// Find the best entry
	if (!foundEntries.empty())
	{
		GroupingEntry *current = 0;
		std::list<GroupingEntry>::iterator itor;
		for (itor = foundEntries.begin();
			itor != foundEntries.end();
			itor++)
		{
			GroupingEntry &entry = *itor;
			if (!current ||
				entry.targets.size() > current->targets.size() ||
				(entry.targets.size() == current->targets.size() &&
				entry.totalDistance < current->totalDistance))
			{
				current = &entry;
			}
		}

		if (current)
		{
			// Check for all angles to see if we can shoot at this tank
			for (float degs=85.0f; degs>=45.0f; degs-=8.0f)
			{
				// Check this angle
				Vector actualPosition;
				TankAIAimGuesser aimGuesser;
				if (aimGuesser.guess(tank, current->position, 
					degs, 15.0f, actualPosition))
				{
					setWeapon(tank, explosionhuge);
					fireShot(tank);
					return true;
				}
			}
		}
	}

	return false;
}

bool TankAICurrentMove::useAvailableBatteries(Tank *tank)
{
	// Use batteries
	bool result = false;
	while (tank->getLife().getLife() < 
		tank->getLife().getMaxLife() &&
		tank->getAccessories().getBatteries().canUse())
	{
		std::list<Accessory *> &entries =
			tank->getAccessories().getAllAccessoriesByType(
				AccessoryPart::AccessoryBattery);			
		if (!entries.empty())
		{
			useBattery(tank, entries.front()->getAccessoryId());
			result = true;
		}
	}
	return result;
}

Vector TankAICurrentMove::lowestHighest(TankAICurrentMoveWeapons &weapons, 
	Vector &directTarget, bool highest)
{
	float radius = weapons.shield->getBoundingSize().asFloat() + 2.0f;
	Vector bestPos = directTarget;
	bestPos[1] += radius;
	bestPos[2] = 
		ScorchedServer::instance()->getLandscapeMaps().
			getGroundMaps().getInterpHeight(
				fixed::fromFloat(bestPos[0]), fixed::fromFloat(bestPos[1])).asFloat();
	for (float a=0.0f; a<360.0f; a+=22.5f)
	{
		float offSetX = sinf(a / 180.0f * PI) * radius;
		float offSetY = cosf(a / 180.0f * PI) * radius;
		
		Vector newPos(
			directTarget[0] + offSetX,
			directTarget[1] + offSetY);
		newPos[2] =
			ScorchedServer::instance()->getLandscapeMaps().
				getGroundMaps().getInterpHeight(
					fixed::fromFloat(newPos[0]), fixed::fromFloat(newPos[1])).asFloat();

		if (highest)
		{
			if (newPos[2] > bestPos[2]) bestPos = newPos;
		}
		else
		{
			if (newPos[2] < bestPos[2]) bestPos = newPos;
		}
	}
	return bestPos;
}

float TankAICurrentMove::getShotDistance(Tank *tank, bool projectile)
{
	// Try to find an existing record
	std::map<Tank *, ShotRecord>::iterator itor = 
		shotRecords_.find(tank);
	if (itor == shotRecords_.end())
	{
		if (projectile) return projectileStartDistance_;
		else return sniperStartDistance_;
	}
	else
	{
		if (projectile) return itor->second.projectileCurrentDistance;
		else return itor->second.sniperCurrentDistance;
	}
}

void TankAICurrentMove::shotAtTank(Tank *tank, bool projectile, float newDistance)
{
	targets_.shotAt(tank);

	// Try to find an existing record
	std::map<Tank *, ShotRecord>::iterator itor = 
		shotRecords_.find(tank);
	if (itor == shotRecords_.end())
	{
		// Create one
		ShotRecord record;
		record.projectileCurrentDistance = projectileStartDistance_; 
		record.sniperCurrentDistance = sniperStartDistance_;
		record.position = tank->getPosition().getTankPosition().asVector();
		shotRecords_[tank] = record;
	}

	// Update the new record with the details about the current shot
	ShotRecord &record = shotRecords_[tank];
	float distance = (record.position - tank->getPosition().getTankPosition().asVector()).Magnitude();
	float distanceDec = 0.0f;
	if (distance > 5.0f)
	{
		distanceDec = MIN(distance - 5.0f, 20.0f) / 20.0f;
	}

	record.position = tank->getPosition().getTankPosition().asVector();
	if (projectile)
	{
		record.projectileCurrentDistance = newDistance;

		float decrement = 
			projectileMinDecrement_ +
			RAND * (projectileMaxDecrement_ - projectileMinDecrement_);
		record.projectileCurrentDistance = 
			MAX(projectileEndDistance_, record.projectileCurrentDistance - decrement);				

		distanceDec *= projectileMovementFactor_;
		record.projectileCurrentDistance = 
			MIN(projectileStartDistance_, record.projectileCurrentDistance + distanceDec);	
	}
	else 
	{
		float decrement = 
			sniperMinDecrement_ +
			RAND * (sniperMaxDecrement_ - sniperMinDecrement_);
		record.sniperCurrentDistance = 
			MAX(sniperEndDistance_, record.sniperCurrentDistance - decrement);	

		distanceDec *= sniperMovementFactor_;
		record.sniperCurrentDistance = 
			MIN(sniperStartDistance_, record.sniperCurrentDistance + distanceDec);	
	}
}

void TankAICurrentMove::setWeapon(Tank *tank, Accessory *accessory)
{
	tank->getAccessories().getWeapons().setWeapon(accessory);
}

void TankAICurrentMove::skipMove(Tank *tank)
{
	ComsPlayedMoveMessage *message = 
		new ComsPlayedMoveMessage(tank->getPlayerId(), 
			0,
			ComsPlayedMoveMessage::eSkip);
	ServerShotHolder::instance()->addShot(tank->getPlayerId(), message);
}

void TankAICurrentMove::resign(Tank *tank)
{
	ComsPlayedMoveMessage *message = 
		new ComsPlayedMoveMessage(tank->getPlayerId(), 
			0, ComsPlayedMoveMessage::eResign);
	ServerShotHolder::instance()->addShot(tank->getPlayerId(), message);
}

void TankAICurrentMove::fireShot(Tank *tank)
{
	Accessory *currentWeapon = 
		tank->getAccessories().getWeapons().getCurrent();
	if (currentWeapon)
	{
		ComsPlayedMoveMessage *message = 
			new ComsPlayedMoveMessage(tank->getPlayerId(), 
				0, ComsPlayedMoveMessage::eShot);
		message->setShot(
			currentWeapon->getAccessoryId(),
			tank->getPosition().getRotationGunXY(),
			tank->getPosition().getRotationGunYZ(),
			tank->getPosition().getPower(),
			tank->getPosition().getSelectPositionX(),
			tank->getPosition().getSelectPositionY());
	
		if (!ServerShotHolder::instance()->addShot(tank->getPlayerId(), message))
		{
			Logger::log(S3D::formatStringBuffer("AI %u failed to make a shot, shot refused",
				tank->getPlayerId()));
			skipMove(tank);
		}
	}
}

void TankAICurrentMove::useBattery(Tank *tank, unsigned int batteryId)
{
	ComsDefenseMessage defenseMessage(
		tank->getPlayerId(),
		ComsDefenseMessage::eBatteryUse,
		batteryId);

	ServerDefenseHandler::instance()->processDefenseMessage(defenseMessage, tank);
}
