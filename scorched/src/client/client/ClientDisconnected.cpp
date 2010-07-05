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

#include <client/ClientDisconnected.h>
#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <client/ScorchedClient.h>
#include <client/ClientSimulator.h>
#include <landscapemap/LandscapeMaps.h>
#include <server/ScorchedServer.h>
#include <engine/GameState.h>
#include <dialogs/AnimatedBackdropDialog.h>

ClientDisconnected::ClientDisconnected() : GameStateI("ClientDisconnected")
{
}

ClientDisconnected::~ClientDisconnected()
{
}

void ClientDisconnected::enterState(const unsigned state)
{
	std::string name = ScorchedClient::instance()->getLandscapeMaps().
		getDefinitions().getDefinition().getName();
	if (name == "blank")
	{
		AnimatedBackdropDialog::instance()->reInit();
	}
	
	ScorchedClient::instance()->getClientSimulator().clear();
	ScorchedServer::instance()->stopServer();

	ScorchedClient::instance()->getGameState().stimulate(
		ClientState::StimOptions);
}
