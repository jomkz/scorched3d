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

#include <engine/Simulator.h>
#include <engine/ScorchedContext.h>
#include <movement/TargetMovement.h>
#include <SDL/SDL.h>

static const fixed StepSize = fixed(true, 20);

Simulator::Simulator() : 
	speed_(1), stepTime_(0), totalTime_(0),
	firstItteration_(true)
{

}

Simulator::~Simulator()
{

}

void Simulator::setScorchedContext(ScorchedContext *context)
{
	context_ = context;
	actionController_.setScorchedContext(context);
}

void Simulator::simulate()
{
	unsigned int currentTime = SDL_GetTicks();
	unsigned int timeDiff = 0;
	if (!firstItteration_)
	{
		timeDiff = currentTime - lastTickTime_;
	}
	lastTickTime_ = currentTime;
	firstItteration_ = false;

	if (timeDiff > 0)
	{
		fixed fixedTimeDiff(true, timeDiff);
		fixedTimeDiff *= speed_;

		stepTime_ += fixedTimeDiff;
		while (stepTime_ >= StepSize)
		{
			actualSimulate(StepSize);

			// More time has passed
			totalTime_ += StepSize;
			stepTime_ -= StepSize;
		}
	}
}

void Simulator::actualSimulate(fixed frameTime)
{
	// Move the targets
	context_->getTargetMovement().simulate(*context_, frameTime);

	// Move the actions
	actionController_.simulate(frameTime);


	//events_.simulate(frameTime, *context_);
}

void Simulator::draw()
{
	actionController_.draw();
}

SimulatorGameState::SimulatorGameState(Simulator *simulator) :
	GameStateI("Simulator"),
	simulator_(simulator)
{
}

SimulatorGameState::~SimulatorGameState()
{
}

void SimulatorGameState::simulate(const unsigned state, float simTime)
{
	simulator_->simulate();
}

void SimulatorGameState::draw(const unsigned state)
{
	simulator_->draw();
}