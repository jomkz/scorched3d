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

#include <target/TargetDamageCalc.h>
#include <common/OptionsScorched.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <weapons/AccessoryStore.h>
#include <weapons/WeaponExplosion.h>
#include <actions/Explosion.h>
#include <actions/CameraPositionAction.h>
#include <engine/ScorchedContext.h>
#include <engine/ActionController.h>
#include <landscapemap/DeformLandscape.h>
#include <landscapemap/LandscapeMaps.h>
#include <math.h>

#ifndef S3D_SERVER
#include <client/ScorchedClient.h>
#include <client/ClientUISync.h>
#include <uiactions/UIParticleAction.h>
#endif

Explosion::Explosion(FixedVector &position,
	FixedVector &velocity,
	WeaponExplosion *weapon, WeaponFireContext &weaponContext) :
	Action(weaponContext.getInternalContext().getReferenced()),
	firstTime_(true), totalTime_(0),
	weapon_(weapon), weaponContext_(weaponContext), 
	position_(position), velocity_(velocity)
{
}

Explosion::~Explosion()
{
}

void Explosion::init()
{
	fixed multiplier = fixed(((int) context_->getOptionsGame().getWeapScale()) - 
							 OptionsGame::ScaleMedium);
	multiplier *= fixed(true, 5000);
	multiplier += 1;
	explosionSize_ = weapon_->getDeformSize(*context_) * multiplier;	

#ifndef S3D_SERVER
	if (!context_->getServerMode()) 
	{
		if (0 != strcmp(weapon_->getAccessoryTypeName(), "WeaponMuzzle")) {
			ScorchedClient::instance()->getClientUISync().addActionFromClient(
				new UIParticleAction(position_, "mp2_explosion_01"));
		}

		if (!weapon_->getNoCameraTrack())
		{
			TankViewPointProvider *vPoint = new TankViewPointProvider();
			vPoint->setValues(position_, TankViewPointProvider::defaultLookFrom, explosionSize_);
			CameraPositionAction *pos = new CameraPositionAction(
				weaponContext_.getPlayerId(),
				vPoint,
				4, 10, true);
			context_->getActionController().addAction(pos);
		}

/*
		float height = context_->getLandscapeMaps().getGroundMaps().getInterpHeight(
			position_[0], position_[1]).asFloat();
		float aboveGround = position_[2].asFloat() - height;

		// If there is a weapon play a splash sound when in water
		bool waterSplash = false;
		if (params_->getCreateSplash())
		{
			//waterSplash = 
			//	Landscape::instance()->getWater().explosion(
			//		position_.asVector(), params_->getSize().asFloat());
		}


		}

		/*
		// Make the camera shake
		std::map<std::string, TargetCamera *>::iterator itor;
		for (itor = TargetCamera::getAllTargetCameras().begin();
			itor != TargetCamera::getAllTargetCameras().end();
			++itor)
		{
			TargetCamera *targetCamera = itor->second;
			targetCamera->getCamera().addShake(params_->getShake().asFloat());
		}
		*/
	}
#endif // #ifndef S3D_SERVER
}

std::string Explosion::getActionDetails()
{
	return S3D::formatStringBuffer("%s %s", 
		position_.asQuickString(), 
		weapon_->getParent()->getName());
}

void Explosion::simulate(fixed frameTime, bool &remove)
{
	totalTime_ += frameTime;
	if (firstTime_)
	{
		firstTime_ = false;

		// Get the land height at the explosion
		fixed landHeight = context_->getLandscapeMaps().getGroundMaps().
				getInterpHeight(position_[0], position_[1]);

		// Dirt should only form along the ground
		FixedVector newPosition = position_;
		if (weapon_->getDeformType() == WeaponExplosion::DeformUp)
		{
			newPosition[2] = landHeight;
		}

		if (weapon_->getDeformType() != WeaponExplosion::DeformNone)
		{
			// Get the actual explosion size
			fixed multiplier = 
				fixed(((int) context_->getOptionsGame().getWeapScale()) - 
					OptionsGame::ScaleMedium);
			multiplier *= fixed(true, 5000);
			multiplier += 1;
			fixed deformExplosionSize = explosionSize_ * multiplier;	

			// Check if we are allowed to explode underground
			if (!weapon_->getExplodeUnderGround())
			{
				if (position_[2] - landHeight < -1)
				{
					deformExplosionSize = 0;
				}
			}

			// Remove areas from the height map
			if (deformExplosionSize > 0)
			{
				DeformLandscape::deformLandscape(
					*context_,
					newPosition, deformExplosionSize, 
					(weapon_->getDeformType() == WeaponExplosion::DeformDown), 1,
					"");
			}
		}

		fixed hurtAmount = weapon_->getHurtAmount(*context_);
		if (hurtAmount != 0 ||
			weapon_->getDeformType() != WeaponExplosion::DeformNone)
		{
			// Check the tanks for damage
			TargetDamageCalc::explosion(
				*context_,
				weapon_, weaponContext_, 
				newPosition, 
				explosionSize_, 
				hurtAmount,
				(weapon_->getDeformType() != WeaponExplosion::DeformNone),
				weapon_->getOnlyHurtShield());
		}
	}

	remove = true;
	Action::simulate(frameTime, remove);
}
