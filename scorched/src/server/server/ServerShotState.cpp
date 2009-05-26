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

#include <server/ServerShotState.h>
#include <server/ScorchedServer.h>
#include <server/ServerShotHolder.h>
#include <server/ServerCommon.h>
#include <server/ServerChannelManager.h>
#include <coms/ComsMessageSender.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <tank/TankState.h>
#include <engine/ActionController.h>

ServerShotState::ServerShotState() :
	GameStateI("ServerShotState"),
	shotState_(ScorchedServer::instance()->getContext(), *ServerShotHolder::instance())
{
}

ServerShotState::~ServerShotState()
{
}

void ServerShotState::enterState(const unsigned state)
{
	// Record the players that are actualy playing shots
	playing_.clear();
	std::map<unsigned int, Tank *>::iterator itor;
	std::map<unsigned int, Tank *> tanks = 
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	std::set<unsigned int>::iterator findItor;
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		Tank *tank = (*itor).second;
		if (tank->getState().getTankPlaying() &&
			tank->getDestinationId() != 0)
		{
			unsigned int destination = tank->getDestinationId();
			playing_.insert(destination);
		}
	}

	// Check if all the players have skipped, and only
	// computer AIs are playing
	if (ServerShotHolder::instance()->allSkipped() &&
		ScorchedServer::instance()->getOptionsGame().getTurnType().getValue() == OptionsGame::TurnSimultaneous)
	{
		bool allAis = true;
		bool someAlive = false;
		std::map<unsigned int, Tank *> &tanks = 
			ScorchedServer::instance()->getTankContainer().getPlayingTanks();
		std::map<unsigned int, Tank *>::iterator itor;
		for (itor = tanks.begin();
			itor != tanks.end();
			itor++)
		{
			Tank *tank = (*itor).second;
			if (tank->getState().getState() == TankState::sNormal)
			{
				someAlive = true;
				if (tank->getDestinationId() != 0) 
				{
					allAis = false;
				}
			}
		}

		if (allAis && someAlive)
		{
			ServerChannelManager::instance()->sendText(
				ChannelText("info", 
					"TURN_SKIPPING_STALEMATE", 
					"Skipping all turns due to stalemate..."),
				true);
			ScorchedServer::instance()->getOptionsTransient().startNewRound();
		}
	}

	// Send the player state to all players to ensure that the playing field
	// is consistent before the shots start
	// This should be done before the actual shots are fired or
	// any play is made also
	// Send all of the shots to the client
	unsigned int seed = rand();
	ComsPlayMovesMessage playMovesMessage;
	playMovesMessage.getSeed() = seed;
	ServerShotHolder::instance()->createMessage(playMovesMessage);
	ComsMessageSender::sendToAllPlayingClients(playMovesMessage);

	// Play the shots
	//ScorchedServer::instance()->getTankContainer().setAllNotReady();
	//ScorchedServer::instance()->getActionController().getRandom().seed(seed);
	//shotState_.setup();
}

bool ServerShotState::acceptStateChange(const unsigned state, 
		const unsigned nextState,
		float frameTime)
{
	return shotState_.run(frameTime);
}
