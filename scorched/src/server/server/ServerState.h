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

#if !defined(__INCLUDE_ServerStateh_INCLUDE__)
#define __INCLUDE_ServerStateh_INCLUDE__

#include <server/ServerStateEnoughPlayers.h>
#include <server/ServerStateNewGame.h>
#include <server/ServerStateStartingMatch.h>
#include <server/ServerStateBuying.h>
#include <server/ServerStatePlaying.h>
#include <server/ServerStateScore.h>

class ServerState
{
public:
	ServerState();
	virtual ~ServerState();

	enum ServerStateEnum
	{
		ServerStartupState = 1,
		ServerWaitingForPlayersState ,
		ServerMatchCountDownState,
		ServerNewLevelState,
		ServerBuyingState,
		ServerPlayingState,
		ServerScoreState
	};

	void simulate();

	ServerStateEnum getState() { return serverState_; }	

	void buyingFinished(ComsPlayedMoveMessage &message);
	void moveFinished(ComsPlayedMoveMessage &message);
	void shotsFinished(unsigned int moveId);
	void scoreFinished();
	void roundFinished();

protected:
	ServerStateEnum serverState_;

	ServerStateEnoughPlayers enoughPlayers_;
	ServerStateNewGame newGame_;
	ServerStateStartingMatch startingMatch_;
	ServerStateBuying buying_;
	ServerStatePlaying playing_;
	ServerStateScore score_;
};

#endif
