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

#include <math.h>
#include <tank/Tank.h>
#include <tank/TankType.h>
#include <tank/TankColorGenerator.h>
#include <tank/TankModelStore.h>
#include <tank/TankAccessories.h>
#include <tank/TankScore.h>
#include <tank/TankState.h>
#include <tank/TankPosition.h>
#include <tank/TankModelContainer.h>
#include <tank/TankAvatar.h>
#include <tank/TankCamera.h>
#include <tankai/TankAI.h>
#include <tankai/TankAIStore.h>
#include <weapons/AccessoryStore.h>
#include <target/TargetLife.h>
#include <engine/ScorchedContext.h>
#include <common/Defines.h>
#include <common/Logger.h>

Tank::Tank(ScorchedContext &context, 
		unsigned int playerId, 
		unsigned int destinationId,
		const LangString &name, 
		Vector &color, 
		const char *modelName,
		const char *typeName) :
	Target(playerId, name, context), 
	context_(context),
	destinationId_(destinationId),
	color_(color), 
	tankAI_(0),
	team_(0), 
	ipAddress_(0)
{
	accessories_ = new TankAccessories(context);
	score_ = new TankScore(context);
	state_ = new TankState(context, playerId);
	position_ = new TankPosition(context);
	modelContainer_ = new TankModelContainer(modelName, typeName);
	avatar_ = new TankAvatar();
	camera_ = new TankCamera(context);

	position_->setTank(this);
	score_->setTank(this);
	state_->setTank(this);
	accessories_->setTank(this);
	modelContainer_->setTank(this);
	state_->setState(TankState::sLoading);
}

Tank::~Tank()
{
	state_->setState(TankState::sDead);

	delete tankAI_; tankAI_ = 0;
	delete accessories_; accessories_ = 0;
	delete score_; score_ = 0;
	delete state_; state_ = 0;
	delete position_; position_ = 0;
	delete modelContainer_; modelContainer_ = 0;
	delete avatar_; avatar_ = 0;
	delete camera_; camera_ = 0;
}

void Tank::setTankAI(TankAI *ai)
{
	if (tankAI_) delete tankAI_;
	tankAI_ = ai;
}

void Tank::newMatch()
{
	accessories_->newMatch();
	score_->newMatch();
	state_->newMatch();
	if (tankAI_) tankAI_->newMatch();
}

void Tank::newGame()
{
	TankType *tankType = 
		context_.getTankModels().getTypeByName(
			getModelContainer().getTankTypeName());
	getLife().setMaxLife(tankType->getLife());

	Target::newGame();

	state_->newGame();
	score_->newGame();
	position_->newGame();
	if (tankAI_) tankAI_->newGame();
}

void Tank::rezTank()
{
	if (tankAI_) tankAI_->newGame();
	getState().setState(TankState::sNormal);
	getLife().setLife(getLife().getMaxLife());
	getPosition().undo();
}

void Tank::clientNewGame()
{
	position_->clientNewGame();
	state_->clientNewGame();
}

bool Tank::getAlive()
{
	return getState().getState() == TankState::sNormal;
}

Weapon *Tank::getDeathAction()
{
	setDeathAction(context_.getAccessoryStore().getDeathAnimation());
	return Target::getDeathAction();
}

Vector &Tank::getColor()
{
	if (team_ > 0) return TankColorGenerator::getTeamColor(team_);
	return color_;
}

bool Tank::writeMessage(NetBuffer &buffer, bool writeAccessories)
{
	if (!Target::writeMessage(buffer)) return false;  // Base class 1st
	buffer.addToBuffer(destinationId_);
	buffer.addToBuffer(team_);
	buffer.addToBuffer(color_);
	if (!state_->writeMessage(buffer)) return false;
	if (!accessories_->writeMessage(buffer, writeAccessories)) return false;
	if (!score_->writeMessage(buffer)) return false;
	if (!position_->writeMessage(buffer)) return false;
	if (!modelContainer_->writeMessage(buffer)) return false;
	return true;
}

bool Tank::readMessage(NetBufferReader &reader)
{
	if (!Target::readMessage(reader)) 
	{
		Logger::log("Target::readMessage failed");
		return false; // Base class 1st
	}
	if (!reader.getFromBuffer(destinationId_))
	{
		Logger::log("Tank::destinationId_ read failed");
		return false;
	}
	if (!reader.getFromBuffer(team_))
	{
		Logger::log("Tank::team_ read failed");
		return false;
	}
	if (!reader.getFromBuffer(color_))
	{
		Logger::log("Tank::color_ read failed");
		return false;
	}
	if (!state_->readMessage(reader))
	{
		Logger::log("Tank::state_ read failed");
		return false;
	}
	if (!accessories_->readMessage(reader))
	{
		Logger::log("Tank::accessories_ read failed");
		return false;
	}
	if (!score_->readMessage(reader))
	{
		Logger::log("Tank::score_ read failed");
		return false;
	}
	if (!position_->readMessage(reader))
	{
		Logger::log("Tank::position_ read failed");
		return false;
	}
	if (!modelContainer_->readMessage(reader))
	{
		Logger::log("Tank::modelContainer_ read failed");
		return false;
	}

	if (!context_.getServerMode())
	{
		// If any humans turn into computers remove the HumanAI
		if (destinationId_ == 0) setTankAI(0);
	}
	return true;
}
