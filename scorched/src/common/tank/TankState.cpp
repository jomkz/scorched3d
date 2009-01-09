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

#include <stdio.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <target/TargetLife.h>
#include <target/TargetShield.h>
#include <target/TargetState.h>
#include <engine/ScorchedContext.h>
#include <lang/LangResource.h>
#include <common/OptionsScorched.h>
#include <common/Defines.h>
#include <common/Logger.h>

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

LangString &TankState::getSmallStateLangString()
{
	LANG_RESOURCE_CONST_VAR(SPEC_PENDING, "SPEC_PENDING", "(Spec)Pending");
	LANG_RESOURCE_CONST_VAR(PENDING, "PENDING", "Pending");
	LANG_RESOURCE_CONST_VAR(SPEC_ALIVE, "SPEC_ALIVE", "(Spec)Alive");
	LANG_RESOURCE_CONST_VAR(ALIVE, "ALIVE", "Alive");
	LANG_RESOURCE_CONST_VAR(SPEC_INITIALIZING, "SPEC_INITIALIZING", "(Spec)Initializing");
	LANG_RESOURCE_CONST_VAR(INITIALIZING, "INITIALIZING", "Initializing");
	LANG_RESOURCE_CONST_VAR(SPEC_LOADING, "SPEC_LOADING", "(Spec)Loading");
	LANG_RESOURCE_CONST_VAR(LOADING, "LOADING", "Loading");
	LANG_RESOURCE_CONST_VAR(SPEC_DEAD, "SPEC_DEAD", "(Spec)Dead");
	LANG_RESOURCE_CONST_VAR(DEAD, "DEAD", "Dead");


	switch (state_)
	{
	case sPending:
		return spectator_?SPEC_PENDING:PENDING;
	case sNormal:
		return spectator_?SPEC_ALIVE:ALIVE;
	case sInitializing:
		return spectator_?SPEC_INITIALIZING:INITIALIZING;
	case sLoading:
		return spectator_?SPEC_LOADING:LOADING;
	case sDead:
		return spectator_?SPEC_DEAD:DEAD;
		break;
	}

	static LangString nullResult;
	return nullResult;
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
	if (!reader.getFromBuffer(s))
	{
		Logger::log("TankState::state_ read failed");
		return false;
	}
	state_ = (TankState::State) s;
	setState((TankState::State) s);
	if (!reader.getFromBuffer(spectator_))
	{
		Logger::log("TankState::spectator_ read failed");
		return false;
	}
	if (!reader.getFromBuffer(lives_))
	{
		Logger::log("TankState::lives_ read failed");
		return false;
	}
	if (!reader.getFromBuffer(maxLives_))
	{
		Logger::log("TankState::maxLives_ read failed");
		return false;
	}
	return true;
}
