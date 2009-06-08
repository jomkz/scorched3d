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

#include <server/ServerSimulator.h>
#include <server/ScorchedServer.h>
#include <coms/ComsSimulateMessage.h>
#include <coms/ComsMessageSender.h>
#include <common/Logger.h>

ServerSimulator::ServerSimulator()
{
}

ServerSimulator::~ServerSimulator()
{
}

void ServerSimulator::reset()
{
	while (!sendActions_.empty())
	{
		delete sendActions_.front();
		sendActions_.pop_front();
	}
	Simulator::reset();
}

void ServerSimulator::addSimulatorAction(SimAction *action)
{
	sendActions_.push_back(action);
}

void ServerSimulator::nextSendTime()
{
	// On the server we never recieved messages
	// so just never wait
	waitingEventTime_ = nextEventTime_;

	// Add the new actions for this time
	std::list<SimAction *>::iterator itor;
	for (itor = sendActions_.begin();
		itor != sendActions_.end();
		itor++)
	{
		SimAction *action = *itor;
		simActions_.push_back(new SimActionContainer(action, waitingEventTime_));
	}

	//Logger::log(S3D::formatStringBuffer("Total Time %.2f, Waiting Time %.2f", 
	//	totalTime_.asFloat(), waitingEventTime_.asFloat()));

	// Send the time and actions to the client
	ComsSimulateMessage simulateMessage(waitingEventTime_, totalTime_, sendActions_);
	ComsMessageSender::sendToAllLoadedClients(simulateMessage);

	// Not needed but just to make obvious this is cleared
	sendActions_.clear();
}
