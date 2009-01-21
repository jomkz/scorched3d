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

#include <server/ServerBuyAccessoryHandler.h>
#include <server/ServerShotHolder.h>
#include <server/ServerState.h>
#include <server/ScorchedServer.h>
#include <server/TurnController.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <tank/TankAccessories.h>
#include <coms/ComsBuyAccessoryMessage.h>
#include <coms/ComsMessageSender.h>
#include <common/OptionsTransient.h>
#include <common/OptionsScorched.h>
#include <common/Logger.h>
#include <weapons/AccessoryStore.h>
#include <weapons/EconomyStore.h>

ServerBuyAccessoryHandler *ServerBuyAccessoryHandler::instance_ = 0;

ServerBuyAccessoryHandler *ServerBuyAccessoryHandler::instance()
{
	if (!instance_)
	{
		instance_ = new ServerBuyAccessoryHandler;
	}
	return instance_;
}

ServerBuyAccessoryHandler::ServerBuyAccessoryHandler()
{
	ScorchedServer::instance()->getComsMessageHandler().addHandler(
		"ComsBuyAccessoryMessage",
		this);
}

ServerBuyAccessoryHandler::~ServerBuyAccessoryHandler()
{
}

bool ServerBuyAccessoryHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	ComsBuyAccessoryMessage message;
	if (!message.readMessage(reader)) return false;
	unsigned int playerId = message.getPlayerId();

	// Check we are at the correct time to buy anything
	if (ScorchedServer::instance()->getGameState().getState() != 
		ServerState::ServerStateBuying)
	{
		Logger::log( "ERROR: Player attempted to buy accessory but in incorrect state");
		return true;
	}

	// Check we are in the correct round no to buy anything
	if (ScorchedServer::instance()->getOptionsTransient().getCurrentGameNo() != 0)
	{
		Logger::log( "ERROR: Player attempted to buy at incorrect time");
		return true;
	}

	// Check that is player still exists
	Tank *tank = ScorchedServer::instance()->getTankContainer().getTankById(playerId);
	if (!tank)
	{
		Logger::log( "ERROR: Player buying does not exist");
		return true;
	}

	if (tank->getDestinationId() != netMessage.getDestinationId())
	{
		Logger::log( "ERROR: Player buying does not exist at this destination");
		return true;
	}

	// Check this player is alive
	if (tank->getState().getState() != TankState::sNormal)
	{
		Logger::log( "ERROR: Player buying is not alive");
		return true;
	}

	// Check this player has not already given a move
	if (ServerShotHolder::instance()->haveShot(playerId))
	{
		Logger::log( "ERROR: Player buying has made move");
		return true;
	}

	if (!TurnController::instance()->playerThisTurn(playerId))
	{
		Logger::log( "ERROR: Player buying should not be buying");
		return true;		
	}

	// Check that the accessory is valid
	Accessory *accessory = 
		ScorchedServer::instance()->getAccessoryStore().
		findByAccessoryId(message.getAccessoryId());
	if (!accessory)
	{
		Logger::log(S3D::formatStringBuffer("ERROR: Player buying not-existant weapon \"%i\"", 
			message.getAccessoryId()));
		return true;
	}

	// The game state and everything is correct
	// Perform the actual add or remove of accessory
	if (message.getBuy())
	{
		if (!tank->getAccessories().accessoryAllowed(accessory, accessory->getBundle())) return true;
		if (accessory->getNoBuy()) return true;
		if (tank->getScore().getMoney() < accessory->getPrice()) return true;

		EconomyStore::instance()->getEconomy()->accessoryBought(
			tank, accessory->getName());

		// Add the accessory
		tank->getAccessories().add(accessory, accessory->getBundle());
		tank->getScore().setMoney(
			tank->getScore().getMoney() - accessory->getPrice());
	}
	else
	{
		if (tank->getAccessories().getAccessoryCount(accessory) <= 0) return true;

		EconomyStore::instance()->getEconomy()->accessorySold(
			tank, accessory->getName());

		// Remove the accessory
		tank->getAccessories().rm(accessory, 1);
		tank->getScore().setMoney(
			tank->getScore().getMoney() + accessory->getSellPrice());
	}
	return true;
}
