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

#include <actions/Teleport.h>
#include <actions/CameraPositionAction.h>
#include <common/Defines.h>
#include <common/OptionsScorched.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <tank/TankPosition.h>
#include <target/TargetDamageCalc.h>
#include <target/TargetLife.h>
#include <engine/ActionController.h>
#include <engine/ScorchedContext.h>
#include <weapons/AccessoryStore.h>
#include <landscapemap/DeformLandscape.h>
#include <landscapemap/LandscapeMaps.h>
#ifndef S3D_SERVER
	#include <sound/SoundUtils.h>
	#include <sprites/TeleportRenderer.h>
	#include <land/VisibilityPatchGrid.h>
#endif

Teleport::Teleport(FixedVector position,
		WeaponFireContext &weaponContext,
		WeaponTeleport *weapon) :
	Action(weaponContext.getPlayerId()),
	position_(position), 
	weaponContext_(weaponContext),
	weapon_(weapon),
	totalTime_(0),
	firstTime_(true),
	vPoint_(0)
{

}

Teleport::~Teleport()
{
	if (vPoint_) context_->getViewPoints().releaseViewPoint(vPoint_);
}

void Teleport::init()
{
	vPoint_ = context_->getViewPoints().getNewViewPoint(weaponContext_.getPlayerId());

#ifndef S3D_SERVER
	if (!context_->getServerMode())
	{
		Tank *tank = context_->getTankContainer().getTankById(weaponContext_.getPlayerId());
		if (tank && tank->getState().getState() == TankState::sNormal)
		{
			Vector white(1.0f, 1.0f, 1.0f);
			TeleportRenderer *teleport = new TeleportRenderer(
				tank->getPosition().getTankTurretPosition().asVector(),
				white);
			context_->getActionController().addAction(new SpriteAction(teleport));
		}
	}
#endif

	CameraPositionAction *pos = new CameraPositionAction(
		position_, 5, 5);
	context_->getActionController().addAction(pos);
}

void Teleport::simulate(fixed frameTime, bool &remove)
{
	if (vPoint_) vPoint_->setPosition(position_);

	if (firstTime_)
	{
		firstTime_ = false;

#ifndef S3D_SERVER
		if (!context_->getServerMode())
		{
			Tank *tank = context_->getTankContainer().getTankById(weaponContext_.getPlayerId());
			if (tank && tank->getState().getState() == TankState::sNormal)
			{
				SoundBuffer *activateSound = 
					Sound::instance()->fetchOrCreateBuffer(
						S3D::getModFile(weapon_->getSound()));
				SoundUtils::playAbsoluteSound(VirtualSoundPriority::eAction,
					activateSound, tank->getPosition().getTankPosition().asVector());
			}
		}
#endif // #ifndef S3D_SERVER
	}

	totalTime_ += frameTime;
	if (totalTime_ > weapon_->getDelay(*context_))
	{
		Tank *tank = context_->getTankContainer().getTankById(weaponContext_.getPlayerId());
		if (tank && tank->getState().getState() == TankState::sNormal)
		{
			fixed height = context_->getLandscapeMaps().getGroundMaps().getInterpHeight(
				position_[0], position_[1]);
			if (weapon_->getGroundOnly() || height >= position_[2])
			{
				// Set the position on the ground
				position_[2] = height;

				if (context_->getOptionsGame().getActionSyncCheck())
				{
					context_->getActionController().addSyncCheck(
						S3D::formatStringBuffer("Telport: %u %i, %i, %i", 
							tank->getPlayerId(),
							position_[0].getInternal(),
							position_[1].getInternal(),
							position_[2].getInternal()));
				}

				// Set this position and flatten the landscape
				tank->getLife().setTargetPosition(position_);
				DeformLandscape::flattenArea(*context_, position_);
#ifndef S3D_SERVER
				if (!context_->getServerMode())
				{
					VisibilityPatchGrid::instance()->recalculateErrors(position_, 2);
				}
#endif
			}
			else
			{
				if (context_->getOptionsGame().getActionSyncCheck())
				{
					context_->getActionController().addSyncCheck(
						S3D::formatStringBuffer("Telport: %u %i, %i, %i", 
							tank->getPlayerId(),
							position_[0].getInternal(),
							position_[1].getInternal(),
							position_[2].getInternal()));
				}

				// Set the position, what ever this is
				tank->getLife().setTargetPosition(position_);

				// Check if this tank can fall, this will result in flattening the area
				TargetDamageCalc::damageTarget(*context_, tank, weapon_, 
					weaponContext_, 0, false, true, false);
			}
		}

		remove = true;
	}

	Action::simulate(frameTime, remove);
}

std::string Teleport::getActionDetails()
{
	return S3D::formatStringBuffer("%u %i,%i,%i %s", 
		weaponContext_.getPlayerId(), 
		position_[0].getInternal(),
		position_[1].getInternal(),
		position_[2].getInternal(),
		weapon_->getParent()->getName());
}
