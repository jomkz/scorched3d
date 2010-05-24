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

#if !defined(__INCLUDE_ServerStateBuyingh_INCLUDE__)
#define __INCLUDE_ServerStateBuyingh_INCLUDE__

#include <engine/SimulatorI.h>
#include <set>
#include <map>
#include <server/ServerTurnsSimultaneous.h>
#include <coms/ComsPlayedMoveMessage.h>

class ServerStateBuying
{
public:
	ServerStateBuying();
	virtual ~ServerStateBuying();

	void enterState();
	bool simulate(fixed frameTime);

	void buyingFinished(ComsPlayedMoveMessage &playedMessage);

protected:
	struct BuyingPlayer
	{
		BuyingPlayer(
			unsigned int moveId,
			fixed moveTime) :
			startedMove_(false),
			moveId_(moveId),
			moveTime_(moveTime)
		{
		}

		unsigned int moveId_;
		bool startedMove_;
		fixed moveTime_;
	};

	fixed totalTime_;
	unsigned int nextMoveId_;
	std::set<unsigned int> boughtPlayers_;
	SimulatorIAdapter<ServerStateBuying> *buyingStarted_;
	std::map<unsigned int, BuyingPlayer*> buyingPlayers_;

	void playerBuying(unsigned int playerId);
	void buyingStarted(fixed simulationTime, SimAction *action);
};

#endif
