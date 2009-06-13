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

#if !defined(__INCLUDE_TankStateh_INCLUDE__)
#define __INCLUDE_TankStateh_INCLUDE__

#include <net/NetBuffer.h>

class Tank;
class ScorchedContext;
class TankState
{
public:
	enum State
	{
		sSpectator,
		sNormal,
		sDead
	};
	enum ServerState
	{
		serverJoined,
		serverNone,
		serverMakingMove
	};

	TankState(ScorchedContext &context, unsigned int playerId);
	virtual ~TankState();
	
	void setTank(Tank *tank) { tank_ = tank; }

	// State Modifiers
	void newGame();
	void newMatch();
	void clientNewGame();

	// State
	void setState(State s);
	State getState() { return state_; }
	void setServerState(ServerState s);
	ServerState getServerState() { return serverState_; }

	bool getTankPlaying();

	void setMuted(bool muted) { muted_ = muted; }
	bool getMuted() { return muted_; }
	void setSkipShots(bool skip) { skipshots_ = skip; }
	bool getSkipShots() { return skipshots_; }
	int getLives() { return lives_; }
	void setLives(int lives) { lives_ = lives; }
	int getMaxLives() { return maxLives_; }
	void setDestroy(bool destroy) { destroy_ = destroy; }
	bool getDestroy() { return destroy_; }
	
	const char *getStateString();
	const char *getSmallStateString();
	LangString &getSmallStateLangString();

	// Serialize the tank
	bool writeMessage(NetBuffer &buffer);
	bool readMessage(NetBufferReader &reader);

protected:
	Tank *tank_;
	ScorchedContext &context_;
	State state_;
	ServerState serverState_;
	int lives_, maxLives_;
	bool muted_;
	bool skipshots_;
	bool destroy_;

};

#endif

