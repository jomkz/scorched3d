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

#include <tankai/TankAICurrentMove.h>
#include <tankai/TankAIAimGuesser.h>
#include <tankai/TankAISniperGuesser.h>
#include <coms/ComsPlayedMoveMessage.h>
#include <coms/ComsDefenseMessage.h>
#include <common/Logger.h>
#include <common/OptionsScorched.h>
#include <server/ScorchedServer.h>
#include <server/ServerSimulator.h>
#include <server/ServerState.h>
#include <simactions/TankDefenseSimAction.h>
#include <simactions/TankStartMoveSimAction.h>
#include <tankai/TankAI.h>
#include <tank/Tank.h>
#include <tank/TankLib.h>
#include <tanket/TanketAccessories.h>
#include <tanket/TanketShotInfo.h>
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
	XMLEntryGroup("attack", "Offensive AI parameters"),
	resignGroup_("resign", "Parameters to determine if this AI should resign"),
	useResign_("useresign", "Enable resigning ability"),
	resignLife_("resignlife", "Don't consider resigning unless you have less than this amount of life"),
	movementGroup_("movement", "Parameters to determine if this AI should move.  Tank must have fuel before movement can occur."),
	useFuel_("usefuel", "Enable movement ability"),		
	movementDamage_("movementdamage", "The amount of damage taken before movement is considered"),
	movementDamageChance_("movementdamagechance", "Once the above damage has been taken the chance to move out the way"),
	movementLife_("movementlife", "The amount of life before we consider moving"),
	movementRandom_("movementrandom", "The randomness applied to the final movement position"),
	movementCloseness_("movementcloseness", "The distance left between the target tank and the movement destination"),
	groupShotGroup_("groupshot", "Parameters to determine when to try to kill multiple tanks (in a group)"),
	groupShotSize_("groupshotsize", "The number of tanks that need to be grouped before considering a group shot A group shot is available if targets are close together and the tank has a hugeweapon"),
	groupShotChance_("groupshotchance", "The chance that a group shot will be considered"),
	groupTargetDistance_("grouptargetdistance", "The max distance between tanks considered grouped"),
	sniperGroup_("sniper", "Parameters to determine if this AI should use sniping shots (direct shots at max power)"),
	sniperMovementFactor_("snipermovementfactor", "The maximum amount of accuracy in degrees that will be lost if the tank moves"),
	sniperUseDistance_("sniperusedistance", "How close do I need to be from a target before considering a sniper shot"),
	sniperStartDistance_("sniperstartdistance", "The distance in degress that my 1st sniper shot may miss by"),
	sniperEndDistance_("sniperenddistance", "The distance in degrees that my best sniper shot may miss by"),
	sniperMinDecrement_("snipermindecrement", "The min distance in degrees that each sniper shot will improve"),
	sniperMaxDecrement_("snipermaxdecrement", "The max distance in degrees that each sniper shot will improve "),
	projectileGroup_("projectile", "Parameters to determine how this AI should use projectiles"),
	projectileMinDistance_("projectilemindistance", "The minimum distance to allow for shots. Stops self kills"),
	projectileStartDistance_("projectilestartdistance", "These are all distances of the 1st particle of the projectile to hit the ground. The distance that the 1st projectile shot may miss by"),
	projectileEndDistance_("projectileenddistance", "The distance that the my best projectile shot may get to"),
	projectileMinDecrement_("projectilemindecrement", "The min distance that each projecile shot will improve"),
	projectileMaxDecrement_("projectilemaxdecrement", "The max distance that each projecile shot will improve"),
	largeWeaponUseDistance_("largeweaponusedistance", "How close a shot do I need to get to a target before firing large weapons"),
	projectileMovementFactor_("projectilemovementfactor", "The maximum amount of accuracy that will be lost if the tank moves"),
	totalDamageBeforeMove_(0.0f),
	currentWeapon_(0)
{
	addChildXMLEntry(&resignGroup_);
	resignGroup_.addChildXMLEntry(&useResign_);
	resignGroup_.addChildXMLEntry(&resignLife_);
	addChildXMLEntry(&movementGroup_);
	movementGroup_.addChildXMLEntry(&useFuel_);
	movementGroup_.addChildXMLEntry(&movementDamage_);
	movementGroup_.addChildXMLEntry(&movementDamageChance_);
	movementGroup_.addChildXMLEntry(&movementLife_);
	movementGroup_.addChildXMLEntry(&movementRandom_);
	movementGroup_.addChildXMLEntry(&movementCloseness_);
	addChildXMLEntry(&groupShotGroup_);
	groupShotGroup_.addChildXMLEntry(&groupShotGroup_);
	groupShotGroup_.addChildXMLEntry(&groupShotSize_);
	groupShotGroup_.addChildXMLEntry(&groupShotChance_);
	groupShotGroup_.addChildXMLEntry(&groupTargetDistance_);
	addChildXMLEntry(&sniperGroup_);
	sniperGroup_.addChildXMLEntry(&sniperMovementFactor_);
	sniperGroup_.addChildXMLEntry(&sniperUseDistance_);
	sniperGroup_.addChildXMLEntry(&sniperStartDistance_);
	sniperGroup_.addChildXMLEntry(&sniperEndDistance_);
	sniperGroup_.addChildXMLEntry(&sniperMinDecrement_);
	sniperGroup_.addChildXMLEntry(&sniperMaxDecrement_);
	addChildXMLEntry(&projectileGroup_);
	projectileGroup_.addChildXMLEntry(&projectileMinDistance_);
	projectileGroup_.addChildXMLEntry(&projectileMovementFactor_);
	projectileGroup_.addChildXMLEntry(&projectileStartDistance_);
	projectileGroup_.addChildXMLEntry(&projectileEndDistance_);
	projectileGroup_.addChildXMLEntry(&projectileMinDecrement_);
	projectileGroup_.addChildXMLEntry(&projectileMaxDecrement_);
	projectileGroup_.addChildXMLEntry(&largeWeaponUseDistance_);
}

