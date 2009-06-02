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

#include <client/ScorchedClient.h>
#include <client/ClientDefenseHandler.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <tank/TankPosition.h>
#include <tank/TankAccessories.h>
#include <target/TargetLife.h>
#include <target/TargetShield.h>
#include <target/TargetParachute.h>
#include <coms/ComsDefenseMessage.h>
#include <common/Defines.h>
#include <sound/SoundUtils.h>
#include <weapons/AccessoryStore.h>

ClientDefenseHandler *ClientDefenseHandler::instance_ = 0;

ClientDefenseHandler *ClientDefenseHandler::instance()
{
	if (!instance_)
	{
		instance_ = new ClientDefenseHandler;
	}
	return instance_;
}

ClientDefenseHandler::ClientDefenseHandler()
{
	ScorchedClient::instance()->getComsMessageHandler().addHandler(
		ComsDefenseMessage::ComsDefenseMessageType,
		this);
}

ClientDefenseHandler::~ClientDefenseHandler()
{
}

bool ClientDefenseHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	// Decode the connect message
	ComsDefenseMessage message;
	if (!message.readMessage(reader)) return false;

	// Check tank exists and is alive
	Tank *tank = ScorchedClient::instance()->getTankContainer().getTankById(message.getPlayerId());
	if (!tank || tank->getState().getState() != TankState::sNormal)
	{
		return true;
	}

	// Actually perform the required action
	switch (message.getChange())
	{
	case ComsDefenseMessage::eBatteryUse:
		{
			Accessory *battery = 
				ScorchedClient::instance()->getAccessoryStore().
					findByAccessoryId(message.getInfoId());
			if (battery)
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

				if (tank->getDestinationId() != 
					ScorchedClient::instance()->getTankContainer().getCurrentDestinationId())
				{
					tank->getAccessories().add(battery, 1);
				}

				tank->getLife().setLife(tank->getLife().getLife() + 10);
				tank->getAccessories().rm(battery, 1);
			}
		}
		break;
	case ComsDefenseMessage::eShieldUp:
		{
			Accessory *accessory = 
				ScorchedClient::instance()->getAccessoryStore().
					findByAccessoryId(message.getInfoId());
			if (accessory->getType() == AccessoryPart::AccessoryShield)
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

				if (tank->getDestinationId() != 
					ScorchedClient::instance()->getTankContainer().getCurrentDestinationId())
				{
					// Make sure tank has this shield
					tank->getAccessories().add(accessory, 1);
				}
				tank->getAccessories().rm(accessory, 1);
				tank->getShield().setCurrentShield(accessory);
			}
		}
		break;
	case ComsDefenseMessage::eShieldDown:
		{
			tank->getShield().setCurrentShield(0);
		}	
		break;
	case ComsDefenseMessage::eParachutesUp:
		{
			Accessory *parachute = 
				ScorchedClient::instance()->getAccessoryStore().
					findByAccessoryId(message.getInfoId());
			if (parachute)
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

			tank->getParachute().setCurrentParachute(parachute);
		}
		break;
	case ComsDefenseMessage::eParachutesDown:
		{
			tank->getParachute().setCurrentParachute(0);
		}
		break;
	}

	return true;
}
