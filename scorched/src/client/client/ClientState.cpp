////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011
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
#include <client/ClientParams.h>
#include <client/ClientSimulator.h>
#include <landscape/Landscape.h>
#include <landscape/LandscapeMusicManager.h>
#include <sound/Sound.h>
#include <server/ServerMain.h>
#include <engine/Simulator.h>
#include <common/Logger.h>
#include <ui/RocketGameState.h>
#include <graph/OptionsDisplay.h>
#include <graph/FrameLimiter.h>
#include <graph/FrameTimer.h>
#include <graph/ParticleEngine.h>
#include <graph/MainCamera.h>
#include <tankgraph/RenderTargets.h>
#include <target/TargetContainer.h>
#include <net/NetInterface.h>
#include <SDL/SDL.h>

static time_t startTime = 0;

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
	stopped_(false), paused_(false),
	currentState_(StateNone),
	serverTime_(0.0f)
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
			RocketGameState::instance()->setState(STATE_NAMES[i].stateName);
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

void ClientState::clientEventLoop()
{
	// Process SDL events
	static SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_KEYUP:
			break;
		case SDL_KEYDOWN:
			/* keyevents are handled in mainloop */
			//Keyboard::instance()->processKeyboardEvent(event);
			break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEMOTION:
			RocketGameState::instance()->processMouseEvent(event);
			break;
		case SDL_ACTIVEEVENT:
			paused_ = (OptionsDisplay::instance()->getFocusPause() && (event.active.gain == 0));
			break;
		case SDL_VIDEORESIZE:
			break;
		case SDL_QUIT:
			stopped_ = true;
			break;
		}
	}

	// Render
	if (!paused_)
	{
		RocketGameState::instance()->draw();
	}
	else
	{
		SDL_Delay(100);  // Otherwise when not drawing graphics its an infinite loop
		frameLimiter_.dontLimitFrameTime();
	}
	frameLimiter_.limitFrameTime(); 

	// Simulate
	float frameTime = frameClock_.getTimeDifference();
	if (!ClientParams::instance()->getConnectedToServer())
	{
		serverTime_ += frameTime;
		if (serverTime_ > 0.05f)
		{
			fixed timeDifference = fixed::fromFloat(serverTime_);
			timeDifference *= ScorchedClient::instance()->getSimulator().getFast();
			serverLoop(timeDifference);
			serverTime_ = 0.0f;
		}
	}

	Sound::instance()->simulate(frameTime);
	LandscapeMusicManager::instance()->simulate(currentState_, frameTime);
	FrameTimer::instance()->simulate(frameTime);
	ScorchedClient::instance()->getClientSimulator().simulate();
	if (currentState_ == StateWait)
	{
		MainCamera::instance()->simulate(currentState_, frameTime);
		Landscape::instance()->simulate(frameTime);
		ScorchedClient::instance()->getParticleEngine().simulate(frameTime);
		RenderTargets::instance()->simulate(frameTime);
	}

	Logger::processLogEntries();
	if (ScorchedClient::instance()->getContext().getNetInterfaceValid())
	{
		ScorchedClient::instance()->getNetInterface().processMessages();
	}

	if (ClientParams::instance()->getExitTime() > 0)
	{
		if (startTime == 0) startTime = time(0);
		if (time(0) - startTime > ClientParams::instance()->getExitTime())
		{
			exit(0);
		}
	}
	if (ClientParams::instance()->getDisconnectTime() > 0)
	{
		if (startTime == 0) startTime = time(0);
		if (time(0) - startTime > ClientParams::instance()->getDisconnectTime())
		{
			startTime = time(0);
			if (ScorchedClient::instance()->getTargetContainer().getCurrentDestinationId())
			{
				ScorchedClient::instance()->getNetInterface().disconnectAllClients();
			}
			else
			{
				setState(StateInitialize);
			}
		}
	}
}

void ClientState::clientMainLoop()
{
	setState(StateMainOptions);
	while (!stopped_)
	{
		clientEventLoop();
	}
}

