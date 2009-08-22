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

#include <actions/TimerAction.h>
#include <engine/ScorchedContext.h>
#include <tank/TankState.h>
#include <tank/TankContainer.h>
#include <server/ScorchedServer.h>
#include <server/ServerState.h>
#include <coms/ComsPlayedMoveMessage.h>

TimerAction::TimerAction(unsigned int playerId, unsigned int moveId, 
	fixed timeout, bool buying) :
	Action(Action::ACTION_NOT_REFERENCED),
	playerId_(playerId), moveId_(moveId), 
	timeout_(timeout), buying_(buying)
{
}

TimerAction::~TimerAction()
{
}

void TimerAction::init()
{
}

void TimerAction::simulate(fixed frameTime, bool &remove)
{
	timeout_ -= frameTime;

	Tank *tank = context_->getTankContainer().getTankById(playerId_);
	if (!tank || tank->getState().getMoveId() != moveId_)
	{
		remove = true;
	}
	else if (timeout_ < 0)
	{
		remove = true;
		if (context_->getServerMode())
		{
			ComsPlayedMoveMessage message(playerId_, moveId_, ComsPlayedMoveMessage::eTimeout);
			if (buying_)
			{
				ScorchedServer::instance()->getServerState().buyingFinished(message);
			}
			else
			{
				ScorchedServer::instance()->getServerState().moveFinished(message);
			}
		}
	}	
	Action::simulate(frameTime, remove);
}
