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
#include <movement/TargetMovement.h>
#include <coms/ComsSimulateMessage.h>
#include <common/Logger.h>

ClientSimulator::ClientSimulator() : 
	GameStateI("ClientSimulator")
{
}

ClientSimulator::~ClientSimulator()
{
}

void ClientSimulator::nextSendTime()
{

}

void ClientSimulator::simulate(const unsigned state, float simTime)
{
	Simulator::simulate();
}

void ClientSimulator::draw(const unsigned state)
{
	Simulator::draw();
}

bool ClientSimulator::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	ComsSimulateMessage message;
	if (!message.readMessage(reader)) return false;

	// Make sure simulator is up to date
	Simulator::simulate();

	// Set new waiting time
	waitingEventTime_ = message.getEventTime();

	Logger::log(S3D::formatStringBuffer("Total Time %.2f, Server Total Time %.2f, Waiting Time %.2f", 
		totalTime_.asFloat(), message.getTotalTime().asFloat(), waitingEventTime_.asFloat()));

	return true;
}