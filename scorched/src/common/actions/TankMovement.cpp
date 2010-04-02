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

#include <actions/TankMovement.h>
#include <actions/TankFalling.h>
#include <actions/TankDamage.h>
#include <actions/ShotProjectile.h>
#include <actions/CameraPositionAction.h>
#include <engine/ScorchedContext.h>
#include <engine/ActionController.h>
#include <weapons/WeaponMoveTank.h>
#include <weapons/AccessoryStore.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeDefn.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapemap/MovementMap.h>
#include <landscapemap/DeformLandscape.h>
#ifndef S3D_SERVER
	#include <landscape/Smoke.h>
	#include <landscape/Landscape.h>
	#include <image/ImageStore.h>
	#include <GLEXT/GLImageModifier.h>
	#include <sound/Sound.h>
	#include <land/VisibilityPatchGrid.h>
#endif
#include <tank/TankContainer.h>
#include <tank/TankModelStore.h>
#include <tank/TankPosition.h>
#include <tank/TankState.h>
#include <tank/TankModelContainer.h>
#include <tank/TankAccessories.h>
#include <tank/TankViewPoints.h>
#include <target/TargetLife.h>
#include <target/TargetState.h>
#include <target/TargetSpace.h>
#include <image/ImageStore.h>
#include <common/OptionsScorched.h>
#include <common/Defines.h>

static const int NoMovementTransitions = 4;

TankMovement::TankMovement(WeaponFireContext &weaponContext,
	WeaponMoveTank *weapon,
	int positionX, int positionY) : 
	Action(weaponContext.getPlayerId()),
	weaponContext_(weaponContext), 
	positionX_(positionX), positionY_(positionY),
	timePassed_(0), weapon_(weapon),
	remove_(false), moving_(true), moveSoundSource_(0),
	smokeCounter_(0.1f, 0.1f), stepCount_(0),
	vPoint_(0)
{
}

TankMovement::~TankMovement()
{
#ifndef S3D_SERVER
	if (!context_->getServerMode())
	{
		delete moveSoundSource_;
		moveSoundSource_ = 0;
	}
#endif
	if (vPoint_) vPoint_->decrementReference();
}

void TankMovement::init()
{
	Tank *tank = context_->getTankContainer().getTankById(weaponContext_.getPlayerId());
	if (!tank) return;	

	tank->getTargetState().setMoving(this);

	startPosition_ = tank->getLife().getTargetPosition();

	// Start the tank movement sound
#ifndef S3D_SERVER
	if (!context_->getServerMode()) 
	{
		vPoint_ = new TankViewPointProvider();
		vPoint_->setValues(startPosition_);
		vPoint_->incrementReference();

		CameraPositionAction *positionAction = new CameraPositionAction(
			weaponContext_.getPlayerId(),
			vPoint_,
			5,
			10, 
			false);
		context_->getActionController().addAction(positionAction);

		SoundBuffer *moveSound = 
			Sound::instance()->fetchOrCreateBuffer(
				S3D::getModFile("data/wav/movement/tankmove.wav"));
		moveSoundSource_ = new VirtualSoundSource(VirtualSoundPriority::eAction, true, false);
		moveSoundSource_->setPosition(tank->getPosition().getTankPosition().asVector());
		moveSoundSource_->play(moveSound);
	}
#endif // #ifndef S3D_SERVER

	// As with everything to do with movement
	// The xy position is stored as an unsigned int
	// to save space, z is calculated from the landscape
	// Lower 32 bits = y position
	// Upper 32 bits = x positions
	std::list<unsigned int> positions;
	MovementMap mmap(
		tank, 
		*context_);
	FixedVector pos(positionX_, positionY_, 0);
	mmap.calculatePosition(pos, mmap.getFuel(weapon_));
	
	MovementMap::MovementMapEntry entry =
		mmap.getEntry(positionX_, positionY_);
	if (entry.type == MovementMap::eMovement)
	{
		// Add the end (destination) point to the list of points for the tank
		// to visit
		unsigned int pt = (positionX_ << 16) | (positionY_ & 0xffff);
		positions.push_front(pt);

		// Work backward to the source point and pre-pend them onto the
		// this of points
		while (entry.srcEntry)
		{
			pt = entry.srcEntry;
			unsigned int x = pt >> 16;
			unsigned int y = pt & 0xffff;
			positions.push_front(pt);
			entry = mmap.getEntry(x, y);
		}
	}
	
	// Expand these positions into a interpolated set of positions with
	// x, y and z
	std::list<unsigned int>::iterator itor;
	for (itor = positions.begin();
		itor != positions.end();)
	{
		unsigned int fistpt = (*itor);
		itor++;

		if (itor != positions.end())
		{
			unsigned int secpt = (*itor);

			int firstx = int(fistpt >> 16);
			int firsty = int(fistpt & 0xffff);
			int secx = int(secpt >> 16);
			int secy = int(secpt & 0xffff);
			int diffX = secx - firstx;
			int diffY = secy - firsty;
			fixed ang = (atan2x(fixed(diffY), fixed(diffX)) / fixed::XPI * 180) - 90;

			for (int i=0; i<NoMovementTransitions; i++)
			{
				fixed currentX = fixed(firstx) + fixed(diffX)/fixed(NoMovementTransitions)*fixed(i+1);
				fixed currentY = fixed(firsty) + fixed(diffY)/fixed(NoMovementTransitions)*fixed(i+1);
				expandedPositions_.push_back(
					PositionEntry(
						firstx, firsty, 
						secx, secy,
						currentX, currentY, 
						ang, (i==(NoMovementTransitions-1))));
			}
		}
	}

	// If this weapon is set to use a constant amount of fuel then use this amount
	if (weapon_->getUseFuel() > 0)
	{
		tank->getAccessories().rm(weapon_->getParent(), weapon_->getUseFuel());
	}
}