TankAICurrentMove::~TankAICurrentMove()
{
}

void TankAICurrentMove::clear()
{
	totalDamageBeforeMove_ = 0.0f;
	shotRecords_.clear();
}

void TankAICurrentMove::playMove(Tanket *tanket, 
	TankAIWeaponSets::WeaponSet *weapons, bool useBatteries,
	unsigned int moveId)
{
	MoveData moveData;
	moveData.moveId = moveId;
	moveData.madeMove = false;
	moveData.usedBatteries = false;
	playMoveInternal(tanket, weapons, useBatteries, moveData);

	if (!moveData.madeMove)
	{
		if (moveData.usedBatteries)
		{
			TankStartMoveSimAction *simAction = 
				new TankStartMoveSimAction(tanket->getPlayerId(), moveId, 0, false, 0);
			ScorchedServer::instance()->getServerSimulator().addSimulatorAction(simAction);
		}
		else
		{
			DIALOG_ASSERT(0);
		}
	}
}

void TankAICurrentMove::playMoveInternal(Tanket *tanket, 
	TankAIWeaponSets::WeaponSet *weapons, bool useBatteries,
	MoveData &moveData)
{
	if (TankAI::getTankAILogging())
	{
		Logger::log(S3D::formatStringBuffer("----- TankAI %u %s ----------------",
			tanket->getPlayerId(), tanket->getCStrName().c_str()));
	}

	std::list<Tanket *> sortedTankets;	

	// Find the list of tanks we can shoot at 
	// In the order we want to shoot at them
	targets_.getTargets(tanket, sortedTankets);
	if (TankAI::getTankAILogging())
	{
		Logger::log(S3D::formatStringBuffer("TankAI - Found %u tanks to shoot at", sortedTankets.size()));
	}

	// Check if we have taken a lot of damage and we can move
	float totalDamage = 
		targets_.getTotalDamageTaken() - totalDamageBeforeMove_;
	if (totalDamage > movementDamage_.getValue().asFloat() &&
		S3D_RAND <= movementDamageChance_.getValue().asFloat())
	{
		// Bring the health back up
		if (useBatteries)
		{
			if (useAvailableBatteries(tanket, moveData)) return;
		}

		if (tanket->getLife().getLife().asFloat() > movementLife_.getValue().asFloat())
		{
			// Try to move
			if (makeMoveShot(tanket, weapons, sortedTankets, moveData)) return;
		}
	}

	// Check to see if we can make a huge shot at a number of tanks
	if (S3D_RAND <= groupShotChance_.getValue().asFloat())
	{
		if (makeGroupShot(tanket, weapons, sortedTankets, moveData)) return;
	}

	// Try to shoot at each tank in turn
	while (!sortedTankets.empty())
	{
		// Get the first tank
		Tanket *targetTanket = sortedTankets.front();
		sortedTankets.pop_front();

		if (TankAI::getTankAILogging())
		{
			Logger::log(S3D::formatStringBuffer("TankAI - Checking target %u %s", 
				targetTanket->getPlayerId(), targetTanket->getCStrName().c_str()));
		}

		// Get the list of weapons that might make sense here
		TankAICurrentMoveWeapons moveWeapons(tanket, targetTanket, weapons);

		// Try to shoot at it
		if (shootAtTank(tanket, targetTanket, moveWeapons, moveData)) return;

		// Keeping trying to shoot at the tanks until we make a shot
		// or run out of tanks

		// Check if we can use batteries, as perhaps we couldn't shoot 
		// due to a lack of power
		if (useBatteries &&
			tanket->getLife().getLife() < tanket->getLife().getMaxLife())
		{
			// Bring the health back up
			if (useAvailableBatteries(tanket, moveData)) return;
		}
	}

	if (TankAI::getTankAILogging())
	{
		Logger::log(S3D::formatStringBuffer("TankAI - no targets can be shot at"));
	}

	// Try to move so we can get a better shot at the targets
	// Only move if we have a hope of hitting them
	if (tanket->getLife().getLife().asFloat() > movementLife_.getValue().asFloat())
	{
		targets_.getTargets(tanket, sortedTankets);
		if (makeMoveShot(tanket, weapons, sortedTankets, moveData)) return;
	}

	// Is there any point in making a move
	// Done after select weapons to allow shields to be raised
	if (useResign_.getValue() &&
		ScorchedServer::instance()->getOptionsGame().getResignMode() != OptionsGame::ResignNone &&
		tanket->getLife().getLife().asFloat() < resignLife_.getValue().asFloat()) 
	{
		resign(tanket, moveData);
		return;
	}

	// By default skip this move if we can't find anything to do
	// Perhaps we are burried etc...
	skipMove(tanket, moveData);
}

