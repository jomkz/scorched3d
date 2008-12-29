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

#include <client/ClientFileHandler.h>
#include <client/ClientState.h>
#include <client/ScorchedClient.h>
#include <engine/ModFiles.h>
#include <dialogs/ProgressDialog.h>
#include <common/Logger.h>
#include <common/Defines.h>
#include <common/OptionsScorched.h>
#include <coms/ComsMessageSender.h>
#include <coms/ComsFileMessage.h>
#include <coms/ComsFileAkMessage.h>
#include <lang/LangResource.h>

ClientFileHandler *ClientFileHandler::instance_ = 0;

ClientFileHandler *ClientFileHandler::instance()
{
	if (!instance_)
	{
		instance_ = new ClientFileHandler;
	}
	return instance_;
}

ClientFileHandler::ClientFileHandler() : totalBytes_(0)
{
	ScorchedClient::instance()->getComsMessageHandler().addHandler(
		"ComsFileMessage",
		this);
}

ClientFileHandler::~ClientFileHandler()
{
}

bool ClientFileHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &mainreader)
{
	ComsFileMessage message;
	if (!message.readMessage(mainreader)) return false;
	NetBufferReader reader(message.fileBuffer);

	if (ScorchedClient::instance()->getGameState().getState() !=
		ClientState::StateLoadFiles)
	{
		ScorchedClient::instance()->getGameState().stimulate(
			ClientState::StimLoadFiles);
	}

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
		const char *shortFileName = fileName.c_str();
		if (strrchr(shortFileName, '/')) shortFileName = strrchr(shortFileName, '/') + 1;
		unsigned int doneBytes = totalBytes_ - bytesLeft;
		ProgressDialog::instance()->progressChange(
			LANG_RESOURCE_1("DOWNLOADING_FILE", "Downloading {0}", shortFileName), 
			float(doneBytes * 100 / totalBytes_));

		// Read the size
		unsigned int maxsize = 0;
		unsigned int uncompressedsize = 0;
		unsigned int crc = 0;
		unsigned int size = 0;
		reader.getFromBuffer(maxsize);
		reader.getFromBuffer(uncompressedsize);
		reader.getFromBuffer(crc);
		reader.getFromBuffer(size);
		
		// The first part
		if (firstChunk)
		{
			// Remove the file if it already exists
			std::map<std::string, ModFileEntry *>::iterator findItor = 
				files.find(fileName);
			if (findItor != files.end())
			{
				delete (*findItor).second;
				files.erase(findItor);
			}

			// Create a new file
			ModFileEntry *fileEntry = new ModFileEntry;
			fileEntry->setFileName(fileName.c_str());
			fileEntry->setCompressedCrc(crc);
			fileEntry->setUncompressedSize(uncompressedsize);
			files[fileName] = fileEntry;
		}

		// Locate the file
		std::map<std::string, ModFileEntry *>::iterator findItor = 
			files.find(fileName);
		if (findItor == files.end())
		{
			Logger::log(S3D::formatStringBuffer("Failed to find partial mod file \"%s\"", 
				fileName.c_str()));
			return false;
		}
		ModFileEntry *entry = (*findItor).second;

		// Add the bytes to the file
		entry->getCompressedBuffer().addDataToBuffer(
			reader.getBuffer() + reader.getReadSize(), size);
		reader.setReadSize(reader.getReadSize() + size);

		// Check if we have finished this file
		if (lastChunk)
		{
			// Finished
			Logger::log(S3D::formatStringBuffer(" %u/%u %s - %i bytes",
				doneBytes,
				totalBytes_,
				fileName.c_str(),
				entry->getCompressedSize()));

			// Wrong size
			if (entry->getCompressedSize() != maxsize)
			{
				Logger::log(S3D::formatStringBuffer("Downloaded mod file incorrect size \"%s\".\n"
					"Expected %u, got %u.",
					fileName.c_str(), entry->getCompressedSize(), maxsize));
				return false;
			}

			// Write file
			if (!entry->writeModFile(fileName.c_str(),
				ScorchedClient::instance()->getOptionsGame().getMod()))
			{
				Logger::log(S3D::formatStringBuffer("Failed to write mod file \"%s\"",
					fileName.c_str()));
				return false;
			}
		}
	}

	// Send the ak for this file
	ComsFileAkMessage akMessage;
	ComsMessageSender::sendToServer(akMessage);

	return true;
}