std::string TankMovement::getActionDetails()
{
	return S3D::formatStringBuffer("%u %i,%i %s",
		weaponContext_.getPlayerId(), positionX_, positionY_, 
		weapon_->getParent()->getName());
}

void TankMovement::simulate(fixed frameTime, bool &remove)
{
	if (!remove_)
	{
		if (moving_)
		{
			simulationMove(frameTime);
		}
	}
	else
	{
		remove = true;
	}

#ifndef S3D_SERVER
	if (remove && moveSoundSource_)
	{
		moveSoundSource_->stop();
	}
#endif // #ifndef S3D_SERVER
	
	Action::simulate(frameTime, remove);
}

void TankMovement::simulationMove(fixed frameTime)
{
	Tank *tank = 
		context_->getTankContainer().getTankById(weaponContext_.getPlayerId());
	if (tank)
	{
		// Stop moving if the tank is dead
		if (tank->getState().getState() == TankState::sNormal)
		{
			// Check to see if this tank is falling
			// If it is then we wait until the fall is over
			if (!tank->getTargetState().getFalling() &&
				tank->getTargetState().getMoving() == this)
			{
				// Add a smoke trail
				// Check if we are not on the server
#ifndef S3D_SERVER
				if (!context_->getServerMode())
				{
					// Check if this tank type allows smoke trails
					TankModel *model = tank->getModelContainer().getTankModel();
					if (model && model->getMovementSmoke())
					{
						if (smokeCounter_.nextDraw(frameTime.asFloat()))
						{
							Landscape::instance()->getSmoke().addSmoke(
								tank->getLife().getFloatPosition()[0],
								tank->getLife().getFloatPosition()[1],
								tank->getLife().getFloatPosition()[2]);
						}
					}
				}
#endif // S3D_SERVER

				// Move the tank one position every stepTime seconds
				// i.e. 1/stepTime positions a second
				timePassed_ += frameTime;
				fixed stepTime = weapon_->getStepTime();
				while (timePassed_ >= stepTime)
				{
					timePassed_ -= stepTime;
					if (!expandedPositions_.empty())
					{
						moveTank(tank);
					}
					else break;
				}

				if (expandedPositions_.empty()) moving_ = false;
			}
		}
		else moving_ = false;
	}
	else moving_ = false;

	if (moving_ == false)
	{
		if (tank)
		{
			if (tank->getTargetState().getMoving() == this) 
			{
				tank->getTargetState().setMoving(0);
			}
			tank->getLife().setRotation(0);
			if (tank->getState().getState() == TankState::sNormal)
			{
				// Move the tank to the final position
				DeformLandscape::flattenArea(*context_, tank->getLife().getTargetPosition());
#ifndef S3D_SERVER
				if (!context_->getServerMode())
				{
					Landscape::instance()->recalculate();
					VisibilityPatchGrid::instance()->recalculateErrors(tank->getLife().getTargetPosition(), 2);
				}
#endif
			}
		}

		remove_ = true;
	}
}

