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

#include <math.h>
#include <tankai/TankAIAdder.h>
#include <target/Target.h>
#include <target/TargetState.h>
#include <target/TargetRenderer.h>
#include <target/TargetLife.h>
#include <target/TargetShield.h>
#include <target/TargetParachute.h>
#include <target/TargetGroup.h>
#include <engine/ScorchedContext.h>
#include <weapons/AccessoryStore.h>
#include <landscapemap/LandscapeMaps.h>
#include <common/Defines.h>
#include <common/Logger.h>

Target::Target(unsigned int playerId, 
	const char *name,
	ScorchedContext &context) :
	playerId_(playerId),
	name_(name),
	context_(context),
	deathAction_(0), burnAction_(0),
	renderer_(0), 
	border_(0)
{
	life_ = new TargetLife(context, playerId);
	shield_ = new TargetShield(context, playerId);
	parachute_ = new TargetParachute(context);
	group_ = new TargetGroup(context);
	targetState_ = new TargetState();

	life_->setTarget(this);
	life_->setBoundingSphere(true);
	shield_->setTarget(this);
	shield_->setCurrentShield(0);
	group_->setTarget(this);
}

Target::~Target()
{
	context_.getLandscapeMaps().getGroundMaps().getGroups().
		removeFromGroups(&getGroup());
	life_->setLife(0);

	delete renderer_; renderer_ = 0;
	delete life_; life_ = 0;
	delete shield_; shield_ = 0;
	delete group_; group_ = 0;
	delete parachute_; parachute_ = 0;
	delete targetState_; targetState_ = 0;
	playerId_ = 0;
}

void Target::newGame()
{
	life_->newGame();
	shield_->newGame();
	parachute_->newGame();
}

bool Target::isTemp()
{
	return (isTarget() || 
		getPlayerId() >= TargetID::MIN_TARGET_ID);
}

bool Target::getAlive()
{
	return (life_->getLife() > 0);
}

bool Target::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(name_);
	if (!shield_->writeMessage(buffer)) return false;
	if (!life_->writeMessage(buffer)) return false;
	if (!parachute_->writeMessage(buffer)) return false;
	if (!targetState_->writeMessage(buffer)) return false;
	if (!group_->writeMessage(buffer)) return false;
	buffer.addToBuffer(border_);
	if (!context_.getAccessoryStore().writeWeapon(buffer, deathAction_)) return false;
	if (!context_.getAccessoryStore().writeWeapon(buffer, burnAction_)) return false;

	return true;
}

bool Target::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(name_))
	{
		Logger::log("Target::name_ read failed");
		return false;
	}
	if (!shield_->readMessage(reader))
	{
		Logger::log("Target::shield_ read failed");
		return false;
	}
	if (!life_->readMessage(reader))
	{
		Logger::log("Target::life_ read failed");
		return false;
	}
	if (!parachute_->readMessage(reader))
	{
		Logger::log("Target::parachute_ read failed");
		return false;
	}
	if (!targetState_->readMessage(reader))
	{
		Logger::log("Target::targetState_ read failed");
		return false;
	}
	if (!group_->readMessage(reader))
	{
		Logger::log("Target::group_ read failed");
		return false;
	}
	if (!reader.getFromBuffer(border_))
	{
		Logger::log("Target::border read failed");
		return false;
	}
	deathAction_ = context_.getAccessoryStore().readWeapon(reader);
	burnAction_ = context_.getAccessoryStore().readWeapon(reader);

	return true;
}
