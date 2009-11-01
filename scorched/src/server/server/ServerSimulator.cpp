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
#include <server/ServerState.h>
#include <coms/ComsSimulateMessage.h>
#include <coms/ComsMessageSender.h>
#include <coms/ComsSimulateResultMessage.h>
#include <coms/ComsNetStatMessage.h>
#include <simactions/SyncCheckSimAction.h>
#include <landscapemap/LandscapeMaps.h>
#include <movement/TargetMovement.h>
#include <common/Logger.h>

static fixed maxStepSize(true, 1 * FIXED_RESOLUTION);
static fixed minStepSize(true, FIXED_RESOLUTION / 10);

ServerSimulator::ServerSimulator() :
	sendStepSize_(true, 1 * FIXED_RESOLUTION),
	levelMessage_(0)
{
	nextSendTime_ = 0;
	nextEventTime_ = nextSendTime_ + sendStepSize_;
}

ServerSimulator::~ServerSimulator()
{
}

void ServerSimulator::addSimulatorAction(SimAction *action)
{
	sendActions_.push_back(action);
}

bool ServerSimulator::continueToSimulate()
{
	// Send out the message at the correct time
	if (currentTime_ >= nextSendTime_)
	{
		sendStepSize_ = calcSendStepSize();
		nextSendTime_ = nextEventTime_;
		nextEventTime_ += sendStepSize_;

		nextSendTime();

		/*
		Logger::log(
			S3D::formatStringBuffer(
			"Current time %.2f, send time %.2f, event time %.2f, step size %.2f",
			currentTime_.asFloat(), nextSendTime_.asFloat(), 
			nextEventTime_.asFloat(), sendStepSize_.asFloat()));
		*/
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

	// Send the time and actions to the client
	unsigned int serverTime = SDL_GetTicks();
	ComsSimulateMessage simulateMessage(nextEventTime_, actualTime_, serverTime, sendActions_);
	ComsMessageSender::sendToAllLoadedClients(simulateMessage);
	if (levelMessage_)
	{
		levelMessage_->addSimulation(simulateMessage);
	}

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
		fixed value = destination->getPing().getAverage() + 
			fixed(true, FIXED_RESOLUTION / 10);
		if (value > max)
		{
			max = value;
		}
	}

	fixed stepSize = MAX(MIN(max, maxStepSize), minStepSize);

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

	if (destination->getState() == ServerDestination::sFinished)
	{
		unsigned int serverTime = SDL_GetTicks();
		unsigned int roundTripTime = serverTime - message.getServerTime();
		fixed fixedRoundTripTime(true, roundTripTime * 10);

		destination->getPing().addValue(fixedRoundTripTime);
		if (currentTime_ > destination->getLastSentPingTime() + 2)
		{
			destination->setLastSentPingTime(currentTime_);
			ComsNetStatMessage netStatMessage(
				destination->getPing().getAverage(), sendStepSize_);
			ComsMessageSender::sendToSingleClient(
				netStatMessage, netMessage.getDestinationId());
		}
	}

	return true;
}

void ServerSimulator::newLevel()
{
	// Process any remaining actions
	while (!simActions_.empty())
	{
		SimActionContainer *container = simActions_.front();
		container->action_->invokeAction(*context_);
		delete container;
		simActions_.pop_front();
	}

	Simulator::newLevel();

	nextSendTime_ = 0;
	nextEventTime_ = nextSendTime_ + sendStepSize_;

	// Store the current state
	// Tanks, options, game, random numbers, etc...
	delete levelMessage_;
	levelMessage_ = new ComsLoadLevelMessage();
	levelMessage_->setLandscapeDefinition(ScorchedServer::instance()->
		getLandscapeMaps().getDefinitions().getDefinition());
	levelMessage_->saveState(ScorchedServer::instance()->getContext());
}

ComsLoadLevelMessage &ServerSimulator::getLevelMessage()
{
	levelMessage_->setActualTime(actualTime_);
	return *levelMessage_;
}

void ServerSimulator::actualSimulate(fixed frameTime)
{
	Simulator::actualSimulate(frameTime);
	ScorchedServer::instance()->getServerState().simulate(frameTime);
}
