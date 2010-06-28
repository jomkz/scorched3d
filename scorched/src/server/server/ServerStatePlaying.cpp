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
#include <server/ServerSimulator.h>
#include <server/ServerChannelManager.h>
#include <simactions/RoundStartSimAction.h>
#include <simactions/TankTeamBallanceSimAction.h>
#include <common/OptionsScorched.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>

ServerStatePlaying::ServerStatePlaying() : 
	turns_(0), nextRoundId_(0), roundState_(eNone),
	turnsSimultaneous_(true), turnsSimultaneousNoWait_(false)
{
	roundStarted_ = new SimulatorIAdapter<ServerStatePlaying>(this,  &ServerStatePlaying::roundStarted);
}

ServerStatePlaying::~ServerStatePlaying()
{
}

bool ServerStatePlaying::showScore()
{
	return (roundState_ == eFinished) || turns_->finished();
}

void ServerStatePlaying::enterState()
{
	// Start the round
	roundState_ = eNone;
	roundTime_ = ScorchedServer::instance()->
		getOptionsGame().getRoundTime();
	RoundStartSimAction *roundStart =
		new RoundStartSimAction(++nextRoundId_, roundTime_);
	ScorchedServer::instance()->getServerSimulator().
		addSimulatorAction(roundStart, roundTime_>0?roundStarted_:0);

	// Ballance any teams needing ballanced
	TankTeamBallanceSimAction *teamBallance =
		new TankTeamBallanceSimAction();
	ScorchedServer::instance()->getServerSimulator().
		addSimulatorAction(teamBallance);

	// Reset the auto-sync check timer
	ScorchedServer::instance()->getServerSyncCheck().enterState();

	// Setup the correct turn algorithm for the game
	switch (ScorchedServer::instance()->getOptionsGame().getTurnType().getValue())
	{
	case OptionsGame::TurnSequentialLooserFirst:
	case OptionsGame::TurnSequentialRandom:
		turns_ = &turnsSequential_;
		break;
	case OptionsGame::TurnFree:
	case OptionsGame::TurnFreeTimed:
		turns_ = &turnsFree_;
		break;
	case OptionsGame::TurnSimultaneousNoWait:
		turns_ = &turnsSimultaneousNoWait_;
		break;
	default:
		turns_ = &turnsSimultaneous_;
		break;
	}

	// Start the turns
	turns_->enterState();
}

void ServerStatePlaying::roundStarted(fixed simulationTime, SimAction *action)
{
	roundState_ = eCountingDown;
}

void ServerStatePlaying::simulate(fixed frameTime)
{
	turns_->simulate(frameTime);

	if (roundState_ == eCountingDown)
	{
		roundTime_ -= frameTime;
		if (roundTime_ < 0) 
		{
			ChannelText text("info",
				"ROUND_FINISHED_TIME",
				"Round finished due to round time out");
			ScorchedServer::instance()->getServerChannelManager().sendText(text, true);

			roundState_ = eFinished;
		}
	}
}

void ServerStatePlaying::moveFinished(ComsPlayedMoveMessage &playedMessage)
{
	turns_->moveFinished(playedMessage);
}