bool TankAICurrentMove::shootAtTank(Tanket *tanket, Tanket *targetTanket, 
	TankAICurrentMoveWeapons &weapons, MoveData &moveData)
{
	currentWeapon_ = 0;

	// Try to make a sniper shot
	if (makeSniperShot(tanket, targetTanket, weapons, moveData)) return true;

	// Try to make a laser shot
	if (makeLaserSniperShot(tanket, targetTanket, weapons, moveData)) return true;

	// Then a projectile shot
	if (makeProjectileShot(tanket, targetTanket, weapons, moveData)) return true;

	// Check if we are burried
	if (makeBurriedShot(tanket, targetTanket, weapons, moveData)) return true;

	return false;
}

bool TankAICurrentMove::makeProjectileShot(Tanket *tanket, Tanket *targetTanket, 
	TankAICurrentMoveWeapons &weapons, MoveData &moveData)
{
	if (TankAI::getTankAILogging())
	{
		Logger::log(S3D::formatStringBuffer("TankAI - check for projectile shot"));
	}

	// Check we have any weapons to fire
	if (!weapons.roller &&
		!weapons.digger &&
		!weapons.napalm &&
		!weapons.large &&
		!weapons.small) 
	{
		if (TankAI::getTankAILogging())
		{
			Logger::log(S3D::formatStringBuffer("TankAI - no projectile weapons suitable"));
		}
		return false;
	}

	// Get the place we want to shoot at
	Vector directTarget = targetTanket->getLife().getTargetPosition().asVector();

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
	float tankAimDistance = getShotDistance(targetTanket, true);
	tankAimDistance -= 5.0f;
	if (tankAimDistance < 0.0f) tankAimDistance = 0.0f;
	//Logger::log(S3D::formatStringBuffer("Aim Distance %.2f", tankAimDistance));

	// Find a place where we will hit
	Vector aimPosition = directTarget;
	float a = S3D_RAND * 3.14f * 2.0f;
	aimPosition[0] += sinf(a) * tankAimDistance;
	aimPosition[1] += cosf(a) * tankAimDistance;
	float aimDistance = S3D_MIN(tankAimDistance + 5.0f, 15.0f);

	// Check for all angles to see if we can shoot at this tank
	for (float degs=45.0f; degs<=85.0f; degs+=8.0f)
	{
		// Check this angle
		Vector actualPosition;
		TankAIAimGuesser aimGuesser(ScorchedServer::instance()->getContext());
		TankAIAimResult result;
		if (aimGuesser.guess(tanket, aimPosition, degs, aimDistance, actualPosition, result))
		{	
			// Check we are not firing too close to us
			float distanceFromTarget = 
				(actualPosition - directTarget).Magnitude();
			float distanceFromUs = 
				(actualPosition - tanket->getLife().getTargetPosition().asVector()).Magnitude();
			if (distanceFromUs < projectileMinDistance_.getValue().asFloat())
			{
				if (TankAI::getTankAILogging())
				{
					Logger::log(S3D::formatStringBuffer("TankAI - target too close"));
				}
				continue;
			}

			// We can fire at this tank
			// ...
			// Check how close we are
			if (distanceFromTarget < largeWeaponUseDistance_.getValue().asFloat())
			{
				if (TankAI::getTankAILogging())
				{
					Logger::log(S3D::formatStringBuffer("TankAI - close distance to target"));
				}

				// Check if the tank is in a hole
				if (inhole)
				{
					setWeapon(tanket, weapons.roller);
					if (TankAI::getTankAILogging())
					{
						Logger::log(S3D::formatStringBuffer("TankAI - in hole roller used"));
					}
				}
				else
				{
					// We are close
					// Choose a suitably good weapon
					if (weapons.shield)
					{
						// A shield beating weapon
						if (weapons.digger) setWeapon(tanket, weapons.digger);
						else if (weapons.napalm) setWeapon(tanket, weapons.napalm);
						else if (weapons.large) setWeapon(tanket, weapons.large);
						else 
						{
							if (TankAI::getTankAILogging())
							{
								Logger::log(S3D::formatStringBuffer("TankAI - no projectile shield weapons found"));
							}
							return false;
						}
					}
					else
					{
						// A normal weapon
						if (weapons.digger) setWeapon(tanket, weapons.digger);
						else if (weapons.large) setWeapon(tanket, weapons.large);		
						else if (weapons.small) setWeapon(tanket, weapons.small);					
						else 
						{
							if (TankAI::getTankAILogging())
							{
								Logger::log(S3D::formatStringBuffer("TankAI - no projectile normal weapons found"));
							}
							return false;
						}
					}
				}
			}
			else
			{
				if (TankAI::getTankAILogging())
				{
					Logger::log(S3D::formatStringBuffer("TankAI - far distance to target"));
				}

				// We are not close, choose a cheap weapon
				if (weapons.small) setWeapon(tanket, weapons.small);	
				else if (weapons.large) setWeapon(tanket, weapons.large);
				else 
				{
					if (TankAI::getTankAILogging())
					{
						Logger::log(S3D::formatStringBuffer("TankAI - no normal weapons found"));
					}
					return false;
				}
			}

			// Fire the shot
			shotAtTank(targetTanket, true, distanceFromTarget);
			fireShot(tanket, moveData, result);
			return true;
		}
		else
		{
			if (TankAI::getTankAILogging())
			{
				Logger::log(S3D::formatStringBuffer("TankAI - failed to find a shot at %.2f degrees", degs));
			}
		}
	}

	return false;
}

