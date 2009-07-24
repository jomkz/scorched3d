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

#if !defined(__INCLUDE_ServerTurnsSequentialh_INCLUDE__)
#define __INCLUDE_ServerTurnsSequentialh_INCLUDE__

#include <list>
#include <server/ServerTurns.h>

class ServerTurnsSequential : public ServerTurns
{
public:
	ServerTurnsSequential();
	virtual ~ServerTurnsSequential();

	virtual void newGame();
	virtual void nextMove();

	virtual bool playerFinished(unsigned int playerId);

	virtual void simulate(float frameTime);

protected:
	bool playerFinished_;
	unsigned int playingPlayer_;
	std::list<unsigned int> waitingPlayers_;
};

#endif
