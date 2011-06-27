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

#include <simactions/TankDefenseSimAction.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <tank/TankPosition.h>
#include <tank/TankAccessories.h>
#include <weapons/AccessoryStore.h>
#include <target/TargetLife.h>
#include <target/TargetShield.h>
#include <target/TargetParachute.h>
#ifndef S3D_SERVER
#include <sound/SoundUtils.h>
#endif

REGISTER_CLASS_SOURCE(TankDefenseSimAction);

TankDefenseSimAction::TankDefenseSimAction()
{
}

TankDefenseSimAction::TankDefenseSimAction(ComsDefenseMessage &defenseMessage) :
	defenseMessage_(defenseMessage)
{
}

TankDefenseSimAction::~TankDefenseSimAction()
{
}

bool TankDefenseSimAction::invokeAction(ScorchedContext &context)
{
	unsigned int playerId = defenseMessage_.getPlayerId();

	// Check tank exists and is alive
	Tank *tank = context.getTankContainer().getTankById(playerId);
	if (!tank || !tank->getState().getTankPlaying())
	{
		return true;
	}

	// Actually perform the required action
	switch (defenseMessage_.getChange())
	{
	case ComsDefenseMessage::eBatteryUse:
		if (tank->getAccessories().getBatteries().canUse())
		{
			Accessory *battery = 
				context.getAccessoryStore().
					findByAccessoryId(defenseMessage_.getInfoId());
			if (battery)
			{
				tank->getAccessories().rm(battery, battery->getUseNumber());
				tank->getLife().setLife(tank->getLife().getLife() + 10);

#ifndef S3D_SERVER
				if (!context.getServerMode())
				{
					if (battery->getActivationSound() &&
						0 != strcmp("none", battery->getActivationSound()))
					{
						SoundBuffer *batSound = 
							Sound::instance()->fetchOrCreateBuffer(
								S3D::getModFile(S3D::formatStringBuffer("data/wav/%s", battery->getActivationSound())));
						SoundUtils::playAbsoluteSound(VirtualSoundPriority::eAction,
							batSound, tank->getPosition().getTankPosition().asVector());
					}
				}
#endif
			}
		}
		break;
	case ComsDefenseMessage::eShieldUp:
		{
			Accessory *accessory = 
				context.getAccessoryStore().
					findByAccessoryId(defenseMessage_.getInfoId());
			if (accessory->getType() == AccessoryPart::AccessoryShield)
			{
				if (tank->getAccessories().canUse(accessory))
				{
					tank->getAccessories().rm(accessory, accessory->getUseNumber());
					tank->getShield().setCurrentShield(accessory);

#ifndef S3D_SERVER
					if (!context.getServerMode())
					{
						if (accessory->getActivationSound() &&
							0 != strcmp("none", accessory->getActivationSound()))
						{
							SoundBuffer *activateSound = 
								Sound::instance()->fetchOrCreateBuffer(
									S3D::getModFile(S3D::formatStringBuffer("data/wav/%s", accessory->getActivationSound())));
							SoundUtils::playAbsoluteSound(VirtualSoundPriority::eAction,
								activateSound, tank->getPosition().getTankPosition().asVector());
						}
					}
#endif
				}
			}
		}
		break;
	case ComsDefenseMessage::eShieldDown:
		{
			Accessory *shield = tank->getShield().getCurrentShield();
			if (shield)
			{
				tank->getShield().setCurrentShield(0);
			}
		}	
		break;
	case ComsDefenseMessage::eParachutesUp:
		{
			Accessory *parachute = 
				context.getAccessoryStore().
					findByAccessoryId(defenseMessage_.getInfoId());
			if (parachute->getType() == AccessoryPart::AccessoryParachute)
			{
				if (tank->getAccessories().canUse(parachute))
				{
					tank->getParachute().setCurrentParachute(parachute);

#ifndef S3D_SERVER
					if (!context.getServerMode())
					{
						if (parachute->getActivationSound() &&
							0 != strcmp("none", parachute->getActivationSound()))
						{
							SoundBuffer *paraSound = 
								Sound::instance()->fetchOrCreateBuffer(
									S3D::getModFile(S3D::formatStringBuffer("data/wav/%s", parachute->getActivationSound())));
							SoundUtils::playAbsoluteSound(VirtualSoundPriority::eAction,
								paraSound, tank->getPosition().getTankPosition().asVector());
						}
					}
#endif
				}
			}
		}
		break;
	case ComsDefenseMessage::eParachutesDown:
		if (tank->getParachute().getCurrentParachute())
		{
			tank->getParachute().setCurrentParachute(0);
		}
		break;
	}

	return true;
}

bool TankDefenseSimAction::writeMessage(NetBuffer &buffer)
{
	return defenseMessage_.writeMessage(buffer);
}

bool TankDefenseSimAction::readMessage(NetBufferReader &reader)
{
	return defenseMessage_.readMessage(reader);
}
