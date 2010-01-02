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

#include <server/ServerStateScore.h>
#include <server/ServerStateEnoughPlayers.h>
#include <server/ScorchedServer.h>
#include <server/ServerSimulator.h>
#include <server/ServerSyncCheck.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <simactions/ShowScoreSimAction.h>
#include <simactions/RoundStopSimAction.h>
#include <lua/LUAScriptHook.h>

ServerStateScore::ServerStateScore()
{
}

ServerStateScore::~ServerStateScore()
{
}

void ServerStateScore::enterState(ServerStateEnoughPlayers &enoughPlayers)
{
	// End the round
	RoundStopSimAction *roundStop =
		new RoundStopSimAction();
	ScorchedServer::instance()->getServerSimulator().
		addSimulatorAction(roundStop);

	// Check if we've finished all the rounds
	finished_ = false;
	overAllWinner_ = false;
	if (ScorchedServer::instance()->getOptionsTransient().getCurrentRoundNo() >=
		ScorchedServer::instance()->getOptionsGame().getNoRounds())
	{
		overAllWinner_ = true;
	}
	else
	{
		if (!enoughPlayers.enoughPlayers())
		{
			if (ScorchedServer::instance()->getOptionsTransient().getCurrentRoundNo() >
				ScorchedServer::instance()->getOptionsGame().getNoRounds() / 2 && 
				ScorchedServer::instance()->getOptionsTransient().getCurrentRoundNo() > 2)
			{
				overAllWinner_ = true;
			}
		}
	}

	fixed scoreTime = 0;
	if (overAllWinner_)
	{
		scoreTime = fixed(ScorchedServer::instance()->getOptionsGame().getScoreTime());
	}
	else
	{
		scoreTime = fixed(ScorchedServer::instance()->getOptionsGame().getRoundScoreTime());
	}

	// Tell scripts to score 
	ScorchedServer::instance()->getLUAScriptHook().callHook("server_score", overAllWinner_);

	// Add score simaction
	fixed(ScorchedServer::instance()->getOptionsGame().getScoreTime());
	ShowScoreSimAction *showScoreAction = new ShowScoreSimAction(scoreTime, overAllWinner_);
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(showScoreAction);

	// Force clients to check (if enabled)
	ServerSyncCheck::instance()->sendAutoSyncCheck();
}

void ServerStateScore::scoreFinished()
{
	finished_ = true;
}

bool ServerStateScore::overAllWinner()
{
	return overAllWinner_;
}

bool ServerStateScore::simulate()
{
	if (finished_)
	{
		bool actions = ScorchedServer::instance()->getServerSimulator().
			getSendActionsEmpty();
		return actions;
	}
	return false;
}
