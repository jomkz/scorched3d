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

void ServerSimulator::nextSendTime()
{
	// On the server we never recieved messages
	// so just never wait
	waitingEventTime_ = nextEventTime_;

	//Logger::log(S3D::formatStringBuffer("Total Time %.2f, Waiting Time %.2f", 
	//	totalTime_.asFloat(), waitingEventTime_.asFloat()));

	ComsSimulateMessage simulateMessage(waitingEventTime_, totalTime_);
	ComsMessageSender::sendToAllLoadedClients(simulateMessage);
}
