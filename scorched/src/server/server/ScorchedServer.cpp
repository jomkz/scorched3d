////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2013
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
#include <server/ServerAdminSessions.h>
#include <server/ServerSyncCheck.h>
#include <server/ServerMessageHandler.h>
#include <server/ServerFileServer.h>
#include <server/ScorchedServerSettings.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <common/Logger.h>
#include <common/ThreadUtils.h>
#include <common/Clock.h>
#include <engine/ModFiles.h>
#include <engine/ActionController.h>
#include <engine/SaveGame.h>
#include <engine/ThreadCallback.h>
#include <events/EventHandlerDataBase.h>
#include <events/EventHandlerAchievementNumberRankKills.h>
#include <events/EventController.h>
#include <tank/TankDeadContainer.h>
#include <target/TargetContainer.h>
#include <tank/TankModelStore.h>
#include <tanket/TanketTypes.h>
#include <tankai/TankAIStrings.h>
#include <tankai/TankAIStore.h>
#include <tankai/TankAIWeaponSets.h>
#include <tankai/TankAIAdder.h>
#include <target/TargetSpace.h>
#include <landscapedef/LandscapeDefinitions.h>
#include <coms/ComsSimulateResultMessage.h>
#include <weapons/EconomyStore.h>
#include <weapons/AccessoryStore.h>
#include <net/NetServerTCP3.h>
#include <net/NetLoopBack.h>

ScorchedServer *ScorchedServer::instance_ = 0;
static Clock serverTimer;
static ScorchedServer *instanceLock = 0;
static boost::thread *serverThread;
static boost::thread::id thread_id;
static boost::mutex creationMutex;
static bool serverStopped = false;
TargetSpace *ScorchedServer::targetSpace_ = new TargetSpace();
ThreadCallback *ScorchedServer::threadCallback_ = new ThreadCallback(true);

ScorchedServer *ScorchedServer::instance()
{
	DIALOG_ASSERT(instance_);
	DIALOG_ASSERT(thread_id == boost::this_thread::get_id());
	return instance_;
}

void ScorchedServer::startServer(ScorchedServerSettings *settings, ProgressCounter *counter, ThreadCallbackI *endCallback)
{
	DIALOG_ASSERT(!instance_ && !instanceLock);
	creationMutex.lock();
	serverStopped = false;
	instanceLock = new ScorchedServer;
	instance_ = instanceLock;
	instanceLock = 0;
	serverThread = new boost::thread(ScorchedServer::startServerInternalStatic, instance_, settings, counter, endCallback);
	ThreadUtils::setThreadName(serverThread->native_handle(), "ServerThread");
	creationMutex.unlock();
}

void ScorchedServer::stopServer()
{
	creationMutex.lock();
	serverStopped = true;
	if (serverThread)
	{
		serverThread->join();
		delete serverThread;
		serverThread = 0;
	}
	if (instance_)
	{
		delete instance_;
		instance_ = 0;
	}
	creationMutex.unlock();
}

ScorchedServer::ScorchedServer() : 
	ScorchedContext("Server")
{
	targetSpace_->setContext(this);

	economyStore_ = new EconomyStore();
	serverState_ = new ServerState();
	serverFileServer_ = new ServerFileServer();
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
	serverAdminSessions_ = new ServerAdminSessions();
	serverSyncCheck_ = new ServerSyncCheck(getComsMessageHandler());
	serverHandlers_ = new ServerHandlers(getComsMessageHandler());
	getComsMessageHandler().addHandler(
		ComsSimulateResultMessage::ComsSimulateResultMessageType,
		serverSimulator_);

	serverMessageHandler_ = new ServerMessageHandler();
	getComsMessageHandler().setConnectionHandler(serverMessageHandler_);
}

