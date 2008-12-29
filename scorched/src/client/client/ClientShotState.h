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


#if !defined(__INCLUDE_ClientShotStateh_INCLUDE__)
#define __INCLUDE_ClientShotStateh_INCLUDE__

#include <coms/ComsMessageHandler.h>
#include <engine/PlayShots.h>
#include <engine/GameStateStimulusI.h>
#include <engine/GameStateI.h>
#include <engine/ShotState.h>

class ClientShotState : 
	public GameStateStimulusI,
	public GameStateI,
	public ComsMessageHandlerI
{
public:
	static ClientShotState *instance();

	virtual bool processMessage(
		NetMessage &message,
		const char *messageType,
		NetBufferReader &reader);

	virtual void enterState(const unsigned state);
	virtual bool acceptStateChange(const unsigned state, 
		const unsigned nextState,
		float frameTime);

protected:
	static ClientShotState *instance_;
	PlayShots playShots_;
	ShotState shotState_;

private:
	ClientShotState();
	virtual ~ClientShotState();
};


#endif
