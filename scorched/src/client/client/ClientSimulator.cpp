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

#include <client/ClientSimulator.h>
#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <movement/TargetMovement.h>
#include <coms/ComsSimulateMessage.h>
#include <coms/ComsSimulateResultMessage.h>
#include <coms/ComsMessageSender.h>
#include <common/Logger.h>

ClientSimulator::ClientSimulator() : 
	GameStateI("ClientSimulator")
{
}

ClientSimulator::~ClientSimulator()
{
}

bool ClientSimulator::continueToSimulate()
{
	if (currentTime_ >= waitingEventTime_) return false;
	return true;
}

void ClientSimulator::simulate(const unsigned state, float simTime)
{
	if (state == ClientState::StateLoadLevel) return;
	Simulator::simulate();
}

void ClientSimulator::draw(const unsigned state)
{
	actionController_.draw();
}

bool ClientSimulator::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	ComsSimulateMessage message;
	if (!message.readMessage(reader)) return false;

	// Actualy process message
	processComsSimulateMessage(message);

	ComsSimulateResultMessage resultMessage(message.getTotalTime());
	ComsMessageSender::sendToServer(resultMessage);

	serverTimeDifference_ = message.getTotalTime() - currentTime_;
	//Logger::log(S3D::formatStringBuffer("Total Time %.2f, Server Total Time %.2f, Waiting Time %.2f", 
	//	currentTime_.asFloat(), message.getTotalTime().asFloat(), waitingEventTime_.asFloat()));

	return true;
}

void ClientSimulator::processComsSimulateMessage(ComsSimulateMessage &message)
{
	// Set new waiting time
	waitingEventTime_ = message.getEventTime();

	// Add the new actions for this time
	std::list<SimAction *>::iterator itor;
	for (itor = message.getActions().begin();
		itor != message.getActions().end();
		itor++)
	{
		SimAction *action = *itor;
		simActions_.push_back(new SimActionContainer(action, waitingEventTime_));
	}
}

void ClientSimulator::newLevel()
{
	// Remove any remaining actions
	while (!simActions_.empty())
	{
		SimActionContainer *container = simActions_.front();
		delete container;
		simActions_.pop_front();
	}

	// Clear any action controller actions
	actionController_.clear();

	// Reset times
	currentTime_ = 0;
	lastTickTime_ = SDL_GetTicks() - 1;
}

void ClientSimulator::setSimulationTime(fixed actualTime)
{
	// Set actual time
	actualTime_ = actualTime;

	// Simulate
	Simulator::simulate();
}
