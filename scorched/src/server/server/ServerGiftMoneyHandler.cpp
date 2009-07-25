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

#include <server/ServerGiftMoneyHandler.h>
#include <server/ScorchedServer.h>
#include <server/ServerShotHolder.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <engine/GameState.h>
#include <coms/ComsGiftMoneyMessage.h>
#include <coms/ComsMessageSender.h>
#include <common/OptionsTransient.h>
#include <common/Logger.h>

ServerGiftMoneyHandler *ServerGiftMoneyHandler::instance_ = 0;

ServerGiftMoneyHandler *ServerGiftMoneyHandler::instance()
{
	if (!instance_)
	{
		instance_ = new ServerGiftMoneyHandler;
	}
	return instance_;
}

ServerGiftMoneyHandler::ServerGiftMoneyHandler()
{
	ScorchedServer::instance()->getComsMessageHandler().addHandler(
		ComsGiftMoneyMessage::ComsGiftMoneyMessageType,
		this);
}

ServerGiftMoneyHandler::~ServerGiftMoneyHandler()
{
}

bool ServerGiftMoneyHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	ComsGiftMoneyMessage message;
	if (!message.readMessage(reader)) return false;

	unsigned int fromPlayerId = message.getFromPlayerId();
	unsigned int toPlayerId = message.getToPlayerId();

	// Check we are at the correct time to buy anything
	//if (ScorchedServer::instance()->getGameState().getState() != 
	//	ServerState::ServerStateBuying)
	{
		Logger::log( "ERROR: Player attempted to gift money but in incorrect state");
		return true;
	}

	// Check that is player still exists
	Tank *fromTank = ScorchedServer::instance()->
		getTankContainer().getTankById(fromPlayerId);
	if (!fromTank)
	{
		Logger::log( "ERROR: Player gifting does not exist");
		return true;
	}
	if (fromTank->getDestinationId() != netMessage.getDestinationId())
	{
		Logger::log( "ERROR: Player gifting does not exist at this destination");
		return true;
	}
	if (fromTank->getState().getState() != TankState::sNormal)
	{
		Logger::log( "ERROR: Player gifting is not alive");
		return true;
	}
	/*if (!TurnController::instance()->playerThisTurn(fromPlayerId))
	{
		Logger::log( "ERROR: Player gifting should not be buying");
		return true;		
	}*/

	// Check to player
	Tank *toTank = ScorchedServer::instance()->
		getTankContainer().getTankById(toPlayerId);
	if (!toTank)
	{
		Logger::log( "ERROR: Player gifting to does not exist");
		return true;
	}
	if (toTank->getState().getState() != TankState::sNormal &&
		toTank->getState().getState() != TankState::sDead)
	{
		Logger::log( "ERROR: Player gifting to is not alive");
		return true;
	}
	if (toTank->getTeam() != fromTank->getTeam())
	{
		Logger::log( "ERROR: Player gifting is in different teams");
		return true;
	}
	if (toTank == fromTank)
	{
		Logger::log( "ERROR: Player gifting to/from is same");
		return true;
	}

	// Check tank has required amount of money
	int money = message.getMoney();
	if (money < 0) return true;
	if (fromTank->getScore().getMoney() < money) return true;

	fromTank->getScore().setMoney(
		fromTank->getScore().getMoney() - money);
	toTank->getScore().setMoney(
		toTank->getScore().getMoney() + money);

	// Forward this message to the intended
	ComsMessageSender::sendToAllLoadedClients(message);

	return true;
}
