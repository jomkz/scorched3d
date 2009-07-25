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

#include <server/ServerTurnsSimultaneous.h>
#include <coms/ComsPlayedMoveMessage.h>

class ServerStateBuying : public ServerTurnsI
{
public:
	ServerStateBuying();
	virtual ~ServerStateBuying();

	void enterState();
	bool simulate();

	void buyingFinished(ComsPlayedMoveMessage &playedMessage);

	// ServerTurnsI
	virtual void playMoves();
	virtual void playerPlaying(unsigned int playerId);

protected:
	static unsigned int moveId_;
	ServerTurnsSimultaneous simulTurns_;
	bool finished_;
};

#endif
