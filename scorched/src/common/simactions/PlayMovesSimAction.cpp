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

#include <simactions/PlayMovesSimAction.h>
#include <engine/ActionController.h>
#include <weapons/AccessoryStore.h>
#include <tank/TankAvatar.h>
#include <tank/TankState.h>
#include <tank/TankPosition.h>
#include <tank/TankAccessories.h>
#include <tank/TankContainer.h>
#include <target/TargetRenderer.h>
#include <tankai/TankAIStrings.h>
#include <actions/TankSay.h>
#include <actions/ShotFinishedAction.h>
#include <common/StatsLogger.h>
#ifndef S3D_SERVER
	#include <sound/SoundUtils.h>
#endif

REGISTER_CLASS_SOURCE(PlayMovesSimAction);

PlayMovesSimAction::PlayMovesSimAction() :
	moveId_(0)
{
}

PlayMovesSimAction::PlayMovesSimAction(unsigned int moveId) :
	moveId_(moveId)
{
}

PlayMovesSimAction::~PlayMovesSimAction()
{
	while (!messages_.empty())
	{
		delete messages_.back();
		messages_.pop_back();
	}
}

void PlayMovesSimAction::addMove(ComsPlayedMoveMessage *message)
{
	messages_.push_back(message);
}

bool PlayMovesSimAction::invokeAction(ScorchedContext &context)
{
	context.getActionController().addAction(new ShotFinishedAction(moveId_));
	
	std::list<ComsPlayedMoveMessage *>::iterator itor;
	for (itor = messages_.begin();
		itor != messages_.end();
		itor++)
	{
		ComsPlayedMoveMessage *message = *itor;
		Tank *tank = context.getTankContainer().getTankById(message->getPlayerId());
		if (tank && tank->getState().getState() == TankState::sNormal)
		{
			switch (message->getType())
			{
			case ComsPlayedMoveMessage::eShot:
				tankFired(context, tank, *message);
				break;
			}
		}
	}
	return true;
}

void PlayMovesSimAction::tankFired(ScorchedContext &context, 
	Tank *tank, ComsPlayedMoveMessage &message)
{
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
	tank->getPosition().madeShot();

	// Tank say
	if (tank->getDestinationId() == 0)
	{
		const char *line = TankAIStrings::instance()->getAttackLine(context);
		if (line)
		{
			context.getActionController().addAction(
				new TankSay(tank->getPlayerId(), 
				LANG_STRING(line)));
		}
	}

#ifndef S3D_SERVER
	if (context.getServerMode()) 
	{
		TargetRenderer *renderer = tank->getRenderer();
		if (renderer)
		{
			renderer->fired();
		}

		// play fired sound
		if (weapon->getParent()->getActivationSound() &&
			0 != strcmp("none", weapon->getParent()->getActivationSound()))
		{
			SoundBuffer *firedSound = 
				Sound::instance()->fetchOrCreateBuffer(
					S3D::getModFile(S3D::formatStringBuffer("data/wav/%s", 
					weapon->getParent()->getActivationSound())));
			SoundUtils::playAbsoluteSound(VirtualSoundPriority::eAction,
				firedSound, tank->getPosition().getTankPosition().asVector());
		}
	}
#endif // #ifndef S3D_SERVER

	// Get firing context
	WeaponFireContext weaponContext(tank->getPlayerId(), 0);
	FixedVector velocity = tank->getPosition().getVelocityVector() *
		(tank->getPosition().getPower() + 1);
	FixedVector position = tank->getPosition().getTankGunPosition();

	// Create an action for the muzzle flash
	// add it to the action controller
	if (weapon->getParent()->getMuzzleFlash())
	{
		Weapon *muzzleFlash = context.getAccessoryStore().getMuzzelFlash();
		if (muzzleFlash) muzzleFlash->fireWeapon(context, weaponContext, position, velocity);
	}

	// Create the action for the weapon and
	// add it to the action controller
	weapon->fireWeapon(context, weaponContext, position, velocity);

	// Stats events
	StatsLogger::instance()->tankFired(tank, weapon);
	StatsLogger::instance()->weaponFired(weapon, false);	
}

bool PlayMovesSimAction::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(moveId_);
	buffer.addToBuffer((unsigned int) messages_.size());
	std::list<ComsPlayedMoveMessage *>::iterator itor;
	for (itor = messages_.begin();
		itor != messages_.end();
		itor++)
	{
		ComsPlayedMoveMessage *message = *itor;
		message->writeMessage(buffer);
	}

	return true;
}

bool PlayMovesSimAction::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(moveId_)) return false;
	unsigned int size = 0;
	if (!reader.getFromBuffer(size)) return false;
	for (unsigned int s=0; s<size; s++)
	{
		ComsPlayedMoveMessage *message = new ComsPlayedMoveMessage();
		if (!message->readMessage(reader)) return false;
		messages_.push_back(message);
	}
	return true;
}
