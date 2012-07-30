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

#include <client/ClientState.h>
#include <ui/RocketGameState.h>
#include <graph/OptionsDisplay.h>
#include <graph/FrameLimiter.h>
#include <SDL/SDL.h>

static struct STATE_NAME
{
	std::string stateName;
	ClientState::State state;
} STATE_NAMES[] =
{
	"StateMainOptions", ClientState::StateMainOptions,
	"StateStartGame", ClientState::StateStartGame
};

static struct STATE_STIMULI
{
	std::string stimulusName;
	ClientState::Stimulus stimulus;
} STATE_STIMULUS[] =
{
	"StimulusStartGame", ClientState::StimulusStartGame
};

ClientState::ClientState() : 
	stopped_(false), paused_(false),
	currentState_(StateNone)
{
}

ClientState::~ClientState()
{
}

bool ClientState::getCurrentStimulus(Stimulus stimulus)
{
	if (stimuli_.empty()) return false;
	if (stimuli_.front() == stimulus) 
	{
		stimuli_.pop_front();
		return true;
	}
	return false;
}

void ClientState::errorCurrentStimulus()
{
	if (stimuli_.empty()) return;
	S3D::dialogExit("ClientState", 
		S3D::formatStringBuffer("Failed to find state stimulus %u in state %u", 
		stimuli_.front(), currentState_));
}

void ClientState::performStateStimulus(const std::string &state)
{
	for (int i=0; i<sizeof(STATE_STIMULUS)/sizeof(STATE_STIMULI); i++)
	{
		if (STATE_STIMULUS[i].stimulusName == state) 
		{
			stimuli_.push_back(STATE_STIMULUS[i].stimulus);
			return;
		}
	}
	S3D::dialogExit("ClientState", 
		S3D::formatStringBuffer("Failed to find state stimulus %s", state.c_str()));
}

void ClientState::setState(State newState)
{
	currentState_ = newState;
	for (int i=0; i<sizeof(STATE_NAMES)/sizeof(STATE_NAME); i++)
	{
		if (STATE_NAMES[i].state == newState) 
		{
			RocketGameState::instance()->setState(STATE_NAMES[i].stateName);
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

	// State stuff
	switch (currentState_)
	{
	case StateMainOptions:
		if (getCurrentStimulus(StimulusStartGame))
		{
			setState(StateStartGame);
		}
		else 
		{
			errorCurrentStimulus();
		}
		break;
	case StateStartGame:
		errorCurrentStimulus();
		break;
	default:
		S3D::dialogExit("ClientState", 
			S3D::formatStringBuffer("Unknown client state %u", currentState_));
		break;
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

