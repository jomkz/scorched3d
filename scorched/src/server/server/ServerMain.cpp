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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <weapons/AccessoryStore.h>
#include <weapons/EconomyStore.h>
#include <net/NetLoopBack.h>
#include <net/NetServerTCP3.h>
#include <lua/LUAScriptHook.h>
#include <common/Defines.h>
#include <common/Clock.h>
#include <common/ARGParser.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <engine/Simulator.h>
#include <engine/ModFiles.h>
#include <landscapedef/LandscapeDefinitions.h>
#include <landscapemap/LandscapeMaps.h>
#include <tankai/TankAIAdder.h>
#include <tankai/TankAIStore.h>
#include <tank/TankModelStore.h>
#include <server/ServerLinesHandler.h>
#include <server/ServerMessageHandler.h>
#include <server/ServerGiftMoneyHandler.h>
#include <server/ServerDefenseHandler.h>
#include <server/ServerPlayedMoveHandler.h>
#include <server/ServerAddPlayerHandler.h>
#include <server/ServerAdminHandler.h>
#include <server/ServerHaveModFilesHandler.h>
#include <server/ServerBuyAccessoryHandler.h>
#include <server/ServerKeepAliveHandler.h>
#include <server/ServerFileAkHandler.h>
#include <server/ServerInitializeModHandler.h>
#include <server/ServerChannelManager.h>
#include <server/ServerConnectHandler.h>
#include <server/ServerConnectAuthHandler.h>
#include <server/ServerOperationResultHandler.h>
#include <server/ServerFileServer.h>
#include <server/ServerLoadLevel.h>
#include <server/ServerRegistration.h>
#include <server/ServerLog.h>
#include <server/ServerBrowserInfo.h>
#include <server/ServerState.h>
#include <server/ServerCommon.h>
#include <server/ServerBanned.h>
#include <server/ServerMain.h>
#include <server/ScorchedServer.h>
#include <server/ScorchedServerUtil.h>
#include <SDL/SDL.h>

#ifdef S3D_SERVER
#include <webserver/ServerWebServer.h>
#endif

Clock serverTimer;

void checkSettings()
{
	ScorchedServer::instance()->getLandscapes().checkEnabled(
		ScorchedServer::instance()->getOptionsGame());
	
	if (ScorchedServer::instance()->getOptionsGame().getTeamBallance() == 
		OptionsGame::TeamBallanceBotsVs &&
		ScorchedServer::instance()->getOptionsGame().getTeams() > 2)
	{
		S3D::dialogExit("ScorchedServer",
			"Cannot start a game with more than 2 teams in the bots vs mode");		
	}
}

bool startServer(bool local, ProgressCounter *counter)
{
	Logger::log(S3D::formatStringBuffer("Scorched3D - Version %s (%s) - %s",
		S3D::ScorchedVersion.c_str(), 
		S3D::ScorchedProtocolVersion.c_str(), 
		S3D::ScorchedBuildTime.c_str()));

	// Setup the message handling classes
	if (!local)
	{
		// Only create a net server for the actual multiplayer case
		// A loopback is created by the client for a single player game 
		ScorchedServer::instance()->getContext().setNetInterface(
			//new NetServerTCP(new NetServerTCPScorchedProtocol());
			//new NetServerUDP();
			//new NetServerTCP2();
			new NetServerTCP3());
	}

	ScorchedServer::instance()->getOptionsGame().updateChangeSet();
	ScorchedServer::instance()->getNetInterface().setMessageHandler(
		&ScorchedServer::instance()->getComsMessageHandler());
	ScorchedServer::instance()->getComsMessageHandler().setConnectionHandler(
		ServerMessageHandler::instance());
	ServerConnectHandler::instance();
	ServerConnectAuthHandler::instance();
	ServerLinesHandler::instance();
	ServerChannelManager::instance();
	ServerGiftMoneyHandler::instance();
	ServerAdminHandler::instance();
	ServerHaveModFilesHandler::instance();
	ServerInitializeModHandler::instance();
	ServerKeepAliveHandler::instance();
	ServerPlayedMoveHandler::instance();
	ServerFileAkHandler::instance();
	ServerBuyAccessoryHandler::instance();
	ServerAddPlayerHandler::instance();
	ServerLoadLevel::instance();
	ServerDefenseHandler::instance();
	ServerOperationResultHandler::instance();

	// Set the mod
	S3D::setDataFileMod(
		ScorchedServer::instance()->getOptionsGame().getMod());

	// Load mod
#ifdef S3D_SERVER
	{
		if (!ScorchedServer::instance()->getModFiles().loadModFiles(
			ScorchedServer::instance()->getOptionsGame().getMod(), false,
			counter)) return false;
	}
#endif

	if (!ScorchedServer::instance()->getAccessoryStore().parseFile(
		ScorchedServer::instance()->getContext(),
		counter)) return false;
	if (!ScorchedServer::instance()->getTankModels().loadTankMeshes(
		ScorchedServer::instance()->getContext(), 2, counter))
		return false;
	ScorchedServer::instance()->getOptionsTransient().reset();
	if (!ScorchedServer::instance()->getLandscapes().readLandscapeDefinitions()) return false;

	// Add the server side bots
	// Add any new AIs
	if (!ScorchedServer::instance()->getTankAIs().loadAIs()) return false;
	TankAIAdder::addTankAIs(*ScorchedServer::instance());

	// Start the state machine
	// ServerState::setupStates(ScorchedServer::instance()->getGameState());
	EconomyStore::instance();

	checkSettings();

	// Load all script hooks
	if (!ScorchedServer::instance()->getLUAScriptHook().loadHooks()) return false;

	return true;
}

