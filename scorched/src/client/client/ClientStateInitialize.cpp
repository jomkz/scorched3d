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

#include <client/SecureID.h>
#include <client/ClientStateInitialize.h>
#include <client/ClientParams.h>
#include <client/UniqueIdStore.h>
#include <client/ScorchedClient.h>
#include <client/ClientHandlers.h>
#include <client/ClientState.h>
#include <client/ClientStateLoadLevel.h>
#include <client/ClientOptions.h>
#include <common/DefinesAssert.h>
#include <common/DefinesString.h>
#include <common/ProgressCounter.h>
#include <common/OptionsGame.h>
#include <common/OptionsScorched.h>
#include <common/Logger.h>
#include <engine/ModFiles.h>
#include <engine/ModFileEntryLoader.h>
#include <coms/ComsConnectMessage.h>
#include <coms/ComsMessageSender.h>
#include <coms/ComsMessageHandler.h>
#include <coms/ComsConnectAuthMessage.h>
#include <coms/ComsConnectAcceptMessage.h>
#include <coms/ComsHaveModFilesMessage.h>
#include <coms/ComsInitializeModMessage.h>
#include <coms/ComsFileAkMessage.h>
#include <coms/ComsFileMessage.h>
#include <net/NetInterface.h>
#include <net/NetServerTCP3.h>
#include <net/NetLoopBack.h>
#include <server/ScorchedServer.h>
#include <server/ScorchedServerSettings.h>
#include <target/TargetContainer.h>
#include <tank/TankModelStore.h>
#include <tankai/TankAIStrings.h>
#include <tanket/TanketTypes.h>
#include <weapons/AccessoryStore.h>
#include <landscapedef/LandscapeDefinitions.h>

ClientStateInitialize::ClientStateInitialize(ComsMessageHandler &comsMessageHandler) :
	remoteConnectionThread_(0), totalBytes_(0)
{
	new ComsMessageHandlerIAdapter<ClientStateInitialize>(
		this, &ClientStateInitialize::processConnectAuthMessage,
		ComsConnectAuthMessage::ComsConnectAuthMessageType,
		comsMessageHandler);
	new ComsMessageHandlerIAdapter<ClientStateInitialize>(
		this, &ClientStateInitialize::processConnectAcceptMessage,
		ComsConnectAcceptMessage::ComsConnectAcceptMessageType,
		comsMessageHandler);	
	new ComsMessageHandlerIAdapter<ClientStateInitialize>(
		this, &ClientStateInitialize::processFileMessage,
		ComsFileMessage::ComsFileMessageType,
		comsMessageHandler);	
	new ComsMessageHandlerIAdapter<ClientStateInitialize>(
		this, &ClientStateInitialize::processInitializeModMessage,
		ComsInitializeModMessage::ComsInitializeModMessageType,
		comsMessageHandler);		
}

ClientStateInitialize::~ClientStateInitialize()
{
}

UniqueIdStore &ClientStateInitialize::getIdStore()
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

void ClientStateInitialize::getHost(std::string &host, int &port)
{
	if (ClientParams::instance()->getConnectedToServer())
	{
		const char *serverName = ClientParams::instance()->getConnect();
		host = serverName;
		port = S3D::ScorchedPort;

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
	}
	else
	{
		host = "Loopback";
		port = 0;
	}
}

void ClientStateInitialize::enterState()
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

void ClientStateInitialize::connectToServer()
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

	// Do in a thread so connect can block if it wants!
	remoteConnectionThread_ = SDL_CreateThread(ClientStateInitialize::tryRemoteConnection, this);
}

int ClientStateInitialize::tryRemoteConnection(void *th)
{
	ClientStateInitialize *clientStateInitialize = (ClientStateInitialize *) th;

	// Try to connect to the server
	std::string host;
	int port;
	clientStateInitialize->getHost(host, port);
	ScorchedClient::instance()->getNetInterface().connect(host.c_str(), port);
	return 0;
}

void ClientStateInitialize::connected()
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

bool ClientStateInitialize::processConnectAuthMessage(
	NetMessage &netMessage,
	NetBufferReader &reader)
{
	ComsConnectAuthMessage message;
	if (!message.readMessage(reader)) return false;

	bool nameRequired = (0 == strcmp(message.getUserName(), "required"));
	bool passwordRequired = (0 == strcmp(message.getPassword(), "required"));
	if ((passwordRequired && !ClientParams::instance()->getPassword()[0]) ||
		(nameRequired && !ClientParams::instance()->getUserName()[0]))
	{
		/*
		TODO
		AuthDialog::instance()->setRequiredAuth(
			(nameRequired?AuthDialog::eNameRequired:0) |
			(passwordRequired?AuthDialog::ePasswordRequired:0));
		GLWWindowManager::instance()->showWindow(
			AuthDialog::instance()->getId());
		*/
	}
	else
	{
		sendAuth();
	}

	return true;
}

