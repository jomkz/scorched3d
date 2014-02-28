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

#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <client/ClientStateInitialize.h>
#include <client/ClientStateLoadLevel.h>

static struct STATE_NAME
{
	std::string stateName;
	ClientState::ClientStateEnum state;
} STATE_NAMES[] =
{
	"StateMainOptions", ClientState::StateMainOptions,
	"StateInitialize", ClientState::StateInitialize,
	"StateLoadLevel", ClientState::StateLoadLevel,
	"StateWait", ClientState::StateWait,
	"StateWaitNoLandscape", ClientState::StateWaitNoLandscape
};

ClientState::ClientState(ComsMessageHandler &comsMessageHandler) : 
	currentState_(StateMainOptions)
{
	clientInitialize_ = new ClientStateInitialize(comsMessageHandler);
	clientLoadLevel_ = new ClientStateLoadLevel(comsMessageHandler);
}

ClientState::~ClientState()
{
}

void ClientState::setState(ClientStateEnum newState)
{
	currentState_ = newState;
	switch (currentState_) 
	{
	case StateInitialize:
		clientInitialize_->enterState();
		break;
	}

	for (int i=0; i<sizeof(STATE_NAMES)/sizeof(STATE_NAME); i++)
	{
		if (STATE_NAMES[i].state == newState) 
		{
			break;
		}
	}
}

void ClientState::setStateString(const std::string &newState)
{
	for (int i=0; i<sizeof(STATE_NAMES)/sizeof(STATE_NAME); i++)
	{
		if (STATE_NAMES[i].stateName == newState) 
		{
			setState(STATE_NAMES[i].state);
			break;
		}
	}
}


