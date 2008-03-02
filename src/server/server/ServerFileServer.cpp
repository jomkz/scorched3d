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

#include <server/ServerFileServer.h>
#include <server/ScorchedServer.h>
#include <server/ServerCommon.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <tank/TankMod.h>
#include <common/OptionsScorched.h>
#include <common/Defines.h>
#include <coms/ComsMessageSender.h>
#include <coms/ComsInitializeMessage.h>
#include <engine/ModFiles.h>
#include <time.h>

ServerFileServer *ServerFileServer::instance_ = 0;

ServerFileServer *ServerFileServer::instance()
{
	if (!instance_)
	{
		instance_ = new ServerFileServer;
	}
	return instance_;
}

ServerFileServer::ServerFileServer() : lastTime_(0), bytesSent_(0)
{
}

ServerFileServer::~ServerFileServer()
{
}

void ServerFileServer::simulate(float timeDifference)
{
	int downloadCount = 0;

	// TODO this should really be done on a per destination id basis
	// not on a per tank basis!!

	// Check how many people are currently downloading
	// also check for any that have finished downloading
	std::map<unsigned int, Tank *> tanks = 
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		Tank *tank = (*itor).second;
		if (tank->getState().getState() == TankState::sLoading)
		{
			// Does this tank have any more files to send
			if (!tank->getMod().getFiles().empty())
			{
				// Yes, 1 more tank to send to
				downloadCount++;
			}
			else
			{
				// Check if this tank had had a chance to get its mod files
				if (tank->getMod().getInit())
				{
					// Set this tank to finished
					tank->getState().setState(TankState::sInitializing);

					// If all tanks at this destination have finished send init message
					bool allComplete = true;
					std::map<unsigned int, Tank *>::iterator seconditor;
					for (seconditor = tanks.begin();
						seconditor != tanks.end();
						seconditor++)
					{
						Tank *secondtank = (*seconditor).second;
						if (secondtank->getDestinationId() == tank->getDestinationId())
						{
							if (secondtank->getState().getState() ==
								TankState::sLoading)
							{
								allComplete = false;
								break;
							}
						}
					}

					// If this tank is not initialized make it initialized
					if (allComplete)
					{
						// Tell this destination to start initializing
						ComsInitializeMessage initMessage;
						ComsMessageSender::sendToSingleClient(initMessage,
							tank->getDestinationId());
					}
				}
			}
		}
	}

	// Do we allow file downloads
	if (ScorchedServer::instance()->getOptionsGame().getModDownloadSpeed() == 0)
	{
		return;
	}

	// If no people are downloading then there is nothing to do
	if (downloadCount == 0) return;

	// Cacluate the maximum amount that can be downloaded by each
	// client per second
	int maxDownloadPerClient = 
		ScorchedServer::instance()->getOptionsGame().getModDownloadSpeed() /
		downloadCount;

	// Check if this is the next second
	unsigned int theTime = (unsigned int) time(0);
	if (theTime > lastTime_)
	{
		// If so reset the amount that can be sent by this server
		lastTime_ = theTime;
		bytesSent_ = 0;

		// Reset the sent state for each client
		for (itor = tanks.begin();
			itor != tanks.end();
			itor++)
		{
			Tank *tank = (*itor).second;
			if (tank->getState().getState() == TankState::sLoading)
			{
				tank->getMod().setSent(false);
			}
		}
	}

	// Send bytes to each ready client
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		// Make sure we have not sent too much this second already
		if (bytesSent_ + maxDownloadPerClient > 
			(unsigned int) ScorchedServer::instance()->getOptionsGame().getModDownloadSpeed() + 5)
		{
			return;
		}

		Tank *tank = (*itor).second;
		if (tank->getState().getState() == TankState::sLoading)
		{
			// Check if the client is ready to recieve more
			// bytes and there is some to send and
			// we have not sent to this client this second
			if (tank->getMod().getReadyToReceive() &&
				!tank->getMod().getFiles().empty() &&
				!tank->getMod().getSent())
			{
				// Send bytes to this tank
				tank->getMod().setSent(true);
				tank->getMod().setReadyToReceive(false);
				bytesSent_ += maxDownloadPerClient;

				sendBytes(tank, maxDownloadPerClient);
			}
		}
	}
}

void ServerFileServer::sendBytes(Tank *tank, unsigned int size)
{
	ComsFileMessage message;
	// Fill up the message with files, until it is full
	while (size > 0)
	{
		unsigned int bytesSent = 0;
		if (!sendNextFile(message, tank, size, bytesSent)) break;
		size -= bytesSent;
	}
	// Add any empty file name to signal end of files for this message
	message.fileBuffer.addToBuffer("");

	// Send the message to the client
	ComsMessageSender::sendToSingleClient(message, 
		tank->getDestinationId());

}

bool ServerFileServer::sendNextFile(ComsFileMessage &message,
	Tank *tank, unsigned int size,
	unsigned int &bytesSent)
{
	// Get the next file to send
	std::list<ModIdentifierEntry> &files = 
		tank->getMod().getFiles();
	if (files.empty()) return false;
	ModIdentifierEntry &entry = files.front();

	// Find the next file in the modfiles
	std::map<std::string, ModFileEntry *> &modfiles =
		ScorchedServer::instance()->getModFiles().getFiles();
	std::map<std::string, ModFileEntry *>::iterator findItor =
		modfiles.find(entry.fileName);
	DIALOG_ASSERT(findItor != modfiles.end());
	ModFileEntry *modentry = (*findItor).second;

    // Check how much still needs to be sent
	unsigned int sizeSent = entry.length;
	unsigned int sizeLeftToSend = modentry->getCompressedSize() - sizeSent;
	unsigned int sizeToSend = MIN(sizeLeftToSend, size);
	unsigned int bytesLeft = tank->getMod().getTotalLeft();
	bool firstChunk = (sizeSent == 0);
	bool lastChunk = (sizeToSend == sizeLeftToSend);

	// Add the bytes to the buffer
	message.fileBuffer.addToBuffer(modentry->getFileName());
	message.fileBuffer.addToBuffer(firstChunk);
	message.fileBuffer.addToBuffer(lastChunk);
	message.fileBuffer.addToBuffer(bytesLeft);
	message.fileBuffer.addToBuffer(modentry->getCompressedSize());
	message.fileBuffer.addToBuffer(modentry->getUncompressedSize());
	message.fileBuffer.addToBuffer(modentry->getCompressedCrc());
	message.fileBuffer.addToBuffer(sizeToSend);
	message.fileBuffer.addDataToBuffer(modentry->getCompressedBytes() + entry.length,
		sizeToSend);

	// Update how much we have sent
	entry.length += sizeToSend;
	tank->getMod().setTotalLeft(tank->getMod().getTotalLeft() - sizeToSend);

	// Have we sent the whole file
	if (sizeToSend == sizeLeftToSend)
	{
		// If so remove the file from the list that
		// still needs to be sent
		std::string fileName = modentry->getFileName();
		tank->getMod().rmFile(fileName.c_str());
	}

	bytesSent = sizeToSend;
	return true;
}