bool TankAICurrentMove::makeSniperShot(Tanket *tanket, Tanket *targetTanket, 
	TankAICurrentMoveWeapons &weapons, MoveData &moveData)
{
	if (TankAI::getTankAILogging())
	{
		Logger::log(S3D::formatStringBuffer("TankAI - check for sniper shot"));
	}

	// Check if we have any weapons we can use for sniper
	if (!weapons.digger &&
		!weapons.laser &&
		!weapons.large &&
		!weapons.small) 
	{
		if (TankAI::getTankAILogging())
		{
			Logger::log(S3D::formatStringBuffer("TankAI - no sniper weapons suitable"));
		}
		return false;
	}

	// Get the place we want to shoot at
	Vector directTarget = targetTanket->getLife().getTargetPosition().asVector();

	// First check to see if we can make a sniper shot that carries all the way
	// as this is generaly an easier shot
	float offset = getShotDistance(targetTanket, false);
	TankAIAimResult result;
	TankAISniperGuesser sniperGuesser;
	if (sniperGuesser.guess(tanket, directTarget, sniperUseDistance_.getValue().asFloat(), true, offset, result))
	{
		// We can make a ordinary sniper shot
		if (TankAI::getTankAILogging())
		{
			Logger::log(S3D::formatStringBuffer("TankAI - sniper target in range"));
		}

		// Does this target have a bouncy shield
		if (!weapons.shield ||
			(weapons.shield->getShieldType() != Shield::ShieldTypeRoundReflective &&
			weapons.shield->getShieldType() != Shield::ShieldTypeSquareReflective))
		{
			// This is good, use a normal sniper shot
			if (weapons.shield)
			{
				// Use a shield beating weapon			
				if (weapons.digger) setWeapon(tanket, weapons.digger);
				else if (weapons.laser) setWeapon(tanket, weapons.laser);
				else if (weapons.large) setWeapon(tanket, weapons.large);
				else 
				{
					if (TankAI::getTankAILogging())
					{
						Logger::log(S3D::formatStringBuffer("TankAI - no sniper shield weapons"));
					}
					return false;
				}
			}
			else
			{
				// Just use an ordinary weapon
				if (weapons.digger) setWeapon(tanket, weapons.digger);
				else if (weapons.large) setWeapon(tanket, weapons.large);
				else if (weapons.small) setWeapon(tanket, weapons.small);
				else 
				{
					if (TankAI::getTankAILogging())
					{
						Logger::log(S3D::formatStringBuffer("TankAI - no sniper weapons"));
					}
					return false;
				}
			}

			// Fire the shot
			shotAtTank(targetTanket, false, 0.0f);
			fireShot(tanket, moveData, result);
			return true;
		}
		else if (weapons.laser)
		{
			// They have a reflective shield but we can use a laser
			// Set and fire the laser
			shotAtTank(targetTanket, false, 0.0f);
			fireShot(tanket, moveData, result);
			return true;
		}
	}

	return false;
}

