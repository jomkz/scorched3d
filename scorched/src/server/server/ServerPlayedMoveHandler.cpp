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

#include <server/ServerPlayedMoveHandler.h>
#include <server/ServerShotHolder.h>
#include <server/ServerState.h>
#include <server/ScorchedServer.h>
#include <server/TurnController.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <engine/GameState.h>
#include <coms/ComsPlayedMoveMessage.h>

ServerPlayedMoveHandler *ServerPlayedMoveHandler::instance_ = 0;

ServerPlayedMoveHandler *ServerPlayedMoveHandler::instance()
{
	if (!instance_)
	{
		instance_ = new ServerPlayedMoveHandler;
	}
	return instance_;
}

ServerPlayedMoveHandler::ServerPlayedMoveHandler()
{
	ScorchedServer::instance()->getComsMessageHandler().addHandler(
		ComsPlayedMoveMessage::ComsPlayedMoveMessageType,
		this);
}

ServerPlayedMoveHandler::~ServerPlayedMoveHandler()
{
}

bool ServerPlayedMoveHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	ComsPlayedMoveMessage *message = new ComsPlayedMoveMessage;
	if (!message->readMessage(reader))
	{
		delete message;
		return false;
	}

	unsigned int playerId = message->getPlayerId();
	Tank *tank = ScorchedServer::instance()->getTankContainer().getTankById(playerId);
	if (tank && tank->getState().getState() == TankState::sNormal)
	{	
		if (tank->getDestinationId() == netMessage.getDestinationId())
		{
			if (TurnController::instance()->playerThisTurn(playerId))
			{
				//if ((ScorchedServer::instance()->getGameState().getState() == 
				//	ServerState::ServerStatePlaying) || ((
				//	ScorchedServer::instance()->getGameState().getState() == 
				//	ServerState::ServerStateBuying) && 
				//	message->getType() == ComsPlayedMoveMessage::eFinishedBuy))
				{
					ServerShotHolder::instance()->addShot(playerId, message);
				}
				//else delete message;
			}
			else delete message;
		}
		else delete message;
	}
	else delete message;

	return true;
}
