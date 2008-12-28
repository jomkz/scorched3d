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

#include <actions/TankFired.h>
#include <actions/Explosion.h>
#include <actions/TankSay.h>
#include <weapons/AccessoryStore.h>
#include <engine/ScorchedContext.h>
#include <engine/ActionController.h>
#ifndef S3D_SERVER
	#include <sound/SoundUtils.h>
#endif
#include <common/Defines.h>
#include <tank/TankContainer.h>
#include <tank/TankPosition.h>
#include <tankai/TankAIStrings.h>
#include <target/TargetRenderer.h>

TankFired::TankFired(unsigned int playerId,
					 Weapon *weapon,
					 fixed rotXY, fixed rotXZ) :
	ActionReferenced("TankFired"),
	playerId_(playerId), weapon_(weapon),
	rotXY_(rotXY), rotXZ_(rotXZ), firstTime_(true)
{

}

TankFired::~TankFired()
{
}

void TankFired::init()
{

}

std::string TankFired::getActionDetails()
{
	return S3D::formatStringBuffer("%u %s",
		playerId_, weapon_->getParent()->getName());
}

void TankFired::simulate(fixed frameTime, bool &remove)
{
	if (firstTime_)
	{
		firstTime_ = false;
		Tank *tank = 
			context_->getTankContainer().getTankById(playerId_);
		if (tank)
		{
			tank->getPosition().rotateGunXY(rotXY_, false);
			tank->getPosition().rotateGunYZ(rotXZ_, false);
			tank->getPosition().madeShot();

			if (tank->getDestinationId() == 0)
			{
				const char *line = TankAIStrings::instance()->getAttackLine(*context_);
				if (line)
				{
					context_->getActionController().addAction(
						new TankSay(tank->getPlayerId(), 
						LANG_STRING(line)));
				}
			}

#ifndef S3D_SERVER
			if (!context_->getServerMode()) 
			{
				TargetRenderer *renderer = tank->getRenderer();
				if (renderer)
				{
					renderer->fired();
				}

				// play fired sound
				if (weapon_->getParent()->getActivationSound() &&
					0 != strcmp("none", weapon_->getParent()->getActivationSound()))
				{
					SoundBuffer *firedSound = 
						Sound::instance()->fetchOrCreateBuffer(
							S3D::getDataFile(S3D::formatStringBuffer("data/wav/%s", 
							weapon_->getParent()->getActivationSound())));
					SoundUtils::playAbsoluteSound(VirtualSoundPriority::eAction,
						firedSound, tank->getPosition().getTankPosition().asVector());
				}
			}
#endif // #ifndef S3D_SERVER

			if (weapon_->getParent()->getMuzzleFlash())
			{
				WeaponFireContext weaponContext(playerId_, 0);
				Weapon *muzzleFlash = context_->getAccessoryStore().getMuzzelFlash();
				if (muzzleFlash) muzzleFlash->fireWeapon(*context_, weaponContext, 
					tank->getPosition().getTankGunPosition(), FixedVector::getNullVector());
			}
		}
	}

	remove = true;
	Action::simulate(frameTime, remove);
}
