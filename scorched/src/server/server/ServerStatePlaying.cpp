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

#include <server/ServerStatePlaying.h>
#include <server/ScorchedServer.h>
#include <server/ServerSyncCheck.h>
#include <server/ServerChannelManager.h>
#include <server/ServerCommon.h>
#include <common/OptionsScorched.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>

ServerStatePlaying::ServerStatePlaying() : 
	turns_(0)
{
}

ServerStatePlaying::~ServerStatePlaying()
{
}

bool ServerStatePlaying::showScore()
{
	return turns_->finished();
}

void ServerStatePlaying::enterState()
{
	ServerSyncCheck::instance()->enterState();
	switch (ScorchedServer::instance()->getOptionsGame().getTurnType().getValue())
	{
	case OptionsGame::TurnSequentialLooserFirst:
	case OptionsGame::TurnSequentialRandom:
		turns_ = &turnsSequential_;
		break;
	default:
		turns_ = &turnsSimultaneous_;
		break;
	}

	turns_->enterState();
}

void ServerStatePlaying::simulate()
{
	turns_->simulate();
}

void ServerStatePlaying::moveFinished(ComsPlayedMoveMessage &playedMessage)
{
	Tank *tank = ScorchedServer::instance()->getTankContainer().
		getTankById(playedMessage.getPlayerId());
	if (playedMessage.getType() == ComsPlayedMoveMessage::eTimeout)
	{
		int allowedMissed = 
			ScorchedServer::instance()->getOptionsGame().getAllowedMissedMoves();
		if (tank && allowedMissed > 0)
		{
			tank->getScore().setMissedMoves(
				tank->getScore().getMissedMoves() + 1);

			ServerChannelManager::instance()->sendText(
				ChannelText("info",
					"PLAYER_MISSED_SHOOT",
					"Player \"{0}\" failed to move, allowed {1} more missed move(s)",
					tank->getTargetName(),
					allowedMissed - tank->getScore().getMissedMoves()),
					true);

			if (tank->getScore().getMissedMoves() >= allowedMissed)
			{
				// Then kick this player
				ServerCommon::kickDestination(tank->getDestinationId());
			}
		}
	}
	else
	{
		if (tank)
		{
			tank->getScore().setMissedMoves(0);
		}
	}
	turns_->moveFinished(playedMessage);
}

void ServerStatePlaying::shotsFinished(unsigned int moveId)
{
	turns_->shotsFinished(moveId);
}
