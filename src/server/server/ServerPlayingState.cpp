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

#include <server/ServerPlayingState.h>
#include <server/ServerShotHolder.h>
#include <server/ServerState.h>
#include <server/ScorchedServer.h>
#include <server/ServerChannelManager.h>
#include <server/TurnController.h>
#include <server/ServerCommon.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <common/OptionsScorched.h>
#include <common/Logger.h>

ServerPlayingState::ServerPlayingState() : 
	GameStateI("ServerPlayingState"),
	time_(0.0f)
{
}

ServerPlayingState::~ServerPlayingState()
{
}

void ServerPlayingState::enterState(const unsigned state)
{
	// Set the wait timer to the current time
	time_ = 0.0f;
}

bool ServerPlayingState::acceptStateChange(const unsigned state, 
		const unsigned nextState,
		float frameTime)
{
	// Check how long we are allowed to wait
	time_ += frameTime;
	int shotTime = 0;
	if (state == ServerState::ServerStateBuying)
	{
		shotTime = ScorchedServer::instance()->getOptionsGame().getBuyingTime();
	}
	else
	{
		shotTime = ScorchedServer::instance()->getOptionsGame().getShotTime();
	}

	// Check if the time to make the shots has expired
	if (shotTime > 0)
	{
		if (time_ > shotTime + 5)
		{
			// For each alive tank that should have made a move
			// Check if the tank has missed its go
			// If so increment the missed counter
			// Once missed counter exceeds it threshold then kick the player
			std::list<unsigned int> &tanks = 
				TurnController::instance()->getPlayersThisTurn();
			std::list<unsigned int>::iterator itor;
			for (itor = tanks.begin();
				itor != tanks.end();
				itor++)
			{
				Tank *tank = ScorchedServer::instance()->getTankContainer().getTankById(*itor);
				if (tank && tank->getState().getState() == TankState::sNormal)
				{
					if (!ServerShotHolder::instance()->haveShot(
						tank->getPlayerId()))
					{
						int movesMissed = tank->getScore().getMissedMoves() + 1;
						tank->getScore().setMissedMoves(movesMissed);

#ifdef S3D_SERVER
						// If the allowed missed moves has been specified
						if (ScorchedServer::instance()->getOptionsGame().getAllowedMissedMoves() > 0)
						{
							ServerChannelManager::instance()->sendText(
								ChannelText("info",
									"Player \"{0}\" failed to {1}, allowed {2} more missed move(s)",
									tank->getName(),
									((state == ServerState::ServerStateBuying)?"buy":"move"),
									ScorchedServer::instance()->getOptionsGame().getAllowedMissedMoves() - movesMissed),
								true);

							// And this player has exceeded them
							if (movesMissed >= ScorchedServer::instance()->getOptionsGame().getAllowedMissedMoves())
							{
								// Then kick this player
								ServerCommon::kickDestination(tank->getDestinationId());
							}
						}
#endif
					}
				}
			}

			return true;
		}
	}

	// Or we already have all shots
	if (ServerShotHolder::instance()->haveAllTurnShots())
	{
		return true;
	}

	return false;
}
