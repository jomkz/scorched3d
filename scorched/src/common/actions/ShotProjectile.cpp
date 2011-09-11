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

#include <actions/ShotProjectile.h>
#include <actions/CameraPositionAction.h>
#ifndef S3D_SERVER
	#include <sprites/MissileActionRenderer.h>
	#include <tankgraph/RenderTracer.h>
#endif
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeTex.h>
#include <tank/TankViewPoints.h>
#include <target/TargetContainer.h>
#include <tankai/TankAI.h>
#include <tanket/Tanket.h>
#include <common/Defines.h>
#include <common/OptionsScorched.h>
#include <engine/ScorchedContext.h>
#include <engine/ActionController.h>
#include <weapons/AccessoryStore.h>
#include <math.h>

ShotProjectile::ShotProjectile(FixedVector &startPosition, FixedVector &velocity,
							   WeaponProjectile *weapon, WeaponFireContext &weaponContext,
							   unsigned int flareType,
							   fixed spinSpeed, const Vector &spinAxis) :
	PhysicsParticle(weaponContext.getPlayerId()),
	startPosition_(startPosition), velocity_(velocity), 
	weapon_(weapon), weaponContext_(weaponContext), 
	flareType_(flareType), vPoint_(0),
	snapTime_(fixed(true, 2000)), up_(false),
	totalTime_(0), simulateTime_(0), 
	spinSpeed_(spinSpeed), spinAxis_(spinAxis)
{
}

void ShotProjectile::init()
{
#ifndef S3D_SERVER
	if (!context_->getServerMode()) 
	{
		setActionRender(new MissileActionRenderer(flareType_, 
				weapon_->getScale(*context_).asFloat(),
				spinSpeed_.asFloat(),
				spinAxis_));

		if (!weapon_->getNoCameraTrack())
		{
			vPoint_ = new TankViewPointProvider();
			vPoint_->incrementReference();
			FixedVector velocity = velocity_;
			velocity[2] = 10;
			vPoint_->setValues(startPosition_, velocity);
		
			CameraPositionAction *positionAction = new CameraPositionAction(
				weaponContext_.getPlayerId(),
				vPoint_,
				5,
				10,
				false);
			context_->getActionController().addAction(positionAction);
		}
	}
#endif // #ifndef S3D_SERVER

	PhysicsParticleInfo info(ParticleTypeShot, weaponContext_.getPlayerId(), this);
	setPhysics(info, startPosition_, velocity_, 
		0, 0, weapon_->getWindFactor(*context_), weapon_->getGravityFactor(*context_), 
		getWeapon()->getUnder(), 
		false, getWeapon()->getWallCollision());
	thrustTime_ = getWeapon()->getThrustTime(*context_);
	thrustAmount_ = getWeapon()->getThrustAmount(*context_);
	timedCollision_ = getWeapon()->getTimedCollision(*context_);
	heightCollision_ = getWeapon()->getHeightCollision(*context_);
	drag_ = getWeapon()->getDrag(*context_);
	stepSize_ = getWeapon()->getStepSize() * 
		fixed(true, context_->getOptionsGame().getWeaponSpeed());
}

std::string ShotProjectile::getActionDetails()
{
	return S3D::formatStringBuffer("%s %s %s",
		startPosition_.asQuickString(),
		velocity_.asQuickString(),
		weapon_->getParent()->getName());
}

ShotProjectile::~ShotProjectile()
{
	if (vPoint_) vPoint_->decrementReference();
}

void ShotProjectile::collision(PhysicsParticleObject &position, 
	ScorchedCollisionId collisionId)
{
	if (!collision_)
	{
		// Tell all AIs about this collision
		std::map<unsigned int, Tanket *> &tanks = 
			context_->getTargetContainer().getTankets();
		std::map<unsigned int, Tanket *>::iterator itor;
		for (itor = tanks.begin();
			itor != tanks.end();
			++itor)
		{
			Tanket *tanket = (*itor).second;
			TankAI *ai = tanket->getTankAI();
			if (ai)
			{		
				if (tanket->getAlive())
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
	PhysicsParticle::collision(position, collisionId);
}

void ShotProjectile::simulate(fixed frameTime, bool &remove)
{
	totalTime_ += frameTime;

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

	// Apex collision
	if (!remove &&
		heightCollision_ > 0)
	{
		if (getCurrentVelocity()[2] < 0)
		{
			fixed landHeight = context_->getLandscapeMaps().
				getGroundMaps().getInterpHeight(
					getCurrentPosition()[0], getCurrentPosition()[1]);
			if (getCurrentPosition()[2] <= heightCollision_ + landHeight)
			{
				doCollision(getCurrentPosition());
				remove = true;
			}
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

	simulateTime_ += frameTime;
	while (simulateTime_ > stepSize_)
	{
		PhysicsParticle::simulate(stepSize_, remove);
		simulateTime_ -= stepSize_;
	}

	if (vPoint_)
	{
		FixedVector velocity = -getCurrentVelocity();
		velocity[2] = 10;
		vPoint_->setValues(getCurrentPosition(), velocity);
	}
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