ScorchedServer::~ScorchedServer()
{
	targetSpace_->clear();
	delete deadContainer_;
	delete tankAIStore_;
	delete serverSimulator_;
	delete serverDestinations_;
	delete serverState_;
	delete authHandler_;
	delete timedMessage_;
	delete bannedPlayers_;
	delete textFilter_;
	delete serverHandlers_;
	delete serverLoadLevel_;
	delete serverChannelManager_;	
	delete serverAdminSessions_;
	delete serverSyncCheck_;
	delete serverMessageHandler_;
	delete serverFileServer_;
	delete economyStore_;
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

void ScorchedServer::startServerInternalStatic(ScorchedServer *instance, 
		ScorchedServerSettings *settings, 
		ProgressCounter *counter, ThreadCallbackI *endCallback)
{
	thread_id = boost::this_thread::get_id();
	instance->startServerInternal(settings, counter, endCallback);
}

void ScorchedServer::startServerInternal(ScorchedServerSettings *settings, 
	ProgressCounter *counter, ThreadCallbackI *endCallback)
{
	std::string startTime = S3D::getStartTime();
	Logger::log(S3D::formatStringBuffer("Server started : %s", startTime.c_str()));

	std::string settingsType = settings->type();
	if (settingsType == "FILE")
	{
		ScorchedServerSettingsOptions *options = (ScorchedServerSettingsOptions *) settings;

		// Load options
		if (!getOptionsGame().getMainOptions().readFromFile(options->settingsFile_))
		{
			S3D::dialogExit("Scorched3D", 
				S3D::formatStringBuffer("ScorchedServer::startServerInternal - Failed to read server settings file : %s",
				options->settingsFile_.c_str()));
		}
		if (options->rewriteOptions_)
		{
			getOptionsGame().getMainOptions().writeToFile(options->settingsFile_);
		}
	}
	else if (settingsType == "SAVE")
	{
		ScorchedServerSettingsSave *options = (ScorchedServerSettingsSave *) settings;

		// Load the saved game state (settings)
		if (!SaveGame::loadState(options->saveFile_))
		{
			S3D::dialogExit("Scorched3D", S3D::formatStringBuffer(
				"Cannot load save file \"%s\".",
				options->saveFile_.c_str()), 
				false);			
		}
	}
	else
	{
		S3D::dialogExit("Scorched3D", "ScorchedServer::startServerInternal - Unknown settings type");
	}
	delete settings;
	settings = 0;

	// Setup the message handling classes
#ifndef S3D_SERVER
	setNetInterface(new NetLoopBack(true));
#else
	// Only create a net server for the actual multiplayer case
	// A loopback is created by the client for a single player game 
	setNetInterface(new NetServerTCP3());
#endif

	getOptionsGame().updateChangeSet();
	getNetInterface().setMessageHandler(&getComsMessageHandler());

	// Set the mod
	S3D::setDataFileMod(getOptionsGame().getMod());

	// Load mod
#ifdef S3D_SERVER
	if (!getModFiles().loadModFiles(getOptionsGame().getMod(), false, counter)) return false;
#endif	
	
	// Parse config
	if (!getAccessoryStore().parseFile(getContext(), counter)) 
	{
		S3D::dialogExit("Scorched3D", "Cannot load accessory store");
	}
	if (!getTanketTypes().loadTanketTypes(getContext()))
	{
		S3D::dialogExit("Scorched3D", "Cannot load tanket types");
	}
	if (!getTankModels().loadTankMeshes(getContext(), counter)) 
	{
		S3D::dialogExit("Scorched3D", "Cannot load tank meshes");
	}
	getOptionsTransient().reset();
	if (!getLandscapes().readLandscapeDefinitions()) 
	{
		S3D::dialogExit("Scorched3D", "Cannot read landscape definitions");
	}

	// Add the server side bots
	// Add any new AIs
	if (!getTankAIs().loadAIs()) 
	{
		S3D::dialogExit("Scorched3D", "Cannot load AIs");
	}
	TankAIAdder::addTankAIs(*this);
	getTankAIStrings().load();

	checkSettings();

	// Add event hooks
	eventHandlerDataBase_ = EventHandlerDataBase::createInstance();
	if (eventHandlerDataBase_)
	{
		getEventController().addEventHandler(eventHandlerDataBase_);
	}
	getEventController().addEventHandler(new EventHandlerAchievementNumberRankKills(eventHandlerDataBase_));

	// Callback the end of initialization
	if (endCallback)
	{
		endCallback->callbackInvoked();
		delete endCallback;
	}

#ifdef S3D_SERVER
	// Try to start the server
	if (!getNetInterface().start(getOptionsGame().getPortNo()) ||
		!ServerBrowserInfo::instance()->start())
	{
		S3D::dialogExit("Scorched3D Server", 
			S3D::formatStringBuffer("Failed to start the server.\n\n"
			"Could not bind to the server ports.\n"
			"Ensure the specified ports (%i, %i) do not conflict with any other program.",
			getOptionsGame().getPortNo(),
			getOptionsGame().getPortNo() + 1));
	}

	// Contact the registration server
 	if (getOptionsGame().getPublishServer()) 
	{
		ServerRegistration::instance()->start();
	}

	if (getOptionsGame().getManagementPortNo() > 0)
	{
		ServerWebServer::instance()->start(getOptionsGame().getManagementPortNo());

		Logger::log(S3D::formatStringBuffer("Management server running on url http://127.0.0.1:%i",
			getOptionsGame().getManagementPortNo()));
	}
	ServerLog::instance();
#endif

	// The server loop
	serverTimer.reset();
	while (!serverStopped)
	{
		unsigned int ticksDifference = serverTimer.getTicksDifference();
		fixed timeDifference(true, ticksDifference * 10);

		if (!serverLoop(timeDifference))
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(10));
		}
	}
}

bool ScorchedServer::serverLoop(fixed timeDifference)
{
	if (!getNetInterfaceValid())
	{
		return false;
	}
		
	bool processed = getNetInterface().processMessages() > 0;
#ifdef S3D_SERVER
	{
		ServerBrowserInfo::instance()->processMessages();
		ServerWebServer::instance()->processMessages();
	}
#endif

	getSimulator().simulate();
	getServerState().simulate(timeDifference);

	getServerConnectAuthHandler().processMessages();
	getServerFileServer().simulate();
	getServerChannelManager().simulate(timeDifference);
	getTimedMessage().simulate();

	if (timeDifference > fixed(5))
	{
		Logger::log(S3D::formatStringBuffer("Warning: Server loop took %.2f seconds", 
			timeDifference.asFloat()));
	}
	getServerThreadCallback().processCallbacks();

	return processed;
}

void ScorchedServer::checkSettings()
{
	getLandscapes().checkEnabled(getOptionsGame());
	
	if (getOptionsGame().getTeamBallance() == OptionsGame::TeamBallanceBotsVs &&
		getOptionsGame().getTeams() > 2)
	{
		S3D::dialogExit("ScorchedServer",
			"Cannot start a game with more than 2 teams in the bots vs mode");		
	}
}
