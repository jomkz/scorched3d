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

#if !defined(AFX_Simulator_H__86995B4A_478E_4CFE_BD4C_79128DE51904__INCLUDED_)
#define AFX_Simulator_H__86995B4A_478E_4CFE_BD4C_79128DE51904__INCLUDED_

#include <engine/ActionController.h>
#include <engine/EventContainer.h>
#include <common/RandomGenerator.h>
#include <engine/GameStateI.h>
#include <net/NetBuffer.h>

class ScorchedContext;
class Simulator
{
public:
	Simulator();
	virtual ~Simulator();

	void setScorchedContext(ScorchedContext *context);

	// Accessors
	RandomGenerator &getRandomGenerator() { return random_; }
	ActionController &getActionController() { return actionController_; }

	// Set the simulation speed
	void setFast(fixed speedMult);
	fixed getFast() { return speed_; }

	// Inherited from GameStateI
	void simulate();
	void draw();

	bool writeTimeMessage(NetBuffer &buffer);
	bool readTimeMessage(NetBufferReader &reader);

	bool writeSyncMessage(NetBuffer &buffer);
	bool readSyncMessage(NetBufferReader &reader);

protected:
	bool firstItteration_;
	unsigned int lastTickTime_;
	fixed speed_, stepTime_, totalTime_;
	ScorchedContext *context_;
	EventContainer events_;
	FileRandomGenerator random_;
	ActionController actionController_;

	void actualSimulate(fixed frameTime);
};

class SimulatorGameState : public GameStateI
{
public:
	SimulatorGameState(Simulator *simulator);
	virtual ~SimulatorGameState();

	virtual void simulate(const unsigned state, float simTime);
	virtual void draw(const unsigned state);

private:
	Simulator *simulator_;
};

#endif // !defined(AFX_Simulator_H__86995B4A_478E_4CFE_BD4C_79128DE51904__INCLUDED_)
