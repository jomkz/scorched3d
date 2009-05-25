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

#include <client/ClientConnectionAcceptHandler.h>
#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <dialogs/RulesDialog.h>
#include <dialogs/ConnectDialog.h>
#include <dialogs/ProgressDialog.h>
#include <engine/ModFiles.h>
#include <image/ImageFactory.h>
#include <image/ImagePng.h>
#include <tank/TankContainer.h>
#include <net/NetInterface.h>
#include <coms/ComsConnectAcceptMessage.h>
#include <coms/ComsHaveModFilesMessage.h>
#include <coms/ComsMessageSender.h>
#include <common/Logger.h>
#include <common/OptionsScorched.h>
#include <client/ClientParams.h>
#include <common/Defines.h>

ClientConnectionAcceptHandler *ClientConnectionAcceptHandler::instance_ = 0;

ClientConnectionAcceptHandler *ClientConnectionAcceptHandler::instance()
{
	if (!instance_)
	{
	  instance_ = new ClientConnectionAcceptHandler();
	}

	return instance_;
}

ClientConnectionAcceptHandler::ClientConnectionAcceptHandler()
{
	ScorchedClient::instance()->getComsMessageHandler().addHandler(
		"ComsConnectAcceptMessage",
		this);
}

ClientConnectionAcceptHandler::~ClientConnectionAcceptHandler()
{

}

bool ClientConnectionAcceptHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	ComsConnectAcceptMessage message;
	if (!message.readMessage(reader)) return false;

	if (ClientParams::instance()->getConnectedToServer())
	{
		unsigned int ip = netMessage.getIpAddress();
		if (!ConnectDialog::instance()->getIdStore().saveUniqueId(
			ip, message.getUniqueId(), message.getPublishAddress()))
		{
			Logger::log( "Server failed ip security check!");
			return false;
		}
	}

	// The server tells us what our id is.
	// Set this id so we know what our players are
	ScorchedClient::instance()->getTankContainer().
		setCurrentDestinationId(message.getDestinationId());

	// Tell the user to wait
	Logger::log(S3D::formatStringBuffer(
		"Connection accepted by \"%s\".\nPlease wait...",
		message.getServerName()));

	// Set the server specific png for the current server
	{
		GLTexture *texture = 0;
		if (message.getServerPng().getBufferUsed() > 0)
		{
			// Use a custom icon
			ImagePng map;
			map.loadFromBuffer(message.getServerPng());

			// Set the texture
			texture = new GLTexture;
			texture->create(map, false);
		}
		else
		{
			// Use the default icon
			std::string file1(S3D::getDataFile("data/images/scorched.png"));
			std::string file2(S3D::getDataFile("data/images/scorcheda.png"));
			ImageHandle map = ImageFactory::loadImageHandle(
				file1.c_str(), file2.c_str(), false);

			// Set the texture
			texture = new GLTexture;
			texture->create(map, false);
		}
		
		RulesDialog::instance()->addIcon(texture);
	}

	// Set the mod
	S3D::setDataFileMod(
		ScorchedClient::instance()->getOptionsGame().getMod());

	// Load any mod files we currently have for the mod
	// the server is using.
	if (ClientParams::instance()->getConnectedToServer())
	{
		if (!ScorchedClient::instance()->getModFiles().loadModFiles(
			ScorchedClient::instance()->getOptionsGame().getMod(), true,
			ProgressDialogSync::instance()))
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
		itor++)
	{
		const std::string &name = (*itor).first;
		ModFileEntry *file = (*itor).second;

		comsFileMessage.getFiles().push_back(
			ModIdentifierEntry(
				name.c_str(),
				file->getCompressedSize(),
				file->getCompressedCrc()));
	}
	if (!ComsMessageSender::sendToServer(comsFileMessage)) return false;

	// Move into the files state
	ScorchedClient::instance()->getGameState().stimulate(ClientState::StimLoadFiles);
	ScorchedClient::instance()->getGameState().checkStimulate();

	return true;
}
