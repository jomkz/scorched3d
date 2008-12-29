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

#include <server/ServerStartingState.h>
#include <server/ScorchedServer.h>
#include <server/ServerNewGameState.h>
#include <server/ServerChannelManager.h>
#include <server/ServerState.h>
#include <server/ServerCommon.h>
#include <coms/ComsMessageSender.h>
#include <common/OptionsScorched.h>
#include <common/Logger.h>

ServerStartingState::ServerStartingState() :
	GameStateI("ServerStartingState")
{
}

ServerStartingState::~ServerStartingState()
{
}

void ServerStartingState::enterState(const unsigned state)
{
	timeLeft_ = (float) ScorchedServer::instance()->getOptionsGame().getStartTime();
}

bool ServerStartingState::acceptStateChange(const unsigned state, 
		const unsigned nextState,
		float frameTime)
{
	float startTime = timeLeft_;
	timeLeft_ -= frameTime;

	if (frameTime > 0.0f && int(timeLeft_) != int(startTime))
	{
		if (int(startTime) % 5 == 0)
		{
			ServerChannelManager::instance()->sendText(
				ChannelText("info", 
					"GAME_STARTING_IN_X", 
					"Game starting in {0} seconds...", 
					timeLeft_),
				false);
		}
	}

	// Check if we need to add any new bots
	ServerNewGameState::checkBots(true);

	return (timeLeft_ < 0.0f);
}
