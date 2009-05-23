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

#include <server/ScorchedServer.h>
#include <server/ServerShotHolder.h>
#include <server/TurnController.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <tank/TankPosition.h>
#include <tank/TankAccessories.h>
#include <tank/TankContainer.h>
#include <target/TargetLife.h>
#include <tankai/TankAIStrings.h>
#include <engine/ActionController.h>
#include <actions/TankMovement.h>
#include <actions/TankResign.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapemap/MovementMap.h>
#include <landscapedef/LandscapeDefn.h>
#include <weapons/AccessoryStore.h>
#include <coms/ComsMessageSender.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <common/StatsLogger.h>
#include <common/Defines.h>

ServerShotHolder *ServerShotHolder::instance_ = 0;

ServerShotHolder *ServerShotHolder::instance()
{
	if (!instance_)
	{
		instance_ = new ServerShotHolder;
	}
	return instance_;
}

ServerShotHolder::ServerShotHolder()
{
}

ServerShotHolder::~ServerShotHolder()
{
}

bool ServerShotHolder::addShot(unsigned int playerId,
	ComsPlayedMoveMessage *message)
{
	// Check the tank exists for this player
	Tank *tank = ScorchedServer::instance()->getTankContainer().getTankById(playerId);
	if (!tank) return false;

	// Check the tank is alive
	if (tank->getState().getState() != TankState::sNormal)
	{
		return false;
	}

	// Validate this message
	if (message->getType() == ComsPlayedMoveMessage::eShot)
	{
		// Check this player can fire this weapon etc...
		if (!validateFiredMessage(ScorchedServer::instance()->getContext(), *message, tank)) return false;
	}

	// Ensure each player can only add one message
	if (haveShot(playerId))
	{
		return false;
	}

	messages_[playerId] = message;

	return true;
}

void ServerShotHolder::removeShot(unsigned int playerId)
{
	std::map<unsigned int, ComsPlayedMoveMessage *>::iterator itor =
		messages_.find(playerId);
	if (itor == messages_.end()) return;
	delete itor->second;
	messages_.erase(playerId);
}

bool ServerShotHolder::validateFiredMessage(
	ScorchedContext &context, ComsPlayedMoveMessage &message, Tank *tank)
{
	// Check the weapon name exists and is a weapon
	Accessory *accessory = 
		context.getAccessoryStore().findByAccessoryId(
		message.getWeaponId());
	if (accessory && accessory->getType() != AccessoryPart::AccessoryWeapon)
	{
		return false;
	}

	// Check this tank has these weapons
	if (!tank->getAccessories().canUse(accessory)) return false;

	// Check armslevel
	if ((10 - accessory->getArmsLevel()) <=
		context.getOptionsTransient().getArmsLevel() ||
		context.getOptionsGame().getGiveAllWeapons()) {}
	else return false;

	// Check weapons selection parameters
	if (accessory->getPositionSelect() != Accessory::ePositionSelectNone)
	{
		int arenaX = ScorchedServer::instance()->
			getLandscapeMaps().getDefinitions().getDefn()->getArenaX();
		int arenaY = ScorchedServer::instance()->
			getLandscapeMaps().getDefinitions().getDefn()->getArenaY();
		int arenaWidth = ScorchedServer::instance()->
			getLandscapeMaps().getDefinitions().getDefn()->getArenaWidth();
		int arenaHeight = ScorchedServer::instance()->
			getLandscapeMaps().getDefinitions().getDefn()->getArenaHeight();

		if (message.getSelectPositionX() <= arenaX || message.getSelectPositionX() >= arenaX + arenaWidth &&
			message.getSelectPositionY() <= arenaY || message.getSelectPositionY() >= arenaY + arenaHeight)
		{
			return false;
		}

		if (accessory->getPositionSelect() == Accessory::ePositionSelectLimit)
		{
			FixedVector position(
				message.getSelectPositionX(), message.getSelectPositionY(), 0);
			if ((position - tank->getLife().getTargetPosition()).Magnitude() > 
				accessory->getPositionSelectLimit())
			{
				return false;
			}
		}
		else if (accessory->getPositionSelect() == Accessory::ePositionSelectFuelLimit)
		{
			MovementMap mmap(
				tank,
				ScorchedServer::instance()->getContext());

			FixedVector position(
				message.getSelectPositionX(), message.getSelectPositionY(), 0);
			mmap.calculatePosition(position, fixed(accessory->getPositionSelectLimit()));

			MovementMap::MovementMapEntry &entry =	mmap.getEntry(
				message.getSelectPositionX(), message.getSelectPositionY());
			if (entry.type != MovementMap::eMovement) return false;  // Do nothing
		}
	}

	return true;
}

bool ServerShotHolder::allSkipped()
{
	if (messages_.empty()) return false;

	std::map<unsigned int, ComsPlayedMoveMessage *>::iterator itor;
	for (itor = messages_.begin();
		itor != messages_.end();
		itor++)
	{
		unsigned int playerId = (*itor).first;
		ComsPlayedMoveMessage *message = (*itor).second;
		if (message->getType() != ComsPlayedMoveMessage::eSkip)
		{
			return false;
		}
	}
	return true;
}

bool ServerShotHolder::haveAllTurnShots()
{
	std::list<unsigned int> &tanks = 
		TurnController::instance()->getPlayersThisTurn();
	std::list<unsigned int>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		unsigned int playerId = (*itor);
		Tank *tank = 
			ScorchedServer::instance()->getTankContainer().getTankById(playerId);
		if (tank && 
			tank->getState().getState() == TankState::sNormal &&
			tank->getDestinationId() != 0)
		{
			if (!haveShot(tank->getPlayerId())) return false;
		}
	}
	return true;
}
