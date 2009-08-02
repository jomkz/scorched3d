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

#if !defined(__INCLUDE_ServerTurnsh_INCLUDE__)
#define __INCLUDE_ServerTurnsh_INCLUDE__

class Tank;
class ComsPlayedMoveMessage;
class ServerTurns 
{
public:
	ServerTurns();
	virtual ~ServerTurns();

	virtual void enterState() = 0;
	virtual void simulate() = 0;
	virtual bool finished() = 0;

	virtual void moveFinished(ComsPlayedMoveMessage &playedMessage) = 0;
	virtual void shotsFinished(unsigned int moveId) = 0;

protected:
	bool showScore();

	void playMove(Tank *tank, unsigned int moveId);
};

#endif
