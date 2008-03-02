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

#include <server/ServerDefenseHandler.h>
#include <server/ServerState.h>
#include <server/ServerShotHolder.h>
#include <server/ScorchedServer.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <tank/TankAccessories.h>
#include <target/TargetLife.h>
#include <target/TargetShield.h>
#include <target/TargetParachute.h>
#include <common/Logger.h>
#include <common/OptionsScorched.h>
#include <coms/ComsDefenseMessage.h>
#include <coms/ComsMessageSender.h>
#include <weapons/AccessoryStore.h>

ServerDefenseHandler *ServerDefenseHandler::instance_ = 0;

ServerDefenseHandler *ServerDefenseHandler::instance()
{
	if (!instance_)
	{
		instance_ = new ServerDefenseHandler;
	}
	return instance_;
}

ServerDefenseHandler::ServerDefenseHandler()
{
	ScorchedServer::instance()->getComsMessageHandler().addHandler(
		"ComsDefenseMessage",
		this);
}

ServerDefenseHandler::~ServerDefenseHandler()
{
}

bool ServerDefenseHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	// Decode the connect message
	ComsDefenseMessage message;
	if (!message.readMessage(reader)) return false;

	// Get the correct player id from the message
	unsigned int playerId = message.getPlayerId();

	// Check we are in the correct state
	if ((ScorchedServer::instance()->getGameState().getState() != 
		 ServerState::ServerStatePlaying) &&
		(ScorchedServer::instance()->getGameState().getState() != 
		 ServerState::ServerStateBuying))
	{
		Logger::log("ERROR: Player attempted to use defense but in incorrect state");
		return true;
	}

	// Check tank exists and is alive
	Tank *tank = ScorchedServer::instance()->getTankContainer().getTankById(playerId);
	if (!tank || tank->getState().getState() != TankState::sNormal)
	{
		Logger::log("ERROR: Player using defense does not exist");
		return true;
	}

	if (tank->getDestinationId() != netMessage.getDestinationId())
	{
		Logger::log("ERROR: Player using defense does not exist at this destination");
		return true;
	}

	// Check tank has not made move yet
	if (ServerShotHolder::instance()->haveShot(playerId))
	{
		Logger::log("ERROR: Player has already made move");
		return true;
	}

	// Check tank can perform this defense 
	// And if so actually perform the defense
	processDefenseMessage(message, tank);

	return true;
}

void ServerDefenseHandler::processDefenseMessage(
	ComsDefenseMessage &message, Tank *tank)
{
	bool sendMessage = false;

	// Actually perform the required action
	switch (message.getChange())
	{
	case ComsDefenseMessage::eBatteryUse:
		if (tank->getAccessories().getBatteries().getNoBatteries() != 0)
		{
			Accessory *battery = 
				ScorchedServer::instance()->getAccessoryStore().
					findByAccessoryId(message.getInfoId());
			if (battery)
			{
				tank->getAccessories().rm(battery);
				tank->getLife().setLife(tank->getLife().getLife() + 10);
				sendMessage = true;
			}
		}
		break;
	case ComsDefenseMessage::eShieldUp:
		{
			Accessory *accessory = 
				ScorchedServer::instance()->getContext().accessoryStore->
					findByAccessoryId(message.getInfoId());
			if (accessory->getType() == AccessoryPart::AccessoryShield)
			{
				if (tank->getAccessories().getAccessoryCount(accessory) != 0)
				{
					tank->getAccessories().rm(accessory, 1);
					tank->getShield().setCurrentShield(accessory);
					sendMessage = true;
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
				sendMessage = true;
			}
		}	
		break;
	case ComsDefenseMessage::eParachutesUp:
		{
			Accessory *accessory = 
				ScorchedServer::instance()->getContext().accessoryStore->
					findByAccessoryId(message.getInfoId());
			if (accessory->getType() == AccessoryPart::AccessoryParachute)
			{
				if (tank->getAccessories().getAccessoryCount(accessory) != 0)
				{
					tank->getParachute().setCurrentParachute(accessory);
					sendMessage = true;
				}
			}
		}
		break;
	case ComsDefenseMessage::eParachutesDown:
		if (tank->getParachute().getCurrentParachute())
		{
			tank->getParachute().setCurrentParachute(0);
			sendMessage = true;
		}
		break;
	}

	if (sendMessage)
	{
			if (ScorchedServer::instance()->getOptionsGame().getDelayedDefenseActivation())
			{
				ComsMessageSender::sendToSingleClient(message, tank->getDestinationId());
			}
			else
			{
				ComsMessageSender::sendToAllPlayingClients(message);
			}
	}
}
