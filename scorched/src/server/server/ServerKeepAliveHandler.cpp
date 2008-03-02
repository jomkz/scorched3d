////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
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

#include <server/ServerKeepAliveHandler.h>
#include <server/ServerCommon.h>
#include <server/ScorchedServer.h>
#include <common/Logger.h>
#include <common/OptionsScorched.h>
#include <tank/TankContainer.h>
#include <coms/ComsKeepAliveMessage.h>
#include <time.h>

ServerKeepAliveHandler *ServerKeepAliveHandler::instance_ = 0;

ServerKeepAliveHandler *ServerKeepAliveHandler::instance()
{
	if (!instance_)
	{
		instance_ = new ServerKeepAliveHandler;
	}
	return instance_;
}

ServerKeepAliveHandler::ServerKeepAliveHandler()
{
	ScorchedServer::instance()->getComsMessageHandler().addHandler(
		"ComsKeepAliveMessage",
		this);
}

ServerKeepAliveHandler::~ServerKeepAliveHandler()
{
}

bool ServerKeepAliveHandler::processMessage(NetMessage &message,
	const char *messageType, NetBufferReader &reader)
{
	return true;
}

void ServerKeepAliveHandler::keepAlive(unsigned int destinationId)
{
	unsigned int theTime = (unsigned int) time(0);

	std::map<unsigned int, Tank *> &tanks =
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		Tank *current = (*itor).second;
		if (current->getDestinationId() == destinationId)
		{
			current->setKeepAlive(theTime);
		}
	}
}

void ServerKeepAliveHandler::checkKeepAlives()
{
	unsigned int allowedTime = (unsigned int)
		ScorchedServer::instance()->getOptionsGame().getKeepAliveTimeoutTime();
	if (allowedTime == 0) return;

	unsigned int theTime = (unsigned int) time(0);

	std::map<unsigned int, Tank *> &tanks =
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		Tank *current = (*itor).second;
		if (current->getDestinationId() != 0)
		{
			if (current->getKeepAlive() != 0 &&
				theTime - current->getKeepAlive()  > allowedTime)
			{
				ServerCommon::sendString(0, 
					S3D::formatStringBuffer("\"%s\" Kicked for exceeding keep alive timeout (%u seconds)",
						current->getName(),
						theTime - current->getKeepAlive()));

				ServerCommon::kickDestination(current->getDestinationId());

				// To give more time until we repeat this message
				current->setKeepAlive(0); 
				break; // As now the tank container may be out of date
			}
		}
	}
}