void ClientStateInitialize::sendAuth()
{
	std::string hostName = "";
	int portNumber = 0;
	getHost(hostName, portNumber);

	// Update unique id store
	std::string uniqueId, SUI;
	if (ClientParams::instance()->getConnectedToServer())
	{
		IPaddress address;
		if (SDLNet_ResolveHost(&address, (char *) hostName.c_str(), 0) == 0)
		{
			unsigned int ipAddress = SDLNet_Read32(&address.host);
			// TODO
			uniqueId = "";//ConnectDialog::instance()->getIdStore().getUniqueId(ipAddress);

			SecureID MakeKey;
			SUI = MakeKey.getSecureID(ipAddress);
		}
	}

	// Check the number of players that are connecting
	unsigned int noPlayers = 1;
	if (!ClientParams::instance()->getConnectedToServer() &&
		!ClientParams::instance()->getSaveFile()[0])
	{
		int maxComputerAIs = ScorchedServer::instance()->getOptionsGame().getNoMaxPlayers();
		noPlayers = 0;
		for (int i=0; i<maxComputerAIs; i++)
		{
			const char *playerType = ScorchedServer::instance()->getOptionsGame().getPlayerType(i);
			if (0 == stricmp(playerType, "Human"))
			{
				noPlayers++;
			}
		}
	}

	ComsConnectAuthMessage connectMessage;
	connectMessage.setUserName(ClientParams::instance()->getUserName());
	connectMessage.setPassword(ClientParams::instance()->getPassword());
	connectMessage.setUniqueId(uniqueId.c_str());
	connectMessage.setSUI(SUI.c_str());
	connectMessage.setHostDesc(OptionsDisplay::instance()->getHostDescription());
	connectMessage.setNoPlayers(noPlayers);
	connectMessage.setCompatabilityVer((unsigned int) OptionsDisplay::instance()->getOptions().size());

	ComsMessageSender::sendToServer(connectMessage);
}

bool ClientStateInitialize::processConnectAcceptMessage(
	NetMessage &netMessage,
	NetBufferReader &reader)
{
	ComsConnectAcceptMessage message;
	if (!message.readMessage(reader)) return false;

	if (ClientParams::instance()->getConnectedToServer())
	{
		unsigned int ip = netMessage.getIpAddress();
		if (!getIdStore().saveUniqueId(
			ip, message.getUniqueId(), message.getPublishAddress()))
		{
			Logger::log( "Server failed ip security check!");
			return false;
		}
	}

	// The server tells us what our id is.
	// Set this id so we know what our players are
	ScorchedClient::instance()->getTargetContainer().
		setCurrentDestinationId(message.getDestinationId());

	// Tell the user to wait
	Logger::log(S3D::formatStringBuffer(
		"Connection accepted by \"%s\".\nPlease wait...",
		message.getServerName()));

	// Set the mod
	S3D::setDataFileMod(
		ScorchedClient::instance()->getOptionsGame().getMod());

	// Load any mod files we currently have for the mod
	// the server is using.
	if (ClientParams::instance()->getConnectedToServer())
	{
		ProgressCounter progressCounter;
		progressCounter.setNewPercentage(0.0f);

		if (!ScorchedClient::instance()->getModFiles().loadModFiles(
			ScorchedClient::instance()->getOptionsGame().getMod(), true,
			&progressCounter))
		{
			S3D::dialogMessage("ModFiles", 
				S3D::formatStringBuffer("Failed to load mod \"%s\"",
				ScorchedClient::instance()->getOptionsGame().getMod()));
			return false;
		}
	}

	// Tell the server what mod files we actually have
	ComsHaveModFilesMessage comsFileMessage;
	std::map<std::string, ModFileEntry *> &files = 
		ScorchedClient::instance()->getModFiles().getFiles();
	std::map<std::string, ModFileEntry *>::iterator itor;
	for (itor = files.begin();
		itor != files.end();
		++itor)
	{
		const std::string &name = (*itor).first;
		ModFileEntry *file = (*itor).second;

		comsFileMessage.getFiles().push_back(
			ModIdentifierEntry(
				true,
				name.c_str(),
				file->getUncompressedSize(),
				file->getUncompressedCrc()));
	}
	totalBytes_ = 0;
	if (!ComsMessageSender::sendToServer(comsFileMessage)) return false;

	ScorchedClient::instance()->getClientState().getClientLoadLevel().setInitialLevel(true);

	return true;
}

