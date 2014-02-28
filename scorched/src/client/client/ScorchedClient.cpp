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
#include <client/ClientParams.h>
#include <client/ClientSimulator.h>
#include <client/ClientState.h>
#include <client/ClientMessageHandler.h>
#include <client/ClientChannelManager.h>
#include <client/ClientHandlers.h>
#include <client/ClientOptions.h>
#include <net/NetInterface.h>
#include <net/NetServerTCP3.h>
#include <net/NetLoopBack.h>
#include <engine/ThreadCallback.h>
#include <target/TargetSpace.h>
#include <target/TargetContainer.h>
#include <coms/ComsSimulateMessage.h>
#include <coms/ComsNetStatMessage.h>
#include <common/ThreadUtils.h>
#include <common/Logger.h>
#include <landscapemap/LandscapeMaps.h>
#include <server/ScorchedServer.h>
#include <server/ScorchedServerSettings.h>

TargetSpace *ScorchedClient::targetSpace_ = new TargetSpace();
ThreadCallback *ScorchedClient::threadCallback_ = new ThreadCallback();

static time_t startTime = 0;
ScorchedClient *ScorchedClient::instance_ = 0;
static ScorchedClient *instanceLock = 0;
static boost::thread::id thread_id;
static boost::thread *clientThread;
static boost::mutex creationMutex;
static bool clientStopped = false;

ScorchedClient *ScorchedClient::instance()
{
	DIALOG_ASSERT(instance_);
	DIALOG_ASSERT(thread_id == boost::this_thread::get_id());
	return instance_;
}

void ScorchedClient::startClient(ProgressCounter *counter, ThreadCallbackI *endCallback)
{
	DIALOG_ASSERT(!instance_ && !instanceLock);
	creationMutex.lock();
	clientStopped = false;
	instanceLock = new ScorchedClient;
	instance_ = instanceLock;
	instanceLock = 0;
	clientThread = new boost::thread(ScorchedClient::startClientInternalStatic, instance_, counter, endCallback);
	ThreadUtils::setThreadName(clientThread->native_handle(), "ClientThread");
	creationMutex.unlock();
}

void ScorchedClient::stopClient()
{
	creationMutex.lock();
	clientStopped = true;
	if (clientThread)
	{
		clientThread->join();
		delete clientThread;
		clientThread = 0;
	}
	if (instance_)
	{
		delete instance_;
		instance_ = 0;
	}
	creationMutex.unlock();
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

void ScorchedClient::startClientInternalStatic(ScorchedClient *instance, 
		ProgressCounter *counter, ThreadCallbackI *endCallback)
{
	thread_id = boost::this_thread::get_id();
	instance->startClientInternal(counter, endCallback);
}

void ScorchedClient::startClientInternal(
	ProgressCounter *counter, ThreadCallbackI *endCallback)
{
	// Check if we are connecting to a server
	if (ClientParams::instance()->getConnect()[0])
	{

	}
	else if (ClientParams::instance()->getStartCustom() ||
		ClientParams::instance()->getClientFile()[0])
	{

	}
	else if (ClientParams::instance()->getSaveFile()[0])
	{
		// Or the client saved game
		if (!S3D::fileExists(ClientParams::instance()->getSaveFile()))
		{
			S3D::dialogExit("Scorched3D", S3D::formatStringBuffer(
				"Client read saved game file \"%s\" does not exist.",
				ClientParams::instance()->getSaveFile()),
				false);
		}
	}
	else
	{
		S3D::dialogExit("Scorched3D", S3D::formatStringBuffer(
				"Unknown parameter option"));
	}

	// Tidy up any existing net handlers
	if (getContext().getNetInterfaceValid())
	{
		getContext().getNetInterface().stop();
		delete &getContext().getNetInterface();
		getContext().setNetInterface(0);
	}

	// Create the new net handlers
	if (ClientParams::instance()->getConnectedToServer())
	{
		getContext().setNetInterface(new NetServerTCP3());
	}
	else
	{
		getContext().setNetInterface(new NetLoopBack(false));
	}
	ScorchedClient::instance()->getNetInterface().setMessageHandler(
		&ScorchedClient::instance()->getComsMessageHandler());

	// Start the server (if required)
	if (!ClientParams::instance()->getConnectedToServer())
	{
		ThreadCallbackI *callback = new ThreadCallbackIAdapter<ScorchedClient>(
			this, &ScorchedClient::serverStartedServerThread);

		if (ClientParams::instance()->getSaveFile()[0])
		{
			// Load the saved game state (settings)
			ScorchedServerSettingsSave settings(ClientParams::instance()->getSaveFile());
			ScorchedServer::startServer(settings, 0, callback);
		}
		else
		{
			std::string clientFile = ClientParams::instance()->getClientFile();
			if (ClientParams::instance()->getStartCustom())
			{
				clientFile = S3D::getSettingsFile("singlecustom.xml");
			}

			// If not load the client settings file
			if (!S3D::fileExists(clientFile.c_str()))
			{
				S3D::dialogExit("Scorched3D", S3D::formatStringBuffer(
					"Client file \"%s\" does not exist.",
					clientFile.c_str()));
			}

			ScorchedServerSettingsOptions settings(clientFile,
				ClientParams::instance()->getRewriteOptions(),
				ClientParams::instance()->getWriteFullOptions());
			ScorchedServer::startServer(settings, 0, callback);
		}
	}
	else
	{
		serverStartedClientThread();
	}

	// Start the loop
	while (!clientStopped)
	{
		if (!clientLoop())
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(10));
		}
	}
}

void ScorchedClient::serverStartedServerThread()
{
	// This method is called by the server thread so switch flow to the client thread
	ThreadCallbackI *callback = new ThreadCallbackIAdapter<ScorchedClient>(
		instance_, &ScorchedClient::serverStartedClientThread);
	getThreadCallback().addCallback(callback);
}

void ScorchedClient::serverStartedClientThread()
{
	// Set the correct state
	getClientState().setState(ClientState::StateInitialize);
}

bool ScorchedClient::clientLoop()
{
	ScorchedClient::instance()->getClientSimulator().simulate();

	Logger::processLogEntries();
	bool processed = getNetInterface().processMessages() > 0;

	if (ClientParams::instance()->getExitTime() > 0)
	{
		if (startTime == 0) startTime = time(0);
		if (time(0) - startTime > ClientParams::instance()->getExitTime())
		{
			exit(0);
		}
	}
	if (ClientParams::instance()->getDisconnectTime() > 0)
	{
		if (startTime == 0) startTime = time(0);
		if (time(0) - startTime > ClientParams::instance()->getDisconnectTime())
		{
			startTime = time(0);
			if (getTargetContainer().getCurrentDestinationId())
			{
				getNetInterface().disconnectAllClients();
			}
			else
			{
				getClientState().setState(ClientState::StateInitialize);
			}
		}
	}
	getThreadCallback().processCallbacks();

	return processed;
}
