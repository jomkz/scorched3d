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

#include <server/ServerNextRoundState.h>
#include <server/ServerState.h>
#include <server/ServerShotHolder.h>
#include <server/ScorchedServer.h>
#include <server/TurnController.h>
#include <tank/TankContainer.h>
#include <engine/ActionController.h>
#include <coms/ComsMessageSender.h>
#include <coms/ComsGameStateMessage.h>
#include <common/Logger.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <common/StatsLogger.h>
#include <lua/LUAScriptHook.h>

ServerNextRoundState::ServerNextRoundState() :
	GameStateI("ServerNextRoundState")
{
	ScorchedServer::instance()->getLUAScriptHook().addHookProvider("server_newround");
}

ServerNextRoundState::~ServerNextRoundState()
{
}

void ServerNextRoundState::enterState(const unsigned state)
{
	// Move all tanks into the next round
	// Load the set of options for this next player
	ScorchedServer::instance()->getContext().getOptionsTransient().nextRound();

	// Tell the stats about the start of a new round
	bool weaponBuy = 
		(ScorchedServer::instance()->getOptionsTransient().getCurrentGameNo() == 0);
	if (!weaponBuy)
	{
		std::list<Tank *> currentTanks;
		std::map<unsigned int, Tank *> &playingTanks =
			ScorchedServer::instance()->getTankContainer().getPlayingTanks();
		std::map<unsigned int, Tank *>::iterator playingTanksItor;
		for (playingTanksItor = playingTanks.begin();
			playingTanksItor != playingTanks.end();
			playingTanksItor++)
		{
			Tank *tank = (*playingTanksItor).second;
			currentTanks.push_back(tank);
		}
		StatsLogger::instance()->roundStart(currentTanks);
	}

	// Setup this list of players that need to move before this round is over
	TurnController::instance()->nextRound();

	// Notify scripts of a new game starting
	ScorchedServer::instance()->getLUAScriptHook().callHook("server_newround");

	// Make sure all clients have the correct game settings
	ComsGameStateMessage message;
	ComsMessageSender::sendToAllLoadedClients(message);	

	// Move into the ready state
	//ScorchedServer::instance()->getGameState().stimulate(ServerState::ServerStimulusNextShot);
}
