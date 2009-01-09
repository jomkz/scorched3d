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

#include <engine/PlayShots.h>
#include <engine/ScorchedContext.h>
#include <engine/ActionController.h>
#include <tank/TankScore.h>
#include <tank/TankState.h>
#include <tank/TankAccessories.h>
#include <tank/TankPosition.h>
#include <actions/TankFired.h>
#include <actions/TankResign.h>
#include <target/TargetLife.h>
#include <common/OptionsScorched.h>
#include <common/StatsLogger.h>
#include <weapons/AccessoryStore.h>

PlayShots::PlayShots()
{
}

PlayShots::~PlayShots()
{
}

void PlayShots::createMessage(ComsPlayMovesMessage &message)
{
	std::map<unsigned int, ComsPlayedMoveMessage *>::iterator itor;
	for (itor = messages_.begin();
		itor != messages_.end();
		itor++)
	{
		message.getMoves()[(*itor).first] = (*itor).second;
	}
}

void PlayShots::readMessage(ComsPlayMovesMessage &message)
{
	clearShots();
	std::map<unsigned int, ComsPlayedMoveMessage *>::iterator itor;
	for (itor = message.getMoves().begin();
		itor != message.getMoves().end();
		itor++)
	{
		messages_[(*itor).first] = (*itor).second;
	}
}

void PlayShots::clearShots()
{
	std::map<unsigned int, ComsPlayedMoveMessage *>::iterator itor;
	for (itor = messages_.begin();
		itor != messages_.end();
		itor++)
	{
		delete (*itor).second;
	}
	messages_.clear();
}

bool PlayShots::haveShot(unsigned int playerId)
{
	std::map<unsigned int, ComsPlayedMoveMessage *>::iterator itor =
		messages_.find(playerId);
	return (itor != messages_.end());
}

void PlayShots::playShots(ScorchedContext &context)
{
	std::map<unsigned int, ComsPlayedMoveMessage *>::iterator itor;
	for (itor = messages_.begin();
		itor != messages_.end();
		itor++)
	{
		unsigned int playerId = (*itor).first;
		ComsPlayedMoveMessage *message = (*itor).second;

		// Check the tank exists for this player
		// It may not if the player has left the game after firing.
		Tank *tank = context.getTankContainer().getTankById(playerId);
		if (tank)
		{
			// This tank has now made a move, reset its missed move counter
			tank->getScore().setMissedMoves(0);

			// Actually play the move
			processPlayedMoveMessage(context, *message, tank);
		}
	}
}

void PlayShots::processPlayedMoveMessage(ScorchedContext &context, 
	ComsPlayedMoveMessage &message, Tank *tank)
{
	// All actions that are done at the very START of a new round
	switch (message.getType())
	{
		case ComsPlayedMoveMessage::eShot:
			processFiredMessage(context, message, tank);
			break;
		case ComsPlayedMoveMessage::eSkip:
			// Just do nothing as the player has requested
			// That they skip their move
			break;
		case ComsPlayedMoveMessage::eFinishedBuy:
			// Just used as a notification that the player
			// has finished buying, do nothing
			break;
		case ComsPlayedMoveMessage::eResign:
			processResignMessage(context, message, tank);
			break;
		default:
			break;
	}
}

void PlayShots::processResignMessage(ScorchedContext &context, 
	ComsPlayedMoveMessage &message, Tank *tank)
{
	TankResign *resign = new TankResign(tank->getPlayerId());
	if (context.getOptionsGame().getResignMode() == OptionsGame::ResignStart)
	{					
		context.getActionController().addAction(resign);
	}
	else if (context.getOptionsGame().getResignMode() == OptionsGame::ResignDueToHealth)
	{
		if (tank->getLife().getMaxLife() / 2 <= tank->getLife().getLife())
		{
			context.getActionController().addAction(resign);
		}
		else
		{
			context.getActionController().addLastAction(resign);
		}
	}
	else 
	{
		context.getActionController().addLastAction(resign);
	}
}

void PlayShots::processFiredMessage(ScorchedContext &context, 
	ComsPlayedMoveMessage &message, Tank *tank)
{
	// Check the tank is alive
	if (tank->getState().getState() != TankState::sNormal)
	{
		return;
	}

	// Check the weapon name exists and is a weapon
	Accessory *accessory = 
		context.getAccessoryStore().findByAccessoryId(message.getWeaponId());
	if (!accessory) return;

	Weapon *weapon = (Weapon *) accessory->getAction();
	if (accessory->getUseNumber() > 0)
	{
		// Actually use up one of the weapons
		// Fuel, is used up differently at the rate of one weapon per movement square
		// This is done sperately in the tank movement action
		tank->getAccessories().rm(accessory, accessory->getUseNumber());
	}

	// shot fired action
	TankFired *fired = new TankFired(tank->getPlayerId(), 
		weapon,
		message.getRotationXY(), message.getRotationYZ());
	context.getActionController().addAction(fired);

	// Set the tank to have the correct rotation etc..
	tank->getPosition().rotateGunXY(
		message.getRotationXY(), false);
	tank->getPosition().rotateGunYZ(
		message.getRotationYZ(), false);
	tank->getPosition().changePower(
		message.getPower(), false);
	tank->getPosition().setSelectPosition(
		message.getSelectPositionX(), 
		message.getSelectPositionY());

	// Create the action for the weapon and
	// add it to the action controller
	FixedVector velocity = tank->getPosition().getVelocityVector() *
		(tank->getPosition().getPower() + 1);
	FixedVector position = tank->getPosition().getTankGunPosition();

	WeaponFireContext weaponContext(tank->getPlayerId(), 0);
	weapon->fireWeapon(context, weaponContext, position, velocity);
	StatsLogger::instance()->tankFired(tank, weapon);
	StatsLogger::instance()->weaponFired(weapon, false);
}
