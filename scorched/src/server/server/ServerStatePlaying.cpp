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
#include <simactions/RoundStartSimAction.h>
#include <simactions/TankTeamBallanceSimAction.h>
#include <common/OptionsScorched.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>

ServerStatePlaying::ServerStatePlaying() : 
	turns_(0), nextRoundId_(0), roundFinished_(false),
	turnsSimultaneous_(true), turnsSimultaneousNoWait_(false)
{
}

ServerStatePlaying::~ServerStatePlaying()
{
}

void ServerStatePlaying::roundFinished()
{
	roundFinished_ = true;
}

bool ServerStatePlaying::showScore()
{
	return roundFinished_ || turns_->finished();
}

void ServerStatePlaying::enterState()
{
	roundFinished_ = false;

	// Start the round
	fixed roundTime = ScorchedServer::instance()->
		getOptionsGame().getRoundTime();
	RoundStartSimAction *roundStart =
		new RoundStartSimAction(++nextRoundId_, roundTime);
	ScorchedServer::instance()->getServerSimulator().
		addSimulatorAction(roundStart);

	// Ballance any teams needing ballanced
	TankTeamBallanceSimAction *teamBallance =
		new TankTeamBallanceSimAction();
	ScorchedServer::instance()->getServerSimulator().
		addSimulatorAction(teamBallance);

	// Reset the auto-sync check timer
	ServerSyncCheck::instance()->enterState();

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

void ServerStatePlaying::simulate(fixed frameTime)
{
	turns_->simulate(frameTime);
}

void ServerStatePlaying::moveFinished(ComsPlayedMoveMessage &playedMessage)
{
	turns_->moveFinished(playedMessage);
}
