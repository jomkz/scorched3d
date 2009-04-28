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

#include <actions/ShotProjectile.h>
#ifndef S3D_SERVER
	#include <sprites/MissileActionRenderer.h>
	#include <tankgraph/RenderTracer.h>
#endif
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeTex.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <tankai/TankAI.h>
#include <common/Defines.h>
#include <engine/ScorchedContext.h>
#include <engine/ViewPoints.h>
#include <weapons/AccessoryStore.h>
#include <math.h>

ShotProjectile::ShotProjectile(FixedVector &startPosition, FixedVector &velocity,
							   WeaponProjectile *weapon, WeaponFireContext &weaponContext,
							   unsigned int flareType,
							   fixed spinSpeed ) :
	startPosition_(startPosition), velocity_(velocity), 
	weapon_(weapon), weaponContext_(weaponContext), 
	flareType_(flareType), vPoint_(0),
	snapTime_(fixed(true, 2000)), up_(false),
	totalTime_(0), spinSpeed_(spinSpeed)
{

}

void ShotProjectile::init()
{
#ifndef S3D_SERVER
	if (!context_->getServerMode()) 
	{
		setActionRender(new MissileActionRenderer(flareType_, 
				weapon_->getScale(*context_).asFloat(),
				spinSpeed_.asFloat()));
	}
#endif // #ifndef S3D_SERVER

	vPoint_ = context_->getViewPoints().getNewViewPoint(weaponContext_.getPlayerId());
	PhysicsParticleInfo info(ParticleTypeShot, weaponContext_.getPlayerId(), this);
	setPhysics(info, startPosition_, velocity_, 
		0, 0, weapon_->getWindFactor(*context_), getWeapon()->getUnder(), 
		false, getWeapon()->getWallCollision());
	thrustTime_ = getWeapon()->getThrustTime(*context_);
	thrustAmount_ = getWeapon()->getThrustAmount(*context_);
	timedCollision_ = getWeapon()->getTimedCollision(*context_);
	drag_ = getWeapon()->getDrag(*context_);
}

std::string ShotProjectile::getActionDetails()
{
	return S3D::formatStringBuffer("%i,%i,%i %i,%i,%i %s",
		startPosition_[0].getInternal(), startPosition_[1].getInternal(), startPosition_[2].getInternal(),
		velocity_[0].getInternal(), velocity_[1].getInternal(), velocity_[2].getInternal(),
		weapon_->getParent()->getName());
}

ShotProjectile::~ShotProjectile()
{
	if (vPoint_) context_->getViewPoints().releaseViewPoint(vPoint_);
}

void ShotProjectile::collision(PhysicsParticleObject &position, 
	ScorchedCollisionId collisionId)
{
	if (!collision_)
	{
		// Tell all AIs about this collision
		std::map<unsigned int, Tank *> tanks = 
			context_->getTankContainer().getAllTanks();
		std::map<unsigned int, Tank *>::iterator itor;
		for (itor = tanks.begin();
			itor != tanks.end();
			itor++)
		{
			Tank *tank = (*itor).second;
			TankAI *ai = tank->getTankAI();
			if (ai)
			{		
				if (tank->getState().getState() == TankState::sNormal)
				{
					ai->shotLanded(collisionId, 
						getWeapon(), getPlayerId(), 
						getCurrentPosition().asVector());
				}
			}
		}

		bool doColl = true;

		// Apex collisions dud if they collide with the ground
		// unless no dud is set
		if (getWeapon()->getApexCollision() && !getWeapon()->getApexNoDud())
		{
			doColl = false;
		}
		if ((getWeapon()->getTimedCollision(*context_) > 0) && getWeapon()->getTimedDud())
		{
			doColl = false;
		}

		if (doColl) doCollision(position.getPosition());
	}
	PhysicsParticleReferenced::collision(position, collisionId);
}

void ShotProjectile::simulate(fixed frameTime, bool &remove)
{
	totalTime_ += frameTime;
	if (vPoint_)
	{
		vPoint_->setPosition(getCurrentPosition());

		FixedVector velocity = -getCurrentVelocity();
		velocity[2] = 10;
		vPoint_->setLookFrom(velocity);
	}

	// Water collision
	if (!remove &&
		getWeapon()->getWaterCollision())
	{
		fixed waterHeight = -10;
		LandscapeTex &tex = *context_->getLandscapeMaps().getDefinitions().getTex();
		if (tex.border->getType() == LandscapeTexType::eWater)
		{
			LandscapeTexBorderWater *water = 
				(LandscapeTexBorderWater *) tex.border;

			waterHeight = water->height;
		}

		if (getCurrentPosition()[2] < waterHeight)
		{
			doCollision(getCurrentPosition());
			remove = true;
		}
	}

	// Apex collision
	if (!remove &&
		getWeapon()->getApexCollision())
	{
		if (getCurrentVelocity()[2] > 0) up_ = true;
		else if (up_)
		{
			doCollision(getCurrentPosition());
			remove = true;
		}
	}

	// Thrust
	if (thrustAmount_ > 0)
	{
		if (totalTime_ < thrustTime_ ||
			thrustTime_ == 0)
		{
			FixedVector direction = getCurrentVelocity();
			direction.StoreNormalize();
			direction *= thrustAmount_;
			applyForce(direction);
		}
	}

	// Drag
	if (drag_ > 0)
	{
		FixedVector direction = getCurrentVelocity();
		direction *= -drag_;
		applyForce(direction);
	}

	// Timed collision
	if (!remove &&
		timedCollision_ > 0)
	{
		if (totalTime_ > timedCollision_)
		{
			doCollision(getCurrentPosition());
			remove = true;
		}
	}

	// Shot path
#ifndef S3D_SERVER
	if (!context_->getServerMode())
	{
		if (getWeapon()->getShowShotPath())
		{
			snapTime_ += frameTime;
			if (snapTime_.asFloat() > 0.1f || remove)
			{
				Vector up (0.0f, 0.0f, 1.0f);
				RenderTracer::TracerLinePoint point;
				point.position = getCurrentPosition().asVector();
				point.cross = (getCurrentVelocity().asVector() * up).Normalize();
				positions_.push_back(point);
				snapTime_ = 0;
			}
		}
	}
#endif	// #ifndef S3D_SERVER

	PhysicsParticleReferenced::simulate(frameTime, remove);
}

void ShotProjectile::doCollision(FixedVector &position)
{	
#ifndef S3D_SERVER
	if (!context_->getServerMode())
	{
		if (getWeapon()->getShowShotPath())
		{
			RenderTracer::instance()->
				addSmokeTracer(weaponContext_.getPlayerId(), 
					position.asVector(), positions_);
		}
		else if (getWeapon()->getShowEndPoint())
		{
			RenderTracer::instance()->
				addTracer(weaponContext_.getPlayerId(), position.asVector());
		}
	}
#endif // #ifndef S3D_SERVER

	FixedVector velocity;
	getWeapon()->getCollisionAction()->fireWeapon(
		*context_, weaponContext_, position, getCurrentVelocity());
}

