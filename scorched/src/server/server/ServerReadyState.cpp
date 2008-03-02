////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
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

#include <server/ServerReadyState.h>
#include <server/ServerNewGameState.h>
#include <server/ServerState.h>
#include <server/ServerShotHolder.h>
#include <server/ScorchedServer.h>
#include <server/ServerCommon.h>
#include <common/OptionsScorched.h>
#include <common/Logger.h>
#include <common/OptionsTransient.h>
#include <coms/ComsPlayerStatusMessage.h>
#include <coms/ComsMessageSender.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>

ServerReadyState::ServerReadyState() : 
	GameStateI("ServerReadyState"),
	time_(0.0f)
{
}

ServerReadyState::~ServerReadyState()
{
}

void ServerReadyState::enterState(const unsigned state)
{
	// Make sure that there are no shots from the last turns
	ServerShotHolder::instance()->clearShots();

	// Add any pending tanks into the game
	int count = ServerNewGameState::addTanksToGame(state);

	// Set the wait timer to the current time
	time_ = 0.0f;

	if (state == ServerState::ServerStateNewGameReady)
	{
		idleTime_ = (float) ScorchedServer::instance()->
			getOptionsGame().getIdleKickTime();
	}
	else
	{
		if (count == 0)
		{
			idleTime_ = (float) ScorchedServer::instance()->
				getOptionsGame().getIdleShotKickTime();
		}
		else
		{
			idleTime_ = (float) ScorchedServer::instance()->
				getOptionsGame().getIdleKickTime();
		}
	}
#ifndef S3D_SERVER
	idleTime_ = 0;
#endif

	// Make all computer players ready
	// And send out the first status messages
	ComsPlayerStatusMessage statusMessage;
	std::map<unsigned int, Tank *> &tanks = 
		ScorchedServer::instance()->getTankContainer().getAllTanks();
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = tanks.begin();
			itor != tanks.end();
			itor++)
	{
		Tank *tank = (*itor).second;
		if (tank->getDestinationId() == 0)
		{
			// Set computer player ready
			tank->getState().setReady();
		}
		else
		{
			// Set all other players not-ready
			statusMessage.getWaitingPlayers().push_back(tank->getPlayerId());
		}
	}

	// Tell clients who we are waiting on
	ComsMessageSender::sendToAllPlayingClients(statusMessage, NetInterfaceFlags::fAsync);	
}

bool ServerReadyState::acceptStateChange(const unsigned state, 
		const unsigned nextState,
		float frameTime)
{
	// Send status messages every 5 seconds
		if (((int) time_) / 5 != ((int) (time_ + frameTime)) / 5)
		{
			// Say who we are waiting on
			ComsPlayerStatusMessage statusMessage;
			std::map<unsigned int, Tank *> &tanks = 
				ScorchedServer::instance()->getTankContainer().getAllTanks();
			std::map<unsigned int, Tank *>::iterator itor;
			for (itor = tanks.begin();
				 itor != tanks.end();
				 itor++)
			{
				Tank *tank = (*itor).second;
				if (tank->getState().getReadyState() == TankState::SNotReady)
				{
					statusMessage.getWaitingPlayers().push_back(tank->getPlayerId());
				}
			}
			ComsMessageSender::sendToAllPlayingClients(statusMessage, NetInterfaceFlags::fAsync);			
		}

	time_ += frameTime;

	// Check all players returned ready
	if(ScorchedServer::instance()->getTankContainer().allReady())
	{
		//Logger::log( "All ready after %.2f seconds", time_);
		finished();
		return true;
	}

	// Check if any players have timed out
	if ((idleTime_ > 0) && (time_ > idleTime_))
	{
		// Kick all not returned players
		std::map<unsigned int, Tank *> &tanks = 
			ScorchedServer::instance()->getTankContainer().getAllTanks();
		std::map<unsigned int, Tank *>::iterator itor;
		for (itor = tanks.begin();
			itor != tanks.end();
			itor++)
		{
			Tank *tank = (*itor).second;
			if (tank->getState().getReadyState() == TankState::SNotReady)
			{
				ServerCommon::sendString(0, 
					S3D::formatStringBuffer("%s kicked for not responding for %.0f seconds", 
						tank->getName(), idleTime_));
				ServerCommon::kickDestination(tank->getDestinationId());
			}
		}

		finished();

		// Stimulate into the next state
		return true;
	}

	return false;
}

void ServerReadyState::finished()
{
	// Set all the tanks to not ready
	ScorchedServer::instance()->getTankContainer().setAllNotReady();

	// Say we are waiting on no one
	ComsPlayerStatusMessage statusMessage;
	ComsMessageSender::sendToAllPlayingClients(statusMessage);	
}
