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

#include <client/ClientStateStartGame.h>
#include <client/ClientParams.h>
#include <client/UniqueIdStore.h>
#include <client/ScorchedClient.h>
#include <common/DefinesAssert.h>
#include <common/DefinesString.h>
#include <common/ProgressCounter.h>
#include <coms/ComsConnectMessage.h>
#include <coms/ComsMessageSender.h>
#include <coms/ComsMessageHandler.h>
#include <net/NetInterface.h>
#include <net/NetServerTCP3.h>
#include <net/NetLoopBack.h>
#include <server/ScorchedServer.h>
#include <server/ScorchedServerSettings.h>

ClientStateStartGame::ClientStateStartGame() :
	remoteConnectionThread_(0)
{
}

ClientStateStartGame::~ClientStateStartGame()
{
}

UniqueIdStore &ClientStateStartGame::getIdStore()
{
	if (!idStore_)
	{
		idStore_ = new UniqueIdStore();
		// Get the unique id
		if (!idStore_->loadStore())
		{
			S3D::dialogExit("Scorched3D", S3D::formatStringBuffer(
					"Failed to load the id store"));
		}
	}
	return *idStore_;
}

void ClientStateStartGame::enterState()
{
	// Check if we are connecting to a server
	if (ClientParams::instance()->getConnect()[0])
	{
		return connectToServer();	
	}
	else if (ClientParams::instance()->getStartCustom() ||
		ClientParams::instance()->getClientFile()[0])
	{
		return connectToServer();
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

		return connectToServer();
	}
	else
	{
		S3D::dialogExit("Scorched3D", S3D::formatStringBuffer(
				"Unknown parameter option"));
	}
}

void ClientStateStartGame::connectToServer()
{
	ProgressCounter progressCounter;
	progressCounter.setNewPercentage(0.0f);
	ScorchedClient::instance();

	// Tidy up any existing net handlers
	if (ScorchedClient::instance()->getContext().getNetInterfaceValid())
	{
		ScorchedClient::instance()->getContext().getNetInterface().stop();
		delete &ScorchedClient::instance()->getContext().getNetInterface();
		ScorchedClient::instance()->getContext().setNetInterface(0);
	}

	// Create the new net handlers
	if (ClientParams::instance()->getConnectedToServer())
	{
		ScorchedClient::instance()->getContext().setNetInterface(new NetServerTCP3());
	}
	else
	{
		ScorchedClient::instance()->getContext().setNetInterface(new NetLoopBack(false));
	}
	ScorchedClient::instance()->getNetInterface().setMessageHandler(
		&ScorchedClient::instance()->getComsMessageHandler());


	// Start the server (if required)
	if (!ClientParams::instance()->getConnectedToServer())
	{
		ScorchedServerSettings *settings = 0;

		if (ClientParams::instance()->getSaveFile()[0])
		{
			// Load the saved game state (settings)
			settings = new ScorchedServerSettingsSave(ClientParams::instance()->getSaveFile());
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

			settings = new ScorchedServerSettingsOptions(clientFile,
				ClientParams::instance()->getRewriteOptions(),
				ClientParams::instance()->getWriteFullOptions());
		}

		progressCounter.setNewPercentage(0.0f);
		if (!ScorchedServer::instance()->startServer(
			*settings,
			true,
			&progressCounter)) 
		{
			S3D::dialogExit("Scorched3D", 
				S3D::formatStringBuffer("Failed to start server"),
				false);
			return;
		}
		delete settings;
	}



	/*
	TODO
	ProgressDialog::instance()->progressChange(
		LANG_RESOURCE_3("CONNECTING_TO", 
			"Connecting to \"{0}:{1}\" ({3})....", 
			host_, 
			S3D::formatStringBuffer("%i", port_), 
			S3D::formatStringBuffer("%i", tryCount_)), 0);
	*/

	if (ClientParams::instance()->getConnectedToServer())
	{
		// Do in a thread so connect can block if it wants!
		remoteConnectionThread_ = SDL_CreateThread(ClientStateStartGame::tryRemoteConnection, this);
	}
	else
	{
		// Or connect localy
		tryLocalConnection();
	}
}

int ClientStateStartGame::tryRemoteConnection(void *th)
{
	ClientStateStartGame *clientStateStartGame = (ClientStateStartGame *) th;

	const char *serverName = ClientParams::instance()->getConnect();
	std::string host = serverName;
	int port = S3D::ScorchedPort;

	char *colon = strchr((char *)serverName, ':');
	if (colon) 
	{
		char *stop;
		*colon = '\0';
		colon++;
		port = strtol(colon, &stop, 10);
		host = serverName;
		colon--;
		*colon = ':';
	}	

	// Try to connect to the server
	ScorchedClient::instance()->getNetInterface().connect(host.c_str(), port);
	return 0;
}

void ClientStateStartGame::tryLocalConnection()
{
	// Try to connect localy
	ScorchedClient::instance()->getNetInterface().connect("Local", 0);
}

void ClientStateStartGame::connected()
{
	// Wait for the thread to stop (if we started one)
	if (remoteConnectionThread_)
	{
		int status = 0;
		SDL_WaitThread(remoteConnectionThread_, &status);
		remoteConnectionThread_ = 0;
	}

	// Wait for the coms to start
	for (int i=0; i<10 && !ScorchedClient::instance()->getNetInterface().started(); i++)
	{
		SDL_Delay(500);
	}

	// If we connected then send our details to the server
	ComsConnectMessage connectMessage;
	connectMessage.setVersion(S3D::ScorchedVersion.c_str());
	connectMessage.setProtocolVersion(S3D::ScorchedProtocolVersion.c_str());
	ComsMessageSender::sendToServer(connectMessage);
}
