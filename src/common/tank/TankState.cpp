////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
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

#include <stdio.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <target/TargetLife.h>
#include <target/TargetShield.h>
#include <target/TargetState.h>
#include <engine/ScorchedContext.h>
#include <common/OptionsScorched.h>
#include <common/Defines.h>

static struct AllowedStateTransitions
{
	TankState::State from, to;
}
allowedStateTransitions[] =
{
	TankState::sLoading, TankState::sInitializing,
	TankState::sInitializing,TankState::sPending,
	TankState::sPending, TankState::sDead,
	TankState::sDead, TankState::sNormal,
	TankState::sNormal , TankState::sDead
};

TankState::TankState(ScorchedContext &context, unsigned int playerId) : 
	state_(sLoading), tank_(0),
	readyState_(sReady),
	context_(context), spectator_(false), 
	muted_(false),
	skipshots_(false),
	lives_(0), maxLives_(1), destroy_(false)
{
}

TankState::~TankState()
{
}

void TankState::newMatch()
{
	setState(sDead);
	readyState_ = sReady;
}

void TankState::newGame()
{
	setState(sNormal);
	if (!tank_->isTemp())
	{
		maxLives_ = context_.getOptionsGame().getPlayerLives();
	}

	lives_ = maxLives_;
	tank_->getTargetState().setFalling(0);
}

void TankState::clientNewGame()
{
	skipshots_ = false;
}

void TankState::setState(State s)
{
	for (int i=0; i<sizeof(allowedStateTransitions) / 
			sizeof(AllowedStateTransitions); i++)
	{
		if (state_ == allowedStateTransitions[i].from &&
			s == allowedStateTransitions[i].to)
		{
			state_ = s;
			break;
		}
	}

 	if (state_ != sNormal)
	{
		// Make sure the target and shield physics
		// are disabled
		tank_->getLife().setLife(0);
		tank_->getShield().setCurrentShield(0);
	}
	else
	{
		// Make sure target space contains tank
		tank_->getLife().setLife(tank_->getLife().getLife());
	}
}

const char *TankState::getStateString()
{
	static char string[1024];
	snprintf(string, 1024, "%s - %s %s(%i hp)",
		((readyState_==sReady)?"Rdy":"Wait"),
		getSmallStateString(),
		(muted_?"muted ":""),
		(int) tank_->getLife().getLife().asInt());
	return string;
}

const char *TankState::getSmallStateString()
{
	const char *type = "";
	switch (state_)
	{
	case sPending:
		type = spectator_?"(Spec)Pending":"Pending";
		break;
	case sNormal:
		type = spectator_?"(Spec)Alive":"Alive";
		break;
	case sInitializing:
		type = spectator_?"(Spec)Initializing":"Initializing";
		break;
	case sLoading:
		type = spectator_?"(Spec)Loading":"Loading";
		break;
	case sDead:
		type = spectator_?"(Spec)Dead":"Dead";
		break;
	}

	return type;
}

bool TankState::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer((int) state_);
	buffer.addToBuffer(spectator_);
	buffer.addToBuffer(lives_);
	buffer.addToBuffer(maxLives_);
	return true;
}

bool TankState::readMessage(NetBufferReader &reader)
{
	int s;
	if (!reader.getFromBuffer(s)) return false;
	state_ = (TankState::State) s;
	setState((TankState::State) s);
	if (!reader.getFromBuffer(spectator_)) return false;
	if (!reader.getFromBuffer(lives_)) return false;
	if (!reader.getFromBuffer(maxLives_)) return false;
	return true;
}
