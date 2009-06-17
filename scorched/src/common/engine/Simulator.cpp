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

static const fixed StepSize = fixed(true, FIXED_RESOLUTION / 50);

Simulator::Simulator()
{
	reset();
}

Simulator::~Simulator()
{

}

void Simulator::reset()
{
	speed_ = 1;
	stepTime_ = 0;
	totalTime_ = 0;
	lastTickTime_ = SDL_GetTicks();
	while (!simActions_.empty())
	{
		delete simActions_.front();
		simActions_.pop_front();
	}
}

void Simulator::setScorchedContext(ScorchedContext *context)
{
	context_ = context;
	actionController_.setScorchedContext(context);
}

void Simulator::simulate()
{
	unsigned int currentTime = SDL_GetTicks();
	unsigned int timeDiff = currentTime - lastTickTime_;
	lastTickTime_ = currentTime;

	if (timeDiff > 0)
	{
		fixed fixedTimeDiff(true, timeDiff * 10);
		fixedTimeDiff *= speed_;

		stepTime_ += fixedTimeDiff;
		while (stepTime_ >= StepSize)
		{
			if (!continueToSimulate()) break;

			// Simulate
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

	while (!simActions_.empty())
	{
		SimActionContainer *container = simActions_.front();
		if (container->fireTime_ > totalTime_) break;
		container->action_->invokeAction(*context_);
		delete container;
		simActions_.pop_front();
	}
}
