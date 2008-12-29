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

#include <actions/TankFalling.h>
#include <actions/TankDamage.h>
#include <target/TargetContainer.h>
#include <target/TargetState.h>
#include <target/TargetParachute.h>
#include <target/TargetDamageCalc.h>
#include <target/TargetLife.h>
#include <target/TargetSpace.h>
#include <tank/Tank.h>
#include <tank/TankAccessories.h>
#include <engine/ScorchedContext.h>
#include <engine/ActionController.h>
#include <weapons/AccessoryStore.h>
#include <weapons/Parachute.h>
#include <common/OptionsScorched.h>
#include <landscapemap/DeformLandscape.h>
#ifndef S3D_SERVER
	#include <land/VisibilityPatchGrid.h>
#endif

TankFalling::TankFalling(Weapon *weapon, unsigned int fallingPlayerId,
				   WeaponFireContext &weaponContext,
				   Parachute *parachute) :
	weapon_(weapon),
	fallingPlayerId_(fallingPlayerId),
	weaponContext_(weaponContext), parachute_(parachute)
{
}

TankFalling::~TankFalling()
{
	if (context_)
	{
		Target *target = context_->getTargetContainer().getTargetById(fallingPlayerId_);
		if (target)
		{
			if (target->getTargetState().getFalling() == this)
			{
				target->getTargetState().setFalling(0);
			}
		}
	}
}

void TankFalling::init()
{
	Target *current = 
		context_->getTargetContainer().getTargetById(fallingPlayerId_);
	if (current && 
		!current->getTargetState().getFalling() && 
		!current->getTargetState().getNoFalling())
	{
		current->getTargetState().setFalling(this);

		// Store the start positions
		tankStartPosition_ = current->getLife().getTargetPosition();

		FixedVector velocity(0, 0, 0);
		PhysicsParticleInfo info(ParticleTypeFalling, fallingPlayerId_, this);
		setPhysics(info, tankStartPosition_, velocity, 
			0, 0, 0, false);
	}
	else
	{
		collision_ = true;
	}
}

std::string TankFalling::getActionDetails()
{
	return S3D::formatStringBuffer("%u %s",
		fallingPlayerId_, weapon_->getParent()->getName());
}

void TankFalling::simulate(fixed frameTime, bool &remove)
{
	if (!collision_)
	{
		// Slow falling
		if (parachute_)	applyForce(parachute_->getSlowForce());

		// Move the tank to the new position
		Target *target = context_->getTargetContainer().getTargetById(fallingPlayerId_);
		if (target && target->getAlive())
		{
			FixedVector &position = getCurrentPosition();
			if (position[0] != 0 || position[1] != 0 || position[2] != 0)
			{
				target->getLife().setTargetPosition(position);
			}
		}
		else collision_ = true;
	}

	PhysicsParticleReferenced::simulate(frameTime, remove);
}

void TankFalling::collision(PhysicsParticleObject &position, 
	ScorchedCollisionId collisionId)
{
	Target *current = context_->getTargetContainer().getTargetById(fallingPlayerId_);
	if (current && current->getAlive())
	{
		// Find how far we have falled to get the total damage
		fixed dist = (tankStartPosition_ - position.getPosition()).Magnitude();
		fixed damage = dist * 20;

		// Check we need to cancel the damage
		fixed minDist = fixed(context_->getOptionsGame().
			getMinFallingDistance()) / 10;
		if (dist < minDist)
		{
			// No damage (or parachutes used for tiny falls)
			damage = 0;
		}
		else if (current->getTargetState().getNoFallingDamage())
		{
			damage = 0;
		}
		if (parachute_)
		{
			fixed ParachuteThreshold = 0;
			if (dist >= ParachuteThreshold)
			{
				// No damage we were using parachutes
				damage = 0;

				// Remove parachutes if we have one
				if (!current->isTarget())
				{
					Tank *currentTank = (Tank *) current;
					currentTank->getAccessories().rm(parachute_->getParent(),
						parachute_->getParent()->getUseNumber());
					if (currentTank->getAccessories().canUse(parachute_->getParent()))
					{
						current->getParachute().setCurrentParachute(0);
					}
				}
			}
		}

		if (context_->getOptionsGame().getActionSyncCheck())
		{
			context_->getActionController().addSyncCheck(
				S3D::formatStringBuffer("TankFalling: %u %i, %i, %i", 
					current->getPlayerId(),
					position.getPosition()[0].getInternal(),
					position.getPosition()[1].getInternal(),
					position.getPosition()[2].getInternal()));
		}

		// Move the tank to the final position
		current->getLife().setTargetPosition(position.getPosition());

		// Flatten the area around tanks
		if (!current->isTarget())
		{
			DeformLandscape::flattenArea(*context_, position.getPosition());
#ifndef S3D_SERVER
			if (!context_->getServerMode())
			{
				VisibilityPatchGrid::instance()->recalculateErrors(position.getPosition(), 2);
			}
#endif
		}

		// Add the damage to the tank
		TargetDamageCalc::damageTarget(
			*context_,
			current, weapon_, 
			weaponContext_, damage, 
			false, false, false);

		// Check if we have collected/given any items
		std::map<unsigned int, Target *> collisionTargets;
		context_->getTargetSpace().getCollisionSet(
			current->getLife().getTargetPosition(), 3, collisionTargets, false);
		std::map<unsigned int, Target *>::iterator itor;
		for (itor = collisionTargets.begin();
			itor != collisionTargets.end();
			itor++)
		{
			Target *collisionTarget = (*itor).second;

			if (current->isTarget() &&
				current->getTargetState().getDriveOverToDestroy() &&
				!collisionTarget->isTarget())
			{
				// Kill the falling target
				WeaponFireContext weaponContext(weaponContext_);
				weaponContext.setPlayerId(collisionTarget->getPlayerId());

				context_->getActionController().addAction(
					new TankDamage(weapon_, current->getPlayerId(), weaponContext, 
						current->getLife().getLife(),
						false, false, false));
			}
			else if (collisionTarget->isTarget() &&
				collisionTarget->getTargetState().getDriveOverToDestroy() &&
				!current->isTarget())
			{
				// Kill the target we've fallen on
				WeaponFireContext weaponContext(weaponContext_);
				weaponContext.setPlayerId(current->getPlayerId());

				context_->getActionController().addAction(
					new TankDamage(weapon_, 
					collisionTarget->getPlayerId(), weaponContext_, 
					collisionTarget->getLife().getLife(),
					false, false, false));
			}
		}
	}

	PhysicsParticleReferenced::collision(position, collisionId);
}
