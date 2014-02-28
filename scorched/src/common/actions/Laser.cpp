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

#include <actions/Laser.h>
#include <engine/ScorchedContext.h>
#include <engine/ActionController.h>
#include <weapons/AccessoryStore.h>
#include <weapons/Shield.h>
#include <actions/ShieldHit.h>
#include <target/TargetDamage.h>
#include <target/TargetContainer.h>
#include <target/TargetShield.h>
#include <target/TargetLife.h>
#include <target/TargetSpace.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <math.h>
#include <set>

Laser::Laser(WeaponLaser *weapon, 
		FixedVector &position, FixedVector &direction,
		WeaponFireContext &weaponContext) :
	Action(weaponContext.getInternalContext().getReferenced()),
	totalTime_(0),
	drawLength_(0),
	firstTime_(true),
	weaponContext_(weaponContext), 
	weapon_(weapon),
	position_(position), 
	direction_(direction)
{
}

Laser::~Laser()
{
}

void Laser::init()
{
	directionMagnitude_ = direction_.Magnitude();
	fixed per = directionMagnitude_ / 50;
	length_ = weapon_->getMinimumDistance(*context_) + 
		(weapon_->getMaximumDistance(*context_) - weapon_->getMinimumDistance(*context_)) * per;
	damage_ = weapon_->getMinimumHurt(*context_) + 
		(weapon_->getMaximumHurt(*context_) - weapon_->getMinimumHurt(*context_)) * (fixed(1) - per);

	FixedVector dir = direction_.Normalize();

	angXY_ = 180.0f - atan2f(dir[0].asFloat(), dir[1].asFloat()) / 3.14f * 180.0f;
	angYZ_ = acosf(dir[2].asFloat()) / 3.14f * 180.0f;

	// preset some values from the numberparser expressions
	laserTime_ = weapon_->getTotalTime(*context_);
	hurtRadius_ = weapon_->getHurtRadius(*context_);
}

std::string Laser::getActionDetails()
{
	return S3D::formatStringBuffer("%s %s %s",
		position_.asQuickString().c_str(),
		direction_.asQuickString().c_str(),
		weapon_->getParent()->getName());
}

void Laser::simulate(fixed frameTime, bool &remove)
{
	if (firstTime_)
	{
		firstTime_ = false;
		
		if (damage_ > 0 && directionMagnitude_ > 0)
		{
			std::set<unsigned int> damagedTargets_;

			// Build a set of all tanks in the path of the laser
			FixedVector pos = position_;
			FixedVector dir = direction_.Normalize() / 4;
			bool end = false;
			while (!end)
			{
				std::map<unsigned int, Target *> collisionTargets;
				context_->getTargetSpace().getCollisionSet(pos, 
					fixed(hurtRadius_), collisionTargets);
				std::map<unsigned int, Target *>::iterator itor;
				for (itor = collisionTargets.begin();
					itor != collisionTargets.end();
					++itor)
				{
					Target *current = (*itor).second;
					if (current->getAlive() &&
						((current->getPlayerId() != weaponContext_.getPlayerId()) ||
						weapon_->getHurtFirer()))
					{
						Shield::ShieldLaserProofType laserProof = Shield::ShieldLaserProofNone;
						if (current->getShield().getCurrentShield())
						{
							Shield *shield = (Shield *)
								current->getShield().getCurrentShield()->getAction();
							if (shield->getLaserProof() != Shield::ShieldLaserProofNone)
							{
								laserProof = shield->getLaserProof();
								FixedVector offset = current->getLife().getTargetPosition() - pos;
								if (shield->inShield(offset))
								{
									context_->getActionController().addAction(
										new ShieldHit(current->getPlayerId(), pos, 0));

									end = true;
									break;
								}
							}
						}

						if (laserProof != Shield::ShieldLaserProofTotal)
						{
							FixedVector offset = current->getLife().getTargetPosition() - pos;
							fixed targetDistance = offset.Magnitude();

							if (targetDistance < hurtRadius_ + 
								S3D_MAX(current->getLife().getSize()[0], current->getLife().getSize()[1]))
							{
								damagedTargets_.insert(current->getPlayerId());
							}
						}
					}
				}

				if (!end)
				{
					pos += dir;
					drawLength_ = (pos - position_).Magnitude();
					if (drawLength_ > length_) end = true;
				}
			}

			// Subtract set amount from all tanks
			std::set<unsigned int>::iterator itor;
			for (itor = damagedTargets_.begin();
				itor != damagedTargets_.end();
				++itor)
			{
				unsigned int damagedTarget = (*itor);
				TargetDamage::damageTarget(*context_,
						weapon_, damagedTarget, weaponContext_,
						damage_, false, false, false);
			}
		}
	}

	totalTime_ += frameTime;

	remove = (totalTime_ > laserTime_);
	Action::simulate(frameTime, remove);
}
