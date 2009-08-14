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
#include <server/ServerSyncCheck.h>
#include <server/ServerDestinations.h>
#include <coms/ComsSimulateMessage.h>
#include <coms/ComsMessageSender.h>
#include <coms/ComsSimulateResultMessage.h>
#include <simactions/SyncCheckSimAction.h>
#include <movement/TargetMovement.h>
#include <common/Logger.h>

static fixed maxStepSize(true, 1 * FIXED_RESOLUTION);
static fixed minStepSize(true, FIXED_RESOLUTION / 10);

ServerSimulator::ServerSimulator() :
	sendStepSize_(true, 1 * FIXED_RESOLUTION)
{
}

ServerSimulator::~ServerSimulator()
{
}

void ServerSimulator::reset()
{
	nextSendTime_ = 0;
	nextEventTime_ = nextSendTime_ + sendStepSize_;
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

bool ServerSimulator::continueToSimulate()
{
	// Send out the message at the correct time
	if (totalTime_ >= nextSendTime_)
	{
		nextSendTime();
		sendStepSize_ = calcSendStepSize();
		nextSendTime_ = nextEventTime_;
		nextEventTime_ += sendStepSize_;
	}

	return true;
}

void ServerSimulator::nextSendTime()
{
	static SyncCheckSimAction syncCheckSimAction;

	// Add the new actions for this time
	std::list<SimAction *>::iterator itor;
	for (itor = sendActions_.begin();
		itor != sendActions_.end();
		itor++)
	{
		SimAction *action = *itor;
		simActions_.push_back(new SimActionContainer(action, nextEventTime_));
		if (action->getMetaClassId() == syncCheckSimAction.getMetaClassId())
		{
			SyncCheckSimAction *syncAction = (SyncCheckSimAction *) action;
			ServerSyncCheck::instance()->sentSyncCheck(syncAction->getSyncId());
		}
	}

	//Logger::log(S3D::formatStringBuffer("Total Time %.2f, Waiting Time %.2f", 
	//	totalTime_.asFloat(), waitingEventTime_.asFloat()));

	// Send the time and actions to the client
	ComsSimulateMessage simulateMessage(nextEventTime_, totalTime_, sendActions_);
	ComsMessageSender::sendToAllLoadedClients(simulateMessage);

	// Not needed but just to make obvious this is cleared
	sendActions_.clear();
}

fixed ServerSimulator::calcSendStepSize()
{
	// Now check the average ping
	fixed max = 0;
	std::map<unsigned int, ServerDestination*> &destinations = 
		ScorchedServer::instance()->getServerDestinations().getServerDestinations();
	std::map<unsigned int, ServerDestination*>::iterator destItor;
	for (destItor = destinations.begin();
		destItor != destinations.end();
		destItor++)
	{
		ServerDestination *destination = destItor->second;
		if (destination->getPing().getPing() > max)
		{
			max = destination->getPing().getPing();
		}
	}

	fixed stepSize = MAX(MIN(max, maxStepSize), minStepSize);

	//Logger::log(S3D::formatStringBuffer("Average ping time %.2f (%.2f)", 
	//	total.asFloat(), stepSize.asFloat()));

	return stepSize;
}

bool ServerSimulator::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	ComsSimulateResultMessage message;
	if (!message.readMessage(reader)) return false;

	ServerDestination *destination =
		ScorchedServer::instance()->getServerDestinations().
		getDestination(netMessage.getDestinationId());
	if (!destination) return true;

	fixed roundTripTime = totalTime_ - message.getTotalTime();
	destination->getPing().addPing(roundTripTime);

	//Logger::log(S3D::formatStringBuffer("Ping time %.2f (%.2f)", 
	//	roundTripTime.asFloat(), destination->getPing().getPing().asFloat()));

	return true;
}

bool ServerSimulator::writeTimeMessage(NetBuffer &buffer)
{
	// Simulator time
	buffer.addToBuffer(stepTime_);
	buffer.addToBuffer(totalTime_);

	return true;
}

bool ServerSimulator::writeSyncMessage(NetBuffer &buffer)
{
	// Actions
	buffer.addToBuffer((unsigned int) simActions_.size());
	std::list<SimActionContainer *>::iterator itor;
	for (itor = simActions_.begin();
		itor != simActions_.end();
		itor++)
	{
		SimActionContainer *container = *itor;
		buffer.addToBuffer(container->fireTime_);
		buffer.addToBuffer(container->action_->getClassName());
		container->action_->writeMessage(buffer);
	}

	// Random seeds
	if (!random_.writeMessage(buffer)) return false;

	// Target movement
	if (!context_->getTargetMovement().writeMessage(buffer)) return false;

	return true;
}