void serverMain(ProgressCounter *counter)
{
	// Create the server states
	if (!startServer(false, counter)) exit(64);

	// Try to start the server
	if (!ScorchedServer::instance()->getContext().getNetInterface().start(
		ScorchedServer::instance()->getOptionsGame().getPortNo()) ||
		!ServerBrowserInfo::instance()->start())
	{
		S3D::dialogExit("Scorched3D Server", 
			S3D::formatStringBuffer("Failed to start the server.\n\n"
			"Could not bind to the server ports.\n"
			"Ensure the specified ports (%i, %i) do not conflict with any other program.",
			ScorchedServer::instance()->getOptionsGame().getPortNo(),
			ScorchedServer::instance()->getOptionsGame().getPortNo() + 1));
	}

 	if (ScorchedServer::instance()->getOptionsGame().getPublishServer()) 
	{
		ServerRegistration::instance()->start();
	}

#ifdef S3D_SERVER
	if (ScorchedServer::instance()->getOptionsGame().getManagementPortNo() > 0)
	{
		ServerWebServer::instance()->start(
			ScorchedServer::instance()->getOptionsGame().getManagementPortNo());

		Logger::log(S3D::formatStringBuffer("Management server running on url http://127.0.0.1:%i",
			ScorchedServer::instance()->getOptionsGame().getManagementPortNo()));
	}
#endif
	ServerLog::instance();

	std::string startTime = S3D::getStartTime();
	Logger::log(S3D::formatStringBuffer("Server started : %s", startTime.c_str()));
}

void serverLoop()
{
	// Main server loop:
	if (ScorchedServer::instance()->getContext().getNetInterfaceValid())
	{
		Logger::processLogEntries();
		ScorchedServer::instance()->getNetInterface().processMessages();
#ifdef S3D_SERVER
		{
			ServerBrowserInfo::instance()->processMessages();
			ServerWebServer::instance()->processMessages();
		}
#endif

		float timeDifference = serverTimer.getTimeDifference();
		/*
		ScorchedServer::instance()->getGameState().draw();
		ScorchedServer::instance()->getGameState().simulate(timeDifference);
		ServerKeepAliveHandler::instance()->checkKeepAlives();
		*/

		ScorchedServer::instance()->getSimulator().simulate();

		ServerFileServer::instance()->simulate(timeDifference);
		ServerChannelManager::instance()->simulate(timeDifference);
		ScorchedServerUtil::instance()->timedMessage.simulate();


		if (timeDifference > 5.0f)
		{
			Logger::log(S3D::formatStringBuffer("Warning: Server loop took %.2f seconds", 
				timeDifference));
		}
	}
}

class ConsoleServerProgressCounter : public ProgressCounterI, public LoggerI
{
public:
	ConsoleServerProgressCounter() : hashes_(25) {}

	virtual void drawHashes(int neededHashes)
	{
		if (hashes_ < neededHashes)
		{
			for (int h=hashes_;h<neededHashes; h++)
			{
				printf("#");
				if (h == 24)
				{
					printf("\n");
				}
			}
			hashes_ = neededHashes;
		}
		fflush(stdout);
	}

	virtual void logMessage(LoggerInfo &info)
	{
		drawHashes(25);
	}

	virtual void operationChange(const LangString &op)
	{
		Logger::processLogEntries();
		hashes_ = 0;
	}

	virtual void progressChange(const LangString &op, const float percentage)
	{
		int neededHashes = int(percentage / 4.0f);
		drawHashes(neededHashes);
	}
protected:
	bool firstOp_;
	int hashes_;
};

class ConsoleLogger : public LoggerI
{
public:
	virtual void logMessage(LoggerInfo &info)
	{
		printf("%s - %s\n", info.getTime(), info.getMessage());
	}
};

void consoleServer()
{
	ConsoleLogger consoleLogger;
	ProgressCounter progressCounter;
	ConsoleServerProgressCounter progressCounterI;
	progressCounter.setUser(&progressCounterI);

	ServerCommon::startFileLogger();
	Logger::instance()->addLogger(&progressCounterI);
	Logger::instance()->addLogger(&consoleLogger);
	serverMain(&progressCounter);

	// Get a landscape definition to use
	ServerCommon::serverLog("Generating landscape");
	LandscapeDefinition defn = ScorchedServer::instance()->getLandscapes().getRandomLandscapeDefn(
		ScorchedServer::instance()->getContext().getOptionsGame(),
		ScorchedServer::instance()->getContext().getTankContainer());

	// Set all options (wind etc..)
	ScorchedServer::instance()->getContext().getOptionsTransient().newGame();

	// Generate the new level
	ScorchedServer::instance()->getLandscapeMaps().generateMaps(
		ScorchedServer::instance()->getContext(), defn, 
		&progressCounter);
	ScorchedServer::instance()->getSimulator().reset();
	ServerCommon::serverLog("Finished generating landscape");

	for (;;)
	{
		SDL_Delay(10);
		serverLoop();
	}
}

