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

#if !defined(__INCLUDE_ServerStatePlayingh_INCLUDE__)
#define __INCLUDE_ServerStatePlayingh_INCLUDE__

#include <server/ServerTurnsSequential.h>
#include <server/ServerTurnsSimultaneous.h>
#include <coms/ComsPlayedMoveMessage.h>
#include <map>

class ServerStatePlaying : public ServerTurnsI
{
public:
	static ServerStatePlaying *instance();

	ServerStatePlaying();
	virtual ~ServerStatePlaying();

	void enterState();
	void simulate(float frameTime);

	void playerFinishedPlaying(ComsPlayedMoveMessage &playedMessage);
	void shotsFinished(unsigned int moveId);

	// ServerTurnsI
	virtual void playMoves();
	virtual void playerPlaying(unsigned int playerId);

protected:
	static unsigned int moveId_;
	static ServerStatePlaying *instance_;
	ServerTurnsSequential turnsSequential_;
	ServerTurnsSimultaneous turnsSimultaneous_;
	ServerTurns *turns_;
	std::map<unsigned int, ComsPlayedMoveMessage *> messages_;
	bool simulatingShots_;

	void clear();
	void playShots();
	void simulatePlaying(float frameTime);
};

#endif
