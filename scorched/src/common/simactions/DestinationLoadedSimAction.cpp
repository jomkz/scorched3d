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

#include <simactions/DestinationLoadedSimAction.h>
#include <simactions/TankLoadedSimAction.h>
#include <common/Logger.h>
#include <common/OptionsTransient.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <engine/Simulator.h>
#include <server/ScorchedServer.h>
#include <server/ServerSimulator.h>

REGISTER_CLASS_SOURCE(DestinationLoadedSimAction);

DestinationLoadedSimAction::DestinationLoadedSimAction() :
	destinationId_(0)
{
}

DestinationLoadedSimAction::DestinationLoadedSimAction(unsigned int destinationId) :
	destinationId_(destinationId)
{
}

DestinationLoadedSimAction::~DestinationLoadedSimAction()
{
}

bool DestinationLoadedSimAction::invokeAction(ScorchedContext &context)
{
	if (context.getServerMode())
	{
		setLoaded(context);
	}

	return true;
}

void DestinationLoadedSimAction::setLoaded(ScorchedContext &context)
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
}

bool DestinationLoadedSimAction::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(destinationId_);
	return true;
}

bool DestinationLoadedSimAction::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(destinationId_)) return false;
	return true;
}
