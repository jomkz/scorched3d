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

#include <simactions/TankStartMoveSimAction.h>
#include <server/ScorchedServer.h>
#include <tank/TankAvatar.h>
#include <tank/TankState.h>
#include <tank/TankContainer.h>
#include <actions/TimerAction.h>
#include <tankai/TankAI.h>
#include <engine/ActionController.h>
#include <weapons/AccessoryStore.h>
#ifndef S3D_SERVER
#include <client/ScorchedClient.h>
#include <client/ClientStartGameHandler.h>
#include <actions/VisualTimerAction.h>
#endif

REGISTER_CLASS_SOURCE(TankStartMoveSimAction);

TankStartMoveSimAction::TankStartMoveSimAction()
{
}

TankStartMoveSimAction::TankStartMoveSimAction(unsigned int playerId, unsigned int moveId,
	fixed timeout, bool buying) :
	playerId_(playerId), moveId_(moveId),
	timeout_(timeout), buying_(buying)
{
}

TankStartMoveSimAction::~TankStartMoveSimAction()
{
}

bool TankStartMoveSimAction::invokeAction(ScorchedContext &context)
{
	Tank *tank = context.getTankContainer().getTankById(playerId_);
	if (!tank || tank->getState().getState() != TankState::sNormal) return true;

	if (!context.getServerMode())
	{
		tank->getState().setMoveId(moveId_);

		if (tank->getDestinationId() == context.getTankContainer().getCurrentDestinationId())
		{
#ifndef S3D_SERVER
			ClientStartGameHandler::instance()->startGame(this);

			if (timeout_ > 0)
			{
				VisualTimerAction *timerAction = 
					new VisualTimerAction(playerId_, moveId_, timeout_, buying_);
				context.getActionController().addAction(timerAction);
			}
#endif
		}
	}
	else
	{
		if (tank->getDestinationId() == 0)
		{
			if (buying_)
			{
				tank->getTankAI()->buyAccessories(moveId_);
			}
			else
			{
				tank->getTankAI()->playMove(moveId_);
			}
		}
		else
		{
			if (timeout_ > 0)
			{
				TimerAction *timerAction = new TimerAction(playerId_, moveId_, timeout_, buying_);
				context.getActionController().addAction(timerAction);
			}
		}
	}

	return true;
}

bool TankStartMoveSimAction::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(playerId_);
	buffer.addToBuffer(moveId_);
	buffer.addToBuffer(buying_);
	buffer.addToBuffer(timeout_);
	if (buying_)
	{
		if (!ScorchedServer::instance()->getAccessoryStore().
			writeEconomyToBuffer(buffer)) return false;
	}
	return true;
}

bool TankStartMoveSimAction::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(playerId_)) return false;
	if (!reader.getFromBuffer(moveId_)) return false;
	if (!reader.getFromBuffer(buying_)) return false;
	if (!reader.getFromBuffer(timeout_)) return false;
	if (buying_)
	{
#ifndef S3D_SERVER
		if (!ScorchedClient::instance()->getAccessoryStore().
			readEconomyFromBuffer(reader)) return false;
#endif
	}
	return true;
}