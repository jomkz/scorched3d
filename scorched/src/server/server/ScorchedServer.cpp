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

#include <server/ScorchedServer.h>
#include <server/ServerSimulator.h>
#include <server/ServerState.h>
#include <server/ServerDestinations.h>
#include <server/ServerAuthHandlerStore.h>
#include <server/ServerTimedMessage.h>
#include <server/ServerBanned.h>
#include <server/ServerTextFilter.h>
#include <server/ServerHandlers.h>
#include <server/ServerLoadLevel.h>
#include <server/ServerChannelManager.h>
#include <common/OptionsScorched.h>
#include <common/Logger.h>
#include <tank/TankDeadContainer.h>
#include <tank/TankContainer.h>
#include <tankai/TankAIStore.h>
#include <tankai/TankAIWeaponSets.h>
#include <landscapedef/LandscapeDefinitions.h>
#include <coms/ComsSimulateResultMessage.h>
#include <lua/LUAScriptHook.h>

#ifndef S3D_SERVER
#include <client/ClientParams.h>
#include <client/ScorchedClient.h>
#endif

ScorchedServer *ScorchedServer::instance_ = 0;
static bool instanceLock = false;

ScorchedServer *ScorchedServer::instance()
{
	if (!instance_)
	{
		DIALOG_ASSERT(!instanceLock);
		instanceLock = true;
		instance_ = new ScorchedServer;
		instanceLock = false;
	}

#ifndef S3D_SERVER
	if (ClientParams::instance()->getConnectedToServer() &&
		ScorchedClient::instance()->getTankContainer().getCurrentDestinationId() != 0)
	{
		DIALOG_ASSERT(0);
	}
#endif

	return instance_;
}

ScorchedServer::ScorchedServer() : 
	ScorchedContext("Server")
{
	serverState_ = new ServerState();
	serverSimulator_ = new ServerSimulator();
	serverSimulator_->setScorchedContext(this);
	serverDestinations_ = new ServerDestinations();
	deadContainer_ = new TankDeadContainer();
	tankAIStore_ = new TankAIStore();
	authHandler_ = new ServerAuthHandlerStore();
	timedMessage_ = new ServerTimedMessage();
	bannedPlayers_ = new ServerBanned();
	textFilter_ = new ServerTextFilter();
	serverLoadLevel_ = new ServerLoadLevel(getComsMessageHandler());
	serverChannelManager_ = new ServerChannelManager(getComsMessageHandler());

	serverHandlers_ = new ServerHandlers(getComsMessageHandler());
	getComsMessageHandler().addHandler(
		ComsSimulateResultMessage::ComsSimulateResultMessageType,
		serverSimulator_);
	getLUAScriptHook().addHookProvider("server_channeltext");
}

ScorchedServer::~ScorchedServer()
{
	delete deadContainer_;
}

Simulator &ScorchedServer::getSimulator() 
{ 
	return *serverSimulator_; 
}

ServerAuthHandler *ScorchedServer::getAuthHandler()
{
	return authHandler_->getAuthHandler();
}

ServerConnectAuthHandler &ScorchedServer::getServerConnectAuthHandler()
{
	return serverHandlers_->getServerConnectAuthHandler();
}
