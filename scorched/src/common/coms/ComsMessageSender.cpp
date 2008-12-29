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

#include <coms/ComsMessageSender.h>
#include <coms/ComsMessageHandler.h>
#include <net/NetBuffer.h>
#include <net/NetInterface.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#ifndef S3D_SERVER
#include <client/ScorchedClient.h>
#endif
#include <server/ScorchedServer.h>
#include <set>
#include <zlib.h>

static NetBuffer defaultBuffer;

bool ComsMessageSender::formMessage(ComsMessage &message)
{
	// Write the message and its type to the buffer
	defaultBuffer.reset();
	if (!message.writeTypeMessage(defaultBuffer))
	{
		Logger::log( "ERROR: ComsMessageSender - Failed to write message type");
		return false;
	}
	if (!message.writeMessage(defaultBuffer))
	{
		Logger::log( "ERROR: ComsMessageSender - Failed to write message");
		return false;
	}

	// Compress the message
	defaultBuffer.compressBuffer();

	return true;
}

#ifndef S3D_SERVER
bool ComsMessageSender::sendToServer(
	ComsMessage &message, unsigned int flags)
{
	if (!ScorchedClient::instance()->getNetInterfaceValid() ||
		!ScorchedClient::instance()->getNetInterface().started()) return false;
	if (!formMessage(message)) return false;

	if (ScorchedClient::instance()->getComsMessageHandler().getMessageLogging())
	{
		Logger::log(S3D::formatStringBuffer("Client::send(%s, %u)", 
			message.getMessageType(),
			defaultBuffer.getBufferUsed()));
	}	
	ScorchedClient::instance()->getNetInterface().sendMessageServer(
		defaultBuffer, flags);
	return true;
}
#endif

bool ComsMessageSender::sendToMultipleClients(
	ComsMessage &message, std::list<unsigned int> sendDestinations, unsigned int flags)
{
	if (sendDestinations.empty()) return true;
	if (!formMessage(message)) return false;

	// Used to ensure we only send messages to each
	// destination once
	std::set<unsigned int> destinations;
	std::set<unsigned int>::iterator findItor;
	destinations.insert(0); // Make sure we don't send to dest 0

	std::list<unsigned int>::iterator itor;
	for (itor = sendDestinations.begin();
		itor != sendDestinations.end();
		itor++)
	{
		unsigned int destination = *itor;
		findItor = destinations.find(destination);

		if (findItor == destinations.end())
		{
			destinations.insert(destination);

			if (ScorchedServer::instance()->getComsMessageHandler().getMessageLogging())
			{
				Logger::log(S3D::formatStringBuffer("Server::send(%s, %u, %u)", 
					message.getMessageType(),
					destination,
					defaultBuffer.getBufferUsed()));
			}	
			if (!ScorchedServer::instance()->getNetInterfaceValid() ||
				!ScorchedServer::instance()->getNetInterface().started())
			{
				Logger::log( "ERROR: ComsMessageSender::sendToMultipleClients - Server not started");
				return false;
			}
			ScorchedServer::instance()->getNetInterface().sendMessageDest(
				defaultBuffer, destination, flags);
		}
	}

	return true;
}

bool ComsMessageSender::sendToSingleClient(ComsMessage &message,
	unsigned int destination, unsigned int flags)
{
	if (destination == 0) return true;

	std::list<unsigned int> destinations;
	destinations.push_back(destination);
	return sendToMultipleClients(message, destinations, flags);
}

bool ComsMessageSender::sendToAllConnectedClients(
	ComsMessage &message, unsigned int flags)
{
	std::list<unsigned int> destinations;
	std::map<unsigned int, Tank *>::iterator itor;
	std::map<unsigned int, Tank *> tanks = 
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		Tank *tank = (*itor).second;
		destinations.push_back(tank->getDestinationId());
	}
	return sendToMultipleClients(message, destinations, flags);
}

bool ComsMessageSender::sendToAllPlayingClients(
	ComsMessage &message, unsigned int flags)
{
	std::list<unsigned int> destinations;
	std::map<unsigned int, Tank *>::iterator itor;
	std::map<unsigned int, Tank *> tanks = 
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		Tank *tank = (*itor).second;
		if (tank->getState().getState() != TankState::sPending &&
			tank->getState().getState() != TankState::sLoading &&
			tank->getState().getState() != TankState::sInitializing)
		{
			destinations.push_back(tank->getDestinationId());
		}
	}
	return sendToMultipleClients(message, destinations, flags);
}