void TankMovement::moveTank(Tank *tank)
{
	fixed x = expandedPositions_.front().x;
	fixed y = expandedPositions_.front().y;
	fixed a = expandedPositions_.front().ang;
	bool useF = expandedPositions_.front().useFuel;

	int firstx = expandedPositions_.front().firstX;
	int firsty = expandedPositions_.front().firstY;
	fixed firstz = context_->getLandscapeMaps().getGroundMaps().getHeight(firstx, firsty);

	int secondx = expandedPositions_.front().secondX;
	int secondy = expandedPositions_.front().secondY;
	fixed secondz = context_->getLandscapeMaps().getGroundMaps().getHeight(secondx, secondy);
	fixed z = context_->getLandscapeMaps().getGroundMaps().getInterpHeight(x, y);
	expandedPositions_.pop_front();

	// Form the new tank position
	FixedVector newPos(x, y, z);

	// Check we are not trying to climb to high (this may be due
	// to the landscape changing after we started move)
	if (secondz - firstz > context_->getOptionsGame().getMaxClimbingDistance())
	{
		expandedPositions_.clear();
		return;
	}

	// Check to see we are not moving into water with a movement restriction
	// in place
	if (context_->getOptionsGame().getMovementRestriction() ==
		OptionsGame::MovementRestrictionLand ||
		context_->getOptionsGame().getMovementRestriction() ==
		OptionsGame::MovementRestrictionLandOrAbove)
	{
		fixed waterHeight = -10;
		LandscapeTex &tex = *context_->getLandscapeMaps().getDefinitions().getTex();
		if (tex.border->getType() == LandscapeTexType::eWater)
		{
			LandscapeTexBorderWater *water = 
				(LandscapeTexBorderWater *) tex.border;
			waterHeight = water->height;
		}

		if (context_->getOptionsGame().getMovementRestriction() ==
			OptionsGame::MovementRestrictionLandOrAbove)
		{
			if (waterHeight > startPosition_[2] - fixed(true, 1000))
			{
				waterHeight = startPosition_[2] - fixed(true, 1000);
			}
		}

		if (secondz < waterHeight)
		{
			expandedPositions_.clear();
			return;
		}
	}

	// Check this new position is allowed
	if (!MovementMap::allowedPosition(*context_, tank, newPos))
	{
		expandedPositions_.clear();
		return;
	}

	// Move the tank to this new position
	// Use up one unit of fuel
	// -1 means use 1 unit of fuel per movement square
	if (useF && (weapon_->getUseFuel() == -1))
	{
		tank->getAccessories().rm(weapon_->getParent(), 1);
	}

	// Actually move the tank
	tank->getLife().setRotation(a);
	tank->getLife().setTargetPosition(newPos);

	// Remove the targets that this tank "drives over"
	std::map<unsigned int, Target *> collisionTargets;
	context_->getTargetSpace().getCollisionSet(
		tank->getLife().getTargetPosition(), 3, collisionTargets, false);
	std::map<unsigned int, Target *>::iterator itor;
	for (itor = collisionTargets.begin();
		itor != collisionTargets.end();
		itor++)
	{
		// Check that this is a target we have driven over
		// and we can destroy it
		Target *target = (*itor).second;
		if (target->isTarget() &&
			target->getTargetState().getDriveOverToDestroy())
		{
			// Kill the target we've driven over
			context_->getActionController().addAction(
				new TankDamage(weapon_, target->getPlayerId(), weaponContext_, 
					target->getLife().getLife(),
					false, false, false));

			// Do a small explosion where we remove this target
			Accessory *accessory = 
				context_->getAccessoryStore().findByPrimaryAccessoryName("DriveOverDestroy");
			if (accessory && accessory->getType() == AccessoryPart::AccessoryWeapon)
			{
				Weapon *weapon = (Weapon *) accessory->getAction();
				weapon->fireWeapon(*context_, 
					weaponContext_,
					tank->getLife().getTargetPosition(), 
					FixedVector::getNullVector());
			}
		}
	}

	// Add tracks
#ifndef S3D_SERVER
	if (!context_->getServerMode())
	{
		stepCount_++;
		if (stepCount_ % 5 == 0)
		{
			TankModel *model = tank->getModelContainer().getTankModel();
			if (model)
			{
				Image *image = 0;
				if (firstx == secondx)
				{
					image = ImageStore::instance()->
						loadImage(model->getTracksVId());
				}
				else if (firsty == secondy)
				{
					image = ImageStore::instance()->
						loadImage(model->getTracksHId());
				}
				else if (firsty - secondy == firstx - secondx)
				{
					image = ImageStore::instance()->
						loadImage(model->getTracksVHId());
				}
				else 
				{
					image = ImageStore::instance()->
						loadImage(model->getTracksHVId());
				}

				ImageModifier::addBitmapToLandscape(
					*context_,
					*image,
					newPos[0].asFloat(), 
					newPos[1].asFloat(),
					0.04f, 0.04f,
					true);
			}
		}

		if (vPoint_) vPoint_->setValues(newPos);
	}

	if (moveSoundSource_) moveSoundSource_->setPosition(newPos.asVector());

#endif // #ifndef S3D_SERVER
}
