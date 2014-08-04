////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2013
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
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
////////////////////////////////////////////////////////////////////////////////

#include <server/ServerLoadLevel.h>
#include <server/ScorchedServer.h>
#include <server/ServerSimulator.h>
#include <server/ServerDestinations.h>
#include <landscapedef/LandscapeDescription.h>
#include <landscapemap/LandscapeMaps.h>
#include <engine/Simulator.h>
#include <simactions/DestinationLoadedServerSimAction.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <coms/ComsMessageSender.h>
#include <coms/ComsLoadLevelMessage.h>
#include <coms/ComsLevelLoadedMessage.h>
#include <common/Logger.h>

ServerLoadLevel::ServerLoadLevel(ComsMessageHandler &comsMessageHandler)
{
	comsMessageHandler.addHandler(
		ComsLevelLoadedMessage::ComsLevelLoadedMessageType,
		this);
}

ServerLoadLevel::~ServerLoadLevel()
{
}

void ServerLoadLevel::destinationLoadLevel(unsigned int destinationId)
{
	if (destinationId == 0)
	{
		setLoaded(destinationId);
	}
	else
	{
		LandscapeDescription &landscapeDescription =
			ScorchedServer::instance()->getLandscapeMaps().getDescriptions().
			getDescription();
		if (landscapeDescription.getDescriptionNumber() != 0)
		{
			// Set any tanks from this destination that they are loading the level
			// set the current version of the level (definition number) that they are loading
			// so we can check if the level changes before we have finished loading
			ServerDestination *destination =
				ScorchedServer::instance()->getServerDestinations().getDestination(destinationId);
			if (destination)
			{
				destination->setState(ServerDestination::sLoadingLevel);
				destination->setLevelNumber(landscapeDescription.getDescriptionNumber());
			}

			// Tell this destination to start loading the level
			ComsLoadLevelMessage &loadLevelMessage = 
				ScorchedServer::instance()->getServerSimulator().getLevelMessage();
			ComsMessageSender::sendToSingleClient(loadLevelMessage, destinationId, NetInterfaceFlags::fCompress);
		}
	}
}

bool ServerLoadLevel::destinationUsingCurrentLevel(unsigned int destinationId)
{
	LandscapeDescription &landscapeDescription =
		ScorchedServer::instance()->getLandscapeMaps().getDescriptions().
		getDescription();

	ServerDestination *destination =
		ScorchedServer::instance()->getServerDestinations().getDestination(destinationId);
	if (destination)
	{
		return (destination->getLevelNumber() == landscapeDescription.getDescriptionNumber());
	}
	return false;
}

bool ServerLoadLevel::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	unsigned int destinationId = netMessage.getDestinationId();

	// Check that the destination is still using the correct level
	// the server may have changed level during the time taken for the 
	// client to load the level
	if (!destinationUsingCurrentLevel(destinationId)) 
	{
		destinationLoadLevel(destinationId);
		return true;
	}

	// This destination has finished loading
	ServerDestination *destination =
		ScorchedServer::instance()->getServerDestinations().getDestination(destinationId);
	if (destination)
	{
		destination->setState(ServerDestination::sFinished);
	}

	setLoaded(destinationId);

	return true;
}

void ServerLoadLevel::setLoaded(unsigned int destinationId)
{
	ScorchedServer::instance()->getServerSimulator().
		addSimulatorAction(new DestinationLoadedServerSimAction(destinationId));
}
