////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
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

#include <actions/ShieldHit.h>
#include <common/Defines.h>
#include <engine/ScorchedContext.h>
#include <weapons/Accessory.h>
#include <weapons/Shield.h>
#include <target/TargetContainer.h>
#include <target/TargetShield.h>
#ifndef S3D_SERVER
	#include <sound/SoundUtils.h>
	#include <GLEXT/GLLenseFlare.h>
	#include <tankgraph/TargetRendererImplTank.h>
#endif

ShieldHit::ShieldHit(unsigned int playerId,
	FixedVector &position,
	fixed hitPercentage) :
	ActionReferenced("ShieldHit"),
	totalTime_(0),
	firstTime_(true), playerId_(playerId),
	hitPercentage_(hitPercentage),
	position_(position)
{
}

ShieldHit::~ShieldHit()
{
}

void ShieldHit::init()
{
}

void ShieldHit::simulate(fixed frameTime, bool &remove)
{
	if (firstTime_)
	{
		firstTime_ = false;

		Target *target = 
			context_->targetContainer->getTargetById(playerId_);
		if (target)
		{
			Accessory *accessory = 
				target->getShield().getCurrentShield();
			if (accessory)
			{
				Shield *shield = (Shield *) accessory->getAction();
#ifndef S3D_SERVER
				if (!context_->serverMode) 
				{
					SoundBuffer *shieldSound = 
						Sound::instance()->fetchOrCreateBuffer(
							S3D::getDataFile(S3D::formatStringBuffer("data/wav/%s", shield->getCollisionSound())));
					SoundUtils::playAbsoluteSound(VirtualSoundPriority::eAction,
						shieldSound, position_.asVector());

					TargetRenderer *renderer = target->getRenderer();
					if (renderer)
					{
						renderer->shieldHit();
					}
				}
#endif // #ifndef S3D_SERVER

				target->getShield().setShieldPower(
					target->getShield().getShieldPower() -
					shield->getHitRemovePower() * hitPercentage_);
			}
		}
	}

	totalTime_ += frameTime;
	if (totalTime_ > fixed(true, 2000)) remove = true;
	Action::simulate(frameTime, remove);
}

void ShieldHit::draw()
{
#ifndef S3D_SERVER
	if (!context_->serverMode)
	{
		GLLenseFlare::instance()->draw(position_.asVector(), false, 0, 
			1.0f, 1.0f);
	}
#endif // #ifndef S3D_SERVER
}
