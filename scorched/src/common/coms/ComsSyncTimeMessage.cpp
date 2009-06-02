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

#include <coms/ComsSyncTimeMessage.h>
#ifndef S3D_SERVER
#include <client/ScorchedClient.h>
#endif
#include <server/ScorchedServer.h>
#include <engine/Simulator.h>

ComsMessageType ComsSyncTimeMessage::ComsSyncTimeMessageType("ComsSyncTimeMessageType");

ComsSyncTimeMessage::ComsSyncTimeMessage() :
	ComsMessage(ComsSyncTimeMessageType)
{
}

ComsSyncTimeMessage::~ComsSyncTimeMessage()
{
}

bool ComsSyncTimeMessage::writeMessage(NetBuffer &buffer)
{
	// Simulator state
	if (!ScorchedServer::instance()->getSimulator().writeTimeMessage(buffer)) return false;

	return true;
}

bool ComsSyncTimeMessage::readMessage(NetBufferReader &reader)
{
#ifndef S3D_SERVER
	// Simulator state
	if (!ScorchedClient::instance()->getSimulator().readTimeMessage(reader)) return false;
#endif
	
	return true;
}
