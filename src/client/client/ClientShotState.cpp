////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
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

#include <client/ClientShotState.h>
#include <client/ClientState.h>
#include <client/ScorchedClient.h>
#include <client/ClientWaitState.h>
#include <client/ClientParams.h>
#include <engine/ActionController.h>
#include <engine/ViewPoints.h>
#include <common/Logger.h>
#include <coms/ComsPlayMovesMessage.h>
#include <landscape/Landscape.h>

ClientShotState *ClientShotState::instance_ = 0;

ClientShotState *ClientShotState::instance()
{
	if (!instance_)
	{
		instance_ = new ClientShotState;
	}
	return instance_;
}

ClientShotState::ClientShotState() :
	GameStateI("ClientShotState"),
	playShots_(),
	shotState_(ScorchedClient::instance()->getContext(), playShots_)
{
	ScorchedClient::instance()->getComsMessageHandler().addHandler(
		"ComsPlayMovesMessage",
		this);
}

ClientShotState::~ClientShotState()
{
}

bool ClientShotState::processMessage(
	NetMessage &message,
	const char *messageType,
	NetBufferReader &reader)
{
	// Reset graphics
	Landscape::instance()->restoreLandscapeTexture();
	ScorchedClient::instance()->getContext().viewPoints->reset();

	// Read the new shots into the action controller
	ComsPlayMovesMessage playMovesMessage;
	if (!playMovesMessage.readMessage(reader)) return false;

	// Read the moves from the message
	playShots_.readMessage(playMovesMessage);

	// Play the shots
	ScorchedClient::instance()->getActionController().
		getRandom().seed(playMovesMessage.getSeed());
	shotState_.setup();

	// Ensure and move to the shot state
	ScorchedClient::instance()->getGameState().stimulate(ClientState::StimWait);
	ScorchedClient::instance()->getGameState().checkStimulate();
	ScorchedClient::instance()->getGameState().stimulate(ClientState::StimShot);
	return true;
}

void ClientShotState::enterState(const unsigned state)
{
	// Not needed as setup is done above
}

bool ClientShotState::acceptStateChange(const unsigned state, 
		const unsigned nextState,
		float frameTime)
{
	// All the shots have finished, move to finished
	if (shotState_.run(frameTime))
	{
		// Tell the server we have finished processing the landscape
		// This will stimulate us into the wait state
		ClientWaitState::instance()->sendClientReady();
		return false; // This is done by the send ready
	}

	return false;
}
