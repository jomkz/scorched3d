////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011
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
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
////////////////////////////////////////////////////////////////////////////////

#include <client/ScorchedClient.h>
#include <client/ClientSimulator.h>
#include <client/ClientState.h>
#include <client/ClientMessageHandler.h>
#include <client/ClientChannelManager.h>
#include <client/ClientHandlers.h>
#include <client/ClientOptions.h>
#include <engine/ThreadCallback.h>
#include <target/TargetSpace.h>
#include <coms/ComsSimulateMessage.h>
#include <coms/ComsNetStatMessage.h>
#include <landscapemap/LandscapeMaps.h>

TargetSpace *ScorchedClient::targetSpace_ = new TargetSpace();
ThreadCallback *ScorchedClient::threadCallback_ = new ThreadCallback();

ScorchedClient *ScorchedClient::instance_ = 0;
static ScorchedClient *instanceLock = 0;
static boost::thread::id thread_id;

ScorchedClient *ScorchedClient::instance()
{
	DIALOG_ASSERT(instance_);
	DIALOG_ASSERT(thread_id == boost::this_thread::get_id());
	return instance_;
}

void ScorchedClient::startClient()
{
	DIALOG_ASSERT(!instanceLock);
	instanceLock = new ScorchedClient;
	thread_id = boost::this_thread::get_id();
	instance_ = instanceLock;
	instanceLock = 0;
}

ScorchedClient::ScorchedClient() : 
	ScorchedContext("Client")
{
	targetSpace_->setContext(this);

	clientMessageHandler_ = new ClientMessageHandler();
	getComsMessageHandler().setConnectionHandler(clientMessageHandler_);

	channelManager_ = new ClientChannelManager(getComsMessageHandler());
	clientHandlers_ = new ClientHandlers(getComsMessageHandler());
	clientState_ = new ClientState(getComsMessageHandler());
	clientSimulator_ = new ClientSimulator();
	clientSimulator_->setScorchedContext(this);

	new ComsMessageHandlerIAdapter<ClientSimulator>(
		clientSimulator_, &ClientSimulator::processComsSimulateMessage,
		ComsSimulateMessage::ComsSimulateMessageType,
		getComsMessageHandler());
	new ComsMessageHandlerIAdapter<ClientSimulator>(
		clientSimulator_, &ClientSimulator::processNetStatMessage,
		ComsNetStatMessage::ComsNetStatMessageType,
		getComsMessageHandler());

	// Calculate how many particles we can see
	int numberOfBilboards = 6000;
	if (OptionsDisplay::instance()->getEffectsDetail() == 0) 
		numberOfBilboards = 100;
	else if (OptionsDisplay::instance()->getEffectsDetail() == 2) 
		numberOfBilboards = 10000;
}

ScorchedClient::~ScorchedClient()
{
	targetSpace_->clear();
}

Simulator &ScorchedClient::getSimulator() 
{ 
	return *clientSimulator_; 
}
