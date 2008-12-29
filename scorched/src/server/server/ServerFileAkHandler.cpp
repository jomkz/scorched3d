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

#include <server/ServerFileAkHandler.h>
#include <server/ScorchedServer.h>
#include <tank/TankContainer.h>
#include <tank/TankMod.h>
#include <coms/ComsFileAkMessage.h>

ServerFileAkHandler *ServerFileAkHandler::instance()
{
	static ServerFileAkHandler *instance = 
		new ServerFileAkHandler;
	return instance;
}

ServerFileAkHandler::ServerFileAkHandler()
{
	ScorchedServer::instance()->getComsMessageHandler().addHandler(
		"ComsFileAkMessage",
		this);
}

ServerFileAkHandler::~ServerFileAkHandler()
{
}

bool ServerFileAkHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	ComsFileAkMessage message;
	if (!message.readMessage(reader)) return false;

	// Set any tanks from this destination ready to recieve more input
	std::map<unsigned int, Tank *> &tanks = 
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		// For each tank
		Tank *tank = (*itor).second;
		if (netMessage.getDestinationId() == tank->getDestinationId())
		{
			tank->getMod().setReadyToReceive(true);
		}
	}

	return true;
}
