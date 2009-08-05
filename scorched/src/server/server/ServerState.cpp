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

#include <server/ServerState.h>

ServerState::ServerState() :
	serverState_(ServerStartupState)
{
}

ServerState::~ServerState()
{
}

void ServerState::simulate()
{
	switch (serverState_)
	{
	case ServerStartupState:
		newGame_.newGame();
		serverState_ = ServerWaitingForPlayersState;
		break;
	case ServerWaitingForPlayersState:
		newGame_.checkTeams();
		newGame_.checkBots(true);
		if (enoughPlayers_.enoughPlayers())
		{
			startingMatch_.reset();
			serverState_ = ServerMatchCountDownState;
		}
		break;
	case ServerMatchCountDownState:
		newGame_.checkTeams();
		newGame_.checkBots(true);
		if (enoughPlayers_.enoughPlayers())
		{
			if (startingMatch_.startingMatch())
			{
				serverState_ = ServerNewLevelState;
			}
		}
		else
		{
			startingMatch_.stoppingMatch();
			serverState_ = ServerWaitingForPlayersState;
		}
		break;
	case ServerNewLevelState:
		newGame_.newGame();

		serverState_ = ServerBuyingState;
		buying_.enterState();
		break;
	case ServerBuyingState:
		if (buying_.simulate())
		{
			serverState_ = ServerPlayingState;
			playing_.enterState();
		}
		break;
	case ServerPlayingState:
		if (enoughPlayers_.enoughPlayers())
		{
			if (playing_.showScore())
			{
				serverState_ = ServerScoreState;
				score_.enterState();
			}
			else 
			{
				playing_.simulate();
			}
		}
		else
		{
			startingMatch_.stoppingMatch();
			serverState_ = ServerWaitingForPlayersState;
		}
		
		break;
	case ServerScoreState:
		if (score_.simulate())
		{
			if (score_.overAllWinner())
			{
				serverState_ = ServerWaitingForPlayersState;
			}
			else
			{
				serverState_ = ServerNewLevelState;
			}
		}
		break;
	}
}

void ServerState::buyingFinished(ComsPlayedMoveMessage &message)
{
	buying_.buyingFinished(message);
}

void ServerState::moveFinished(ComsPlayedMoveMessage &message)
{
	playing_.moveFinished(message);
}

void ServerState::shotsFinished(unsigned int moveId)
{
	playing_.shotsFinished(moveId);
}

void ServerState::scoreFinished()
{
	score_.scoreFinished();
}
