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

#if !defined(__INCLUDE_ScorchedClienth_INCLUDE__)
#define __INCLUDE_ScorchedClienth_INCLUDE__

#include <engine/ScorchedContext.h>

class MainLoop;
class ParticleEngine;
class GameState;
class SimulatorGameState;
class ScorchedClient : public ScorchedContext
{
public:
	static ScorchedClient *instance();

	SimulatorGameState &getSimulatorGameState() { return *simulatorGameState_; }
	MainLoop &getMainLoop() { return *mainLoop_; }
	ScorchedContext &getContext() { return *this; }
	ParticleEngine &getParticleEngine() { return *particleEngine_; }
	GameState &getGameState() { return *gameState; }

protected:
	static ScorchedClient *instance_;
	MainLoop *mainLoop_;
	ParticleEngine* particleEngine_;
	GameState *gameState;
	SimulatorGameState *simulatorGameState_;

private:
	ScorchedClient();
	virtual ~ScorchedClient();
};

#endif
