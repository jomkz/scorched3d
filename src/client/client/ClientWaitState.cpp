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

#include <client/ClientWaitState.h>
#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <tank/TankContainer.h>
#include <coms/ComsMessageSender.h>
#include <coms/ComsPlayerReadyMessage.h>

ClientWaitState *ClientWaitState::instance_ = 0;

ClientWaitState *ClientWaitState::instance()
{
	if (!instance_)
	{
		instance_ = new ClientWaitState;
	}
	return instance_;
}

ClientWaitState::ClientWaitState() :
	GameStateI("ClientWaitState")
{
}

ClientWaitState::~ClientWaitState()
{
}

void ClientWaitState::enterState(const unsigned state)
{

}

void ClientWaitState::sendClientReady()
{
	ScorchedClient::instance()->getGameState().stimulate(ClientState::StimWait);

	// move into player or main state place
	// The server will move the client into the main state
	// when all clients+server are ready
	std::map<unsigned int, Tank *> &playingTanks = 
		ScorchedClient::instance()->getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = playingTanks.begin();
		itor != playingTanks.end();
		itor++)
	{
		Tank *current = (*itor).second;

		if (current->getDestinationId() == 
			ScorchedClient::instance()->getTankContainer().getCurrentDestinationId())
		{
			ComsPlayerReadyMessage readyMessage(current->getPlayerId());
			ComsMessageSender::sendToServer(readyMessage);
		}
	}
}
