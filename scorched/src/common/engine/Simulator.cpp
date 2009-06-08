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
static const fixed SendStepSize = fixed(true, 1 * FIXED_RESOLUTION);

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
	nextSendTime_ = 0;
	nextEventTime_ = nextSendTime_ + SendStepSize + SendStepSize;
	waitingEventTime_ = nextEventTime_;
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
			if (totalTime_ == waitingEventTime_)
			{
				// The simulation has reached the time for the next message
				// but we have not recieved the message
				// Wait for it
				break;
			}

			// Send out the message at the correct time
			if (totalTime_ == nextSendTime_)
			{
				nextSendTime();
				nextSendTime_ += SendStepSize;
				nextEventTime_ += SendStepSize;
			}

			// Simulate
			actualSimulate(StepSize);

			// More time has passed
			totalTime_ += StepSize;
			stepTime_ -= StepSize;
		}
	}
}

void Simulator::nextSendTime()
{

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

void Simulator::draw()
{
	actionController_.draw();
}

bool Simulator::writeTimeMessage(NetBuffer &buffer)
{
	// Simulator time
	buffer.addToBuffer(stepTime_);
	buffer.addToBuffer(totalTime_);
	buffer.addToBuffer(nextSendTime_);
	buffer.addToBuffer(nextEventTime_);
	buffer.addToBuffer(waitingEventTime_);

	return true;
}

bool Simulator::readTimeMessage(NetBufferReader &reader)
{
	reset();

	// Simulator time
	if (!reader.getFromBuffer(stepTime_)) return false;
	if (!reader.getFromBuffer(totalTime_)) return false;
	if (!reader.getFromBuffer(nextSendTime_)) return false;
	if (!reader.getFromBuffer(nextEventTime_)) return false;
	if (!reader.getFromBuffer(waitingEventTime_)) return false;

	return true;
}

bool Simulator::writeSyncMessage(NetBuffer &buffer)
{
	// Actions
	buffer.addToBuffer((unsigned int) simActions_.size());
	std::list<SimActionContainer *>::iterator itor;
	for (itor = simActions_.begin();
		itor != simActions_.end();
		itor++)
	{
		SimActionContainer *container = *itor;
		buffer.addToBuffer(container->fireTime_);
		buffer.addToBuffer(container->action_->getClassName());
		container->action_->writeMessage(buffer);
	}

	// Random seeds
	if (!random_.writeMessage(buffer)) return false;

	// Target movement
	if (!context_->getTargetMovement().writeMessage(buffer)) return false;

	return true;
}

bool Simulator::readSyncMessage(NetBufferReader &reader)
{
	// Actions
	unsigned int numberActions;
	if (!reader.getFromBuffer(numberActions)) return false;
	for (unsigned int a=0; a<numberActions; a++)
	{
		fixed fireTime;
		if (!reader.getFromBuffer(fireTime)) return false;

		std::string className;
		if (!reader.getFromBuffer(className)) return false;
		SimAction *simAction = (SimAction *)
			MetaClassRegistration::getNewClass(className.c_str());
		if (!simAction) return false;
		if (!simAction->readMessage(reader)) return false;

		SimActionContainer *container = new SimActionContainer(simAction, fireTime);
		simActions_.push_back(container);
	}

	// Random seeds
	if (!random_.readMessage(reader)) return false;

	// Target Movement
	if (!context_->getTargetMovement().readMessage(reader)) return false;
	return true;
}
