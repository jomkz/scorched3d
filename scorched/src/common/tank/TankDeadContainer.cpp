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

#include <tank/TankDeadContainer.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <tank/TankAccessories.h>
#include <net/NetBuffer.h>
#include <server/ScorchedServer.h>
#include <common/OptionsScorched.h>
#include <common/Logger.h>

TankDeadContainer::TankDeadContainer() 
{

}

TankDeadContainer::~TankDeadContainer()
{

}

void TankDeadContainer::addDeadTank(Tank *tank)
{
	// Find/create buffer
	NetBuffer *buffer = 0;
	std::map<std::string, NetBuffer *>::iterator finditor =
		deadTanks_.find(tank->getUniqueId());
	if (finditor != deadTanks_.end())
	{
		buffer = finditor->second;
	}
	else
	{
		buffer = new NetBuffer();
	}
	buffer->reset();
	
	// Write to buffer
	if (!tank->getAccessories().writeMessage(*buffer, true) ||
		!tank->getScore().writeMessage(*buffer))
	{
		Logger::log("ERROR: Failed to update residual player info (write)");
		return;
	}

	// Save buffer
	deadTanks_[tank->getUniqueId()] = buffer;
}

void TankDeadContainer::getDeadTank(Tank *tank, TankAliveSimAction *simAction)
{
	// Get the buffer
	std::map<std::string, NetBuffer *>::iterator finditor =
		deadTanks_.find(tank->getUniqueId());
	if (finditor == deadTanks_.end()) return;
	NetBuffer *buffer = finditor->second;

	simAction->getScoreNetBuffer().reset();
	simAction->getScoreNetBuffer().addDataToBuffer(buffer->getBuffer(), buffer->getBufferUsed());

	deadTanks_.erase(finditor);
	delete buffer;
}

void TankDeadContainer::clearTanks()
{
	std::map<std::string, NetBuffer *>::iterator itor;
	for (itor = deadTanks_.begin();
		itor != deadTanks_.end();
		itor++)
	{
		NetBuffer *current = (*itor).second;
		delete current;
	}
	deadTanks_.clear();
}

