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

#include <simactions/DestinationLoadedServerSimAction.h>
#include <simactions/DestinationLoadedClientSimAction.h>
#include <simactions/TankLoadedSimAction.h>
#include <common/Logger.h>
#include <common/OptionsTransient.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <engine/Simulator.h>
#include <server/ScorchedServer.h>
#include <server/ServerDestinations.h>
#include <server/ServerSimulator.h>

REGISTER_CLASS_SOURCE(DestinationLoadedServerSimAction);

DestinationLoadedServerSimAction::DestinationLoadedServerSimAction() :
	destinationId_(0)
{
}

DestinationLoadedServerSimAction::DestinationLoadedServerSimAction(unsigned int destinationId) :
	destinationId_(destinationId)
{
}

DestinationLoadedServerSimAction::~DestinationLoadedServerSimAction()
{
}

bool DestinationLoadedServerSimAction::invokeAction(ScorchedContext &context)
{
	if (context.getServerMode())
	{
		setLoaded(context);
	}

	return true;
}

void DestinationLoadedServerSimAction::setLoaded(ScorchedContext &context)
{
	// These tanks are now ready to play
	std::map<unsigned int, Tank *>::iterator itor;
	std::map<unsigned int, Tank *> &tanks = 
		context.getTargetContainer().getTanks();
	for (itor = tanks.begin();
		itor != tanks.end();
		++itor)
	{
		// For each tank
		Tank *tank = (*itor).second;
		if (destinationId_ == tank->getDestinationId())
		{
			TankLoadedSimAction *loadedAction = 
				new TankLoadedSimAction(tank->getPlayerId(), 
					tank->getState().getNotSpectator());
			ScorchedServer::instance()->getServerSimulator().
				addSimulatorAction(loadedAction);
		}
	}

	ServerDestination *serverDestination =
		ScorchedServer::instance()->getServerDestinations().getDestination(destinationId_);
	if (serverDestination)
	{
		ScorchedServer::instance()->getServerSimulator().
			addSimulatorAction(new DestinationLoadedClientSimAction(destinationId_, serverDestination->getInitialLevel()));
		serverDestination->setInitialLevel(false);
	}
}

bool DestinationLoadedServerSimAction::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(destinationId_);
	return true;
}

bool DestinationLoadedServerSimAction::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(destinationId_)) return false;
	return true;
}