bool TankAICurrentMove::makeLaserSniperShot(Tanket *tanket, Tanket *targetTanket, 
	TankAICurrentMoveWeapons &weapons, MoveData &moveData)
{
	if (TankAI::getTankAILogging())
	{
		Logger::log(S3D::formatStringBuffer("TankAI - check for laser sniper shot"));
	}

	// Check if we have any lasers to fire
	if (!weapons.laser)
	{
		if (TankAI::getTankAILogging())
		{
			Logger::log(S3D::formatStringBuffer("TankAI - no laser sniper weapons suitable"));
		}
		return false;
	}

	// Get the place we want to shoot at
	Vector directTarget = targetTanket->getLife().getTargetPosition().asVector();
	
	// Second check to see if we can make a sniper shot that is obstructed
	// but could use a laser
	float offset = getShotDistance(targetTanket, false);
	TankAIAimResult result;
	TankAISniperGuesser sniperGuesser;
	if (sniperGuesser.guess(tanket, directTarget, sniperUseDistance_.getValue().asFloat(), false, offset, result))
	{
		if (TankAI::getTankAILogging())
		{
			Logger::log(S3D::formatStringBuffer("TankAI - sniper target in range"));
		}

		// Set and fire the laser
		shotAtTank(targetTanket, false, 0.0f);
		setWeapon(tanket, weapons.laser);
		fireShot(tanket, moveData, result);
		return true;
	}

	return false;
}