bool ClientStateInitialize::processFileMessage(
	NetMessage &netMessage,
	NetBufferReader &mainreader)
{
	ComsFileMessage message;
	if (!message.readMessage(mainreader)) return false;
	NetBufferReader reader(message.fileBuffer);

	std::map<std::string, ModFileEntry *> &files = 
		ScorchedClient::instance()->getModFiles().getFiles();

	// Read the files one at a time
	for (;;)
	{
		// Read the filename
		// a zero length name means last file
		std::string fileName;
		reader.getFromBuffer(fileName);
		if (fileName.size() == 0) break;

		// Check if we are adding or removing this file
		bool addFile = true;
		reader.getFromBuffer(addFile);
		if (addFile)
		{
			// Read flags
			bool firstChunk = false;
			bool lastChunk = false;
			reader.getFromBuffer(firstChunk);
			reader.getFromBuffer(lastChunk);

			// Read file count
			unsigned int bytesLeft = 0;
			reader.getFromBuffer(bytesLeft);
			if (totalBytes_ == 0) totalBytes_ = bytesLeft;

			// Update progress
			unsigned int doneBytes = totalBytes_ - bytesLeft;
			unsigned int percentage = 0;

			if (doneBytes > 0) {
				percentage = (unsigned int)(((doneBytes / 1024) * 100) / (totalBytes_ / 1024));
			}
			/*
			TODO
			ProgressDialog::instance()->progressChange(
				LANG_RESOURCE_3("DOWNLOADING_FILE", "Downloading mod, {0}% {1}/{2} KB", 
				percentage, (doneBytes / 1024), (totalBytes_ / 1024)), float(percentage));
			*/

			// Read the size
			unsigned int uncompressedSize = 0;
			unsigned int chunkSize = 0;
			reader.getFromBuffer(uncompressedSize);
			reader.getFromBuffer(chunkSize);
		
			// The first part
			if (firstChunk)
			{
				recvBuffer_.reset();
			}

			// Add the bytes to the file
			recvBuffer_.addDataToBuffer(
				reader.getBuffer() + reader.getReadSize(), chunkSize);
			reader.setReadSize(reader.getReadSize() + chunkSize);

			// Check if we have finished this file
			if (lastChunk)
			{
				// Finished
				Logger::log(S3D::formatStringBuffer(" %u/%u %s - %i bytes",
					doneBytes,
					totalBytes_,
					fileName.c_str(),
					recvBuffer_.getBufferUsed()));

				// Wrong size
				if (recvBuffer_.getBufferUsed() != uncompressedSize)
				{
					Logger::log(S3D::formatStringBuffer("Downloaded mod file incorrect size \"%s\".\n"
						"Expected %u, got %u.",
						fileName.c_str(), recvBuffer_.getBufferUsed(), uncompressedSize));
					return false;
				}

				// Write file
				if (!ModFileEntryLoader::writeModFile(recvBuffer_, fileName.c_str(),
					ScorchedClient::instance()->getOptionsGame().getMod()))
				{
					Logger::log(S3D::formatStringBuffer("Failed to write mod file \"%s\"",
						fileName.c_str()));
					return false;
				}


				// Remove the file if it already exists
				std::map<std::string, ModFileEntry *>::iterator findItor = 
					files.find(fileName);
				if (findItor != files.end())
				{
					delete (*findItor).second;
					files.erase(findItor);
				}

				// Create a new file
				ModFileEntry *fileEntry = new ModFileEntry(fileName, recvBuffer_.getCrc(), recvBuffer_.getBufferUsed());
				files[fileName] = fileEntry;
			}
		}
		else
		{
			// Remove the file 
			std::map<std::string, ModFileEntry *>::iterator findItor = 
				files.find(fileName);
			if (findItor != files.end())
			{
				ModFileEntryLoader::removeModFile(fileName, 
					ScorchedClient::instance()->getOptionsGame().getMod());

				Logger::log(S3D::formatStringBuffer(" %s - removed",
					fileName.c_str()));

				delete (*findItor).second;
				files.erase(findItor);
			}
		}
	}

	// Send the ak for this file
	ComsFileAkMessage akMessage;
	ComsMessageSender::sendToServer(akMessage);

	return true;
}

bool ClientStateInitialize::processInitializeModMessage(
	NetMessage &netMessage,
	NetBufferReader &reader)
{
	ComsInitializeModMessage message;
	if (!message.readMessage(reader)) return false;

	if (!initializeMod()) return false;

	// Tell the server we are initialized
	ComsMessageSender::sendToServer(message);
	return true;
}

bool ClientStateInitialize::initializeMod()
{
	ProgressCounter progressCounter;
	progressCounter.setNewPercentage(0.0f);

	// Re-load the tank names/sayings
	ScorchedClient::instance()->getTankAIStrings().load();

	// Load the accessory files
	if (!ScorchedClient::instance()->getAccessoryStore().parseFile(
		ScorchedClient::instance()->getContext(),
		&progressCounter)) return false;

	if (!ScorchedClient::instance()->getTanketTypes().
		loadTanketTypes(ScorchedClient::instance()->getContext())) {

		S3D::dialogMessage("Scorched 3D", "Failed to load tank types");
		return false;
	}

	// Load tank models here
	// This is after mods are complete but before any tanks models are used
	if (!ScorchedClient::instance()->getTankModels().loadTankMeshes(
		ScorchedClient::instance()->getContext(), 
		OptionsDisplay::instance()->getTankDetail(),
		&progressCounter))
	{
		S3D::dialogMessage("Scorched 3D", "Failed to load all tank models");
		return false;
	}

	// Load the landscape definitions
	if (!ScorchedClient::instance()->getLandscapes().readLandscapeDefinitions()) return false;

	// Load textures
	if (!ExplosionTextures::instance()->createTextures(&progressCounter)) 
			S3D::dialogExit("Scorched3D", "Failed to load explosion textures");
	GLLenseFlare::instance()->init(&progressCounter);
		
	return true;
}
