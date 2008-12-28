////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
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

#include <server/ServerHaveModFilesHandler.h>
#include <server/ServerChannelManager.h>
#include <server/ScorchedServer.h>
#include <server/ServerCommon.h>
#include <engine/ModFiles.h>
#include <coms/ComsHaveModFilesMessage.h>
#include <common/OptionsScorched.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <tank/TankMod.h>
#include <tank/TankContainer.h>
#include <time.h>

ServerHaveModFilesHandler *ServerHaveModFilesHandler::instance()
{
	static ServerHaveModFilesHandler *instance = 
		new ServerHaveModFilesHandler;
	return instance;
}

ServerHaveModFilesHandler::ServerHaveModFilesHandler()
{
	ScorchedServer::instance()->getComsMessageHandler().addHandler(
		"ComsHaveModFilesMessage",
		this);
}

ServerHaveModFilesHandler::~ServerHaveModFilesHandler()
{
}

bool ServerHaveModFilesHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	ComsHaveModFilesMessage message;
	if (!message.readMessage(reader)) return false;

	unsigned int destinationId = netMessage.getDestinationId();

	std::list<ModFileEntry *> neededEntries_;
	unsigned int neededLength = 0;

	// Build up a list of mod files that this client needs
	// and does not already have
	{
		std::map<std::string, ModFileEntry *> &modFiles = 
			ScorchedServer::instance()->getModFiles().getFiles();
		std::map<std::string, ModFileEntry *>::iterator itor;
		for (itor = modFiles.begin();
			itor != modFiles.end();
			itor++)
		{
			const std::string &fileName = (*itor).first;
			ModFileEntry *fileEntry = (*itor).second;

			ModIdentifierEntry *hasEntry = 
				message.getFile(fileName.c_str());
			if (!hasEntry ||
				hasEntry->crc != fileEntry->getCompressedCrc() ||
				hasEntry->length != fileEntry->getCompressedSize())
			{
#ifdef _DEBUG
				if (!hasEntry)
				{
					ServerCommon::serverLog(
						S3D::formatStringBuffer("Mod download \"%s\" new file",
							fileName.c_str()));
				}
				else if (hasEntry->length != fileEntry->getCompressedSize())
				{
					ServerCommon::serverLog(
						S3D::formatStringBuffer("Mod download \"%s\" size difference %u %u",
						fileName.c_str(),
						hasEntry->length, 
						fileEntry->getCompressedSize()));
				}
				else if (hasEntry->crc != fileEntry->getCompressedCrc())
				{
					ServerCommon::serverLog(
						S3D::formatStringBuffer("Mod download \"%s\" crc difference %u %u",
						fileName.c_str(),
						hasEntry->crc, 
						fileEntry->getCompressedCrc()));
				}
#endif //_DEBUG

				neededEntries_.push_back(fileEntry);
				neededLength += fileEntry->getCompressedSize();
			}
		}
	}

	if (neededEntries_.empty())
	{
		// No files need downloading
		ServerChannelManager::instance()->sendText(
			ChannelText("info",
				"NO_MOD_FILES",
				"No mod files need downloading"),
			netMessage.getDestinationId(), 
			false);
	}
#ifndef S3D_SERVER
	else
	{
		// Do a sanity check that single player games don't need to download
		// any mod files.  As the server and client is the same process and
		// should be using the same mod directory.
		S3D::dialogExit("ModFiles",
			"ERROR: Single player client required mod files");
	}
#else
	else if (ScorchedServer::instance()->getOptionsGame().getModDownloadSpeed() == 0)
	{
		// If this server does not allow file downloads tell the client
		// and disconnect them
		Logger::log("No mod and mod download disabled");
		ServerChannelManager::instance()->sendText(
			ChannelText("info",
				"SERVER_REQUIRES_MOD",
				"This server requires the \"{0}\" Scorched3D mod.\n"
				"The server does not allow in game file downloads.\n"
				"You must download and install this mod before you\n"
				"can connect to this server.",
				ScorchedServer::instance()->getOptionsGame().getMod()),
			destinationId,
			false);
		ServerCommon::kickDestination(destinationId);
	}
	else 
	{
		int timeLeft = int(
				float(neededLength) / 
				float(ScorchedServer::instance()->getOptionsGame().getModDownloadSpeed())
			);
		int timeMinutes = timeLeft / 60;
		int timeSeconds = timeLeft % 60;
		// This server allows file downloads
		// The the client how much needs to be downloaded
		ServerChannelManager::instance()->sendText(
			ChannelText("info",
				"SERVER_DOWNLOAD_MOD",
				"This server requires the \"{0}\" Scorched3D mod.\n"
				"This will require downloading {1} bytes\n"
				"Maximun {2} bytes per second = Minimum of {3} minutes\n"
				"Note: This will also depend on how many server\n"
				"downloads and your link speed.",
				ScorchedServer::instance()->getOptionsGame().getMod(),
				neededLength,
				ScorchedServer::instance()->getOptionsGame().getModDownloadSpeed(),
				timeMinutes),
			destinationId,
			false);
	}
#endif

	// Set the files to download in this tanks profile
	std::map<unsigned int, Tank *> &tanks = 
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		// For each tank
		Tank *tank = (*itor).second;
		if (netMessage.getDestinationId() == tank->getDestinationId())
		{
			// and for each needed entry
			std::list<ModFileEntry *>::iterator neededItor;
			for (neededItor = neededEntries_.begin();
				neededItor != neededEntries_.end();
				neededItor ++)
			{
				ModFileEntry *entry = (*neededItor);

				// Add the entry this tank needs to download
				ModIdentifierEntry newEntry(entry->getFileName(),
					0, entry->getCompressedCrc());
				tank->getMod().addFile(newEntry);
			}
			tank->getMod().setInit(true);
			tank->getMod().setTotalLeft(neededLength);
		}
	}

	return true;
}