bool TankAICurrentMove::makeBurriedShot(Tanket *tanket, Tanket *targetTanket, 
	TankAICurrentMoveWeapons &weapons, MoveData &moveData)
{
	if (TankAI::getTankAILogging())
	{
		Logger::log(S3D::formatStringBuffer("TankAI - check for burried shot"));
	}

	// Don't check if we can't uncover
	if (!weapons.uncover)
	{
		if (TankAI::getTankAILogging())
		{
			Logger::log(S3D::formatStringBuffer("TankAI - no burried shot weapons"));
		}

		return false;
	}

	// Find a shot towards a target
	fixed xy, yz, power;
	TankLib::getSniperShotTowardsPosition(
		ScorchedServer::instance()->getContext(),
		tanket->getLife().getTargetPosition(), targetTanket->getLife().getTargetPosition(),
		100000, xy, yz, power);

	// Check if this shot is burried
	FixedVector tankTurretPosition, tankGunPosition;
	tanket->getLife().getTankTurretPosition(tankTurretPosition);
	TankLib::getTankGunPosition(tankGunPosition, tankTurretPosition, xy, yz);
	if (TankLib::intersection(
		ScorchedServer::instance()->getContext(), 
		tankGunPosition,
		xy, yz, power, 2))
	{
		if (TankAI::getTankAILogging())
		{
			Logger::log(S3D::formatStringBuffer("TankAI - tank burried"));
		}

		// Try to uncover the tank
		TankAIAimResult result(xy, yz, power);
		setWeapon(tanket, weapons.uncover);
		fireShot(tanket, moveData, result);
		return true;
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
			float offSetX = sinf(a / 180.0f * S3D_PI) * 1.25f;
			float offSetY = cosf(a / 180.0f * S3D_PI) * 1.25f;

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
			float offSetX = sinf(a / 180.0f * S3D_PI) * radius;
			float offSetY = cosf(a / 180.0f * S3D_PI) * radius;
			
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

bool TankAICurrentMove::makeMoveShot(Tanket *tanket, 
	TankAIWeaponSets::WeaponSet *weapons,
	std::list<Tanket *> &sortedTankets,
	MoveData &moveData)
{
	if (TankAI::getTankAILogging())
	{
		Logger::log(S3D::formatStringBuffer("TankAI - Checking movement"));
	}

	if (!useFuel_.getValue()) return false;
	if (sortedTankets.empty()) return false;

	Accessory *fuel = weapons->getTankAccessoryByType(tanket, "fuel");
	if (!fuel) return false;

	ScorchedContext &context = ScorchedServer::instance()->getContext();
	WeaponMoveTank *moveWeapon = (WeaponMoveTank *)
		context.getAccessoryStore().findAccessoryPartByAccessoryId(
			fuel->getAccessoryId(), "WeaponMoveTank");
	if (moveWeapon)
	{
		// Try to find a position to move to that we want to move to
		// For the moment, just use the 1st target
		Tanket *target = sortedTankets.front();
		Vector targetPos = target->getLife().getTargetPosition().asVector();
		Vector tankPos = tanket->getLife().getTargetPosition().asVector();
		float totalDistance = S3D_MAX(100.0f, S3D_MIN(500.0f, (targetPos - tankPos).Magnitude() * 2.0f));

		// Can we move to this target at all?
		MovementMap mmap(
			tanket, 
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
			if (distance > movementCloseness_.getValue().asFloat())
			{
				if (entry.dist.asFloat() < totalFuel)
				{
					// Move
					totalDamageBeforeMove_ = targets_.getTotalDamageTaken();

					// Move
					TankAIAimResult result;
					result.selectPositionX_ = (int) x;
					result.selectPositionY_ = (int) y;
					setWeapon(tanket, fuel);
					fireShot(tanket, moveData, result);

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
	std::list<Tanket *> targets;
	float totalDistance;
};

bool TankAICurrentMove::makeGroupShot(Tanket *tanket, 
	TankAIWeaponSets::WeaponSet *weapons,
	std::list<Tanket *> &sortedTankets,
	MoveData &moveData)
{
	if (TankAI::getTankAILogging())
	{
		Logger::log(S3D::formatStringBuffer("TankAI - Checking group shot"));
	}

	if (groupShotSize_.getValue() == 0) return false;
	Accessory *explosionhuge = weapons->getTankAccessoryByType(tanket, "explosionhuge");
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
				(tanket->getLife().getTargetPosition().asVector() - 
				entry.position).Magnitude();
			if (distance < groupTargetDistance_.getValue().asFloat() * 2.0f) continue;
			
			// Find all tanks near this position			
			std::list<Tanket *>::iterator toItor;
			for (toItor = sortedTankets.begin();
				toItor != sortedTankets.end();
				++toItor)
			{
				Tanket *to = *toItor;

				distance = 
					(to->getLife().getTargetPosition().asVector() - 
					entry.position).Magnitude();
				if (distance < groupTargetDistance_.getValue().asFloat())
				{
					entry.totalDistance += distance;
					entry.targets.push_back(to);
				}
			}

			// Are there enough to warrent a shot
			if (entry.targets.size() >= (unsigned int) groupShotSize_.getValue())
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
			++itor)
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
				TankAIAimResult result;
				TankAIAimGuesser aimGuesser(ScorchedServer::instance()->getContext());
				if (aimGuesser.guess(tanket, current->position, 
					degs, 15.0f, actualPosition, result))
				{
					setWeapon(tanket, explosionhuge);
					fireShot(tanket, moveData, result);
					return true;
				}
			}
		}
	}

	return false;
}

bool TankAICurrentMove::useAvailableBatteries(Tanket *tanket, MoveData &moveData)
{
	if (!tanket->getAccessories().getBatteries().canUse()) return false;
	int noBatteries = tanket->getAccessories().getBatteries().getNoBatteries();
	if (noBatteries == 0) return false;
	if (noBatteries == -1) noBatteries = 1000;

	unsigned int batteryId = 
		tanket->getAccessories().getBatteries().getBatteryAccessory()->getAccessoryId();

	int lifeWanted = 
		tanket->getLife().getMaxLife().asInt() - 
		tanket->getLife().getLife().asInt();
	int batteriesWanted = lifeWanted / 10;

	int useBatteries = S3D_MIN(batteriesWanted, noBatteries);
	if (useBatteries <= 0) return false;

	for (int b=0; b<useBatteries; b++)
	{
		useBattery(tanket, batteryId, moveData);
	}

	return true;
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
		float offSetX = sinf(a / 180.0f * S3D_PI) * radius;
		float offSetY = cosf(a / 180.0f * S3D_PI) * radius;
		
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

float TankAICurrentMove::getShotDistance(Tanket *tanket, bool projectile)
{
	// Try to find an existing record
	std::map<Tanket *, ShotRecord>::iterator itor = 
		shotRecords_.find(tanket);
	if (itor == shotRecords_.end())
	{
		if (projectile) return projectileStartDistance_.getValue().asFloat();
		else return sniperStartDistance_.getValue().asFloat();
	}
	else
	{
		if (projectile) return itor->second.projectileCurrentDistance;
		else return itor->second.sniperCurrentDistance;
	}
}

void TankAICurrentMove::shotAtTank(Tanket *tanket, bool projectile, float newDistance)
{
	targets_.shotAt(tanket);

	// Try to find an existing record
	std::map<Tanket *, ShotRecord>::iterator itor = 
		shotRecords_.find(tanket);
	if (itor == shotRecords_.end())
	{
		// Create one
		ShotRecord record;
		record.projectileCurrentDistance = projectileStartDistance_.getValue().asFloat(); 
		record.sniperCurrentDistance = sniperStartDistance_.getValue().asFloat();
		record.position = tanket->getLife().getTargetPosition().asVector();
		shotRecords_[tanket] = record;
	}

	// Update the new record with the details about the current shot
	ShotRecord &record = shotRecords_[tanket];
	float distance = (record.position - tanket->getLife().getTargetPosition().asVector()).Magnitude();
	float distanceDec = 0.0f;
	if (distance > 5.0f)
	{
		distanceDec = S3D_MIN(distance - 5.0f, 20.0f) / 20.0f;
	}

	record.position = tanket->getLife().getTargetPosition().asVector();
	if (projectile)
	{
		record.projectileCurrentDistance = newDistance;

		float decrement = 
			projectileMinDecrement_.getValue().asFloat() +
			S3D_RAND * (projectileMaxDecrement_.getValue().asFloat() - projectileMinDecrement_.getValue().asFloat());
		record.projectileCurrentDistance = 
			S3D_MAX(projectileEndDistance_.getValue().asFloat(), record.projectileCurrentDistance - decrement);				

		distanceDec *= projectileMovementFactor_.getValue().asFloat();
		record.projectileCurrentDistance = 
			S3D_MIN(projectileStartDistance_.getValue().asFloat(), record.projectileCurrentDistance + distanceDec);	
	}
	else 
	{
		float decrement = 
			sniperMinDecrement_.getValue().asFloat() +
			S3D_RAND * (sniperMaxDecrement_.getValue().asFloat() - sniperMinDecrement_.getValue().asFloat());
		record.sniperCurrentDistance = 
			S3D_MAX(sniperEndDistance_.getValue().asFloat(), record.sniperCurrentDistance - decrement);	

		distanceDec *= sniperMovementFactor_.getValue().asFloat();
		record.sniperCurrentDistance = 
			S3D_MIN(sniperStartDistance_.getValue().asFloat(), record.sniperCurrentDistance + distanceDec);	
	}
}

void TankAICurrentMove::setWeapon(Tanket *tanket, Accessory *accessory)
{
	if (TankAI::getTankAILogging())
	{
		Logger::log(S3D::formatStringBuffer("TankAI - setting weapon %s", accessory->getName()));
	}

	// Todo check this weapon is valid
	currentWeapon_ = accessory;
}

void TankAICurrentMove::skipMove(Tanket *tanket, MoveData &moveData)
{
	if (TankAI::getTankAILogging())
	{
		Logger::log(S3D::formatStringBuffer("TankAI - skipping"));
	}

	ComsPlayedMoveMessage message(tanket->getPlayerId(), 
		moveData.moveId,
		ComsPlayedMoveMessage::eSkip);
	ScorchedServer::instance()->getServerState().moveFinished(message);
	moveData.madeMove = true;
}

void TankAICurrentMove::resign(Tanket *tanket, MoveData &moveData)
{
	if (TankAI::getTankAILogging())
	{
		Logger::log(S3D::formatStringBuffer("TankAI - resigning"));
	}

	ComsPlayedMoveMessage message(tanket->getPlayerId(), 
		moveData.moveId, 
		ComsPlayedMoveMessage::eResign);
	ScorchedServer::instance()->getServerState().moveFinished(message);
	moveData.madeMove = true;
}

void TankAICurrentMove::fireShot(Tanket *tanket, MoveData &moveData, TankAIAimResult &result)
{
	if (TankAI::getTankAILogging())
	{
		Logger::log(S3D::formatStringBuffer("TankAI - shooting at tank %s", tanket->getCStrName().c_str()));
	}

	if (currentWeapon_)
	{
		if (TankAI::getTankAILogging())
		{
			Logger::log(S3D::formatStringBuffer("TankAI - firing shot"));
		}

		ComsPlayedMoveMessage message(tanket->getPlayerId(), 
			moveData.moveId, 
			ComsPlayedMoveMessage::eShot);
		message.setShot(
			currentWeapon_->getAccessoryId(),
			result.rotation_,
			result.elevation_,
			result.power_,
			result.selectPositionX_,
			result.selectPositionY_);
	
		ScorchedServer::instance()->getServerState().moveFinished(message);
		moveData.madeMove = true;
	}
	else
	{
		if (TankAI::getTankAILogging())
		{
			Logger::log(S3D::formatStringBuffer("TankAI - skipping due to no weapon"));
		}

		skipMove(tanket, moveData);
	}
}

void TankAICurrentMove::useBattery(Tanket *tanket, unsigned int batteryId, 
	MoveData &moveData)
{
	if (TankAI::getTankAILogging())
	{
		Logger::log(S3D::formatStringBuffer("TankAI - using battery"));
	}

	ComsDefenseMessage defenseMessage(
		tanket->getPlayerId(),
		ComsDefenseMessage::eBatteryUse,
		batteryId);

	TankDefenseSimAction *simAction = new TankDefenseSimAction(defenseMessage);
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(simAction);
	moveData.usedBatteries = true;
}
