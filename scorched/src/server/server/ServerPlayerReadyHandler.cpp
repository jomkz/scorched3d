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


#include <server/ServerPlayerReadyHandler.h>
#include <server/ServerState.h>
#include <server/ScorchedServer.h>
#include <common/Logger.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <coms/ComsPlayerReadyMessage.h>
#include <coms/ComsMessageSender.h>

ServerPlayerReadyHandler *ServerPlayerReadyHandler::instance_ = 0;

ServerPlayerReadyHandler *ServerPlayerReadyHandler::instance()
{
	if (!instance_)
	{
		instance_ = new ServerPlayerReadyHandler;
	}
	return instance_;
}

ServerPlayerReadyHandler::ServerPlayerReadyHandler()
{
	ScorchedServer::instance()->getComsMessageHandler().addHandler(
		"ComsPlayerReadyMessage",
		this);
}

ServerPlayerReadyHandler::~ServerPlayerReadyHandler()
{
}

bool ServerPlayerReadyHandler::processMessage(NetMessage &netMessage,
	const char *messageType, NetBufferReader &reader)
{
	// Decode the connect message
	ComsPlayerReadyMessage message;
	if (!message.readMessage(reader)) return false;

	// Check this client has not tried to add a tank before
	unsigned int tankId = message.getPlayerId();
	Tank *tank = ScorchedServer::instance()->getTankContainer().getTankById(tankId);
	if (!tank)
	{
		Logger::log(S3D::formatStringBuffer("ERROR: Message from unknown tank \"%i\"", tankId));
		return false;
	}

	if (tank->getDestinationId() != netMessage.getDestinationId())
	{
		Logger::log(S3D::formatStringBuffer("ERROR: ServerPlayerReadyHandler - "
			"Message from tank at wrong destination \"%i != %i\"", 
			tank->getDestinationId(), netMessage.getDestinationId()));
		return false;
	}

	// Check the message is sent in the correct state
	if (ScorchedServer::instance()->getGameState().getState() != ServerState::ServerStateNewGameReady &&
		ScorchedServer::instance()->getGameState().getState() != ServerState::ServerStateShotReady &&
		ScorchedServer::instance()->getGameState().getState() != ServerState::ServerStateShot)
	{
		return true;
	}

	// Set this tank as ready to proceed
	// This is used by stimuli to check if all tanks are syncronised
	tank->getState().setReady();

	return true;
}
