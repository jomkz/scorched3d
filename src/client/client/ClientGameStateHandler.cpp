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

#include <client/ClientGameStateHandler.h>
#include <client/ScorchedClient.h>
#include <tank/TankContainer.h>
#include <engine/ActionController.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscape/Landscape.h>
#include <coms/ComsGameStateMessage.h>

ClientGameStateHandler *ClientGameStateHandler::instance_ = 0;

ClientGameStateHandler *ClientGameStateHandler::instance()
{
	if (!instance_)
	{
	  instance_ = new ClientGameStateHandler();
	}

	return instance_;
}

ClientGameStateHandler::ClientGameStateHandler()
{
	ScorchedClient::instance()->getComsMessageHandler().addHandler(
		"ComsGameStateMessage",
		this);
}

ClientGameStateHandler::~ClientGameStateHandler()
{

}

bool ClientGameStateHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	ComsGameStateMessage message;
	if (!message.readMessage(reader)) return false;

	return true;
}
