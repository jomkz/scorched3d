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
#include <coms/ComsNetStatMessage.h>
#include <coms/ComsMessageSender.h>
#include <common/Logger.h>

ClientSimulator::ClientSimulator() : 
	GameStateI("ClientSimulator"),
	serverTimeDifference_(10, 0),
	serverChoke_(25, 0)
{
}

ClientSimulator::~ClientSimulator()
{
}

bool ClientSimulator::continueToSimulate()
{
	bool choked = (currentTime_ >= waitingEventTime_);
	serverChoke_.addValue(choked?100:0);
	return !choked;
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

bool ClientSimulator::processNetStatMessage(
	NetMessage &netMessage,
	NetBufferReader &reader)
{
	ComsNetStatMessage message;
	if (!message.readMessage(reader)) return false;

	// Store some stats
	serverStepTime_ = message.getSendStepSize();
	serverRoundTripTime_ = message.getRoundTripTime();

	return true;
}

bool ClientSimulator::processComsSimulateMessage(
	NetMessage &netMessage,
	NetBufferReader &reader)
{
	ComsSimulateMessage message;
	if (!message.readMessage(reader)) return false;

	// Actualy process message
	addComsSimulateMessage(message);

	// Send back a response so ping times can be calculated
	ComsSimulateResultMessage resultMessage(message.getActualTime());
	ComsMessageSender::sendToServer(resultMessage);

	// Calculate the difference between the local time and the server time
	fixed difference = (message.getActualTime() + serverRoundTripTime_ / 2) - actualTime_;
	serverTimeDifference_.addValue(difference);

	// Make adjustment to local time
	actualTime_ += serverTimeDifference_.getAverage() / 20;

	Logger::log(S3D::formatStringBuffer("Client Actual %.2f, Server Actual %.2f, Adjustment %.2f",
		actualTime_.asFloat(), message.getActualTime().asFloat(), serverTimeDifference_.getAverage().asFloat()));

	return true;
}

void ClientSimulator::addComsSimulateMessage(ComsSimulateMessage &message)
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
	actualTime_ = 0;
	currentTime_ = 0;
	lastTickTime_ = SDL_GetTicks() - 1;
	serverTimeDifference_.reset(0);
}

void ClientSimulator::setSimulationTime(fixed actualTime)
{
	// Set actual time
	actualTime_ = actualTime;
	serverTimeDifference_.reset(0);

	// Simulate
	Simulator::simulate();
}
