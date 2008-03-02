////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
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

#include <client/ClientInitializeHandler.h>
#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <client/ClientWindowSetup.h>
#include <sprites/ExplosionTextures.h>
#include <server/ScorchedServer.h>
#include <weapons/AccessoryStore.h>
#include <landscapedef/LandscapeDefinitions.h>
#include <common/Logger.h>
#include <common/OptionsScorched.h>
#include <client/ClientParams.h>
#include <dialogs/ProgressDialog.h>
#include <dialogs/PlayerDialog.h>
#include <tank/TankModelStore.h>
#include <engine/ModFiles.h>
#include <graph/OptionsDisplay.h>
#include <coms/ComsInitializeMessage.h>
#include <coms/ComsMessageSender.h>
#include <GLW/GLWWindowManager.h>
#include <GLW/GLWWindowSkinManager.h>
#include <GLEXT/GLLenseFlare.h>

ClientInitializeHandler *ClientInitializeHandler::instance_ = 0;

ClientInitializeHandler *ClientInitializeHandler::instance()
{
	if (!instance_)
	{
		instance_ = new ClientInitializeHandler;
	}
	return instance_;
}

ClientInitializeHandler::ClientInitializeHandler() :
	initialized_(false)
{
	ScorchedClient::instance()->getComsMessageHandler().addHandler(
		"ComsInitializeMessage",
		this);
}

ClientInitializeHandler::~ClientInitializeHandler()
{
}

bool ClientInitializeHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	ComsInitializeMessage message;
	if (!message.readMessage(reader)) return false;

	if (!initialized_)
	{
		if (!initialize()) return false;
		else initialized_ = true;
	}

	// Tell the server we are initialized
	ComsMessageSender::sendToServer(message);
	return true;
}

bool ClientInitializeHandler::initialize()
{
	// Clear any memory used by stored mod files as they will not be required now
	ScorchedClient::instance()->getModFiles().clearData();
	if (!ClientParams::instance()->getConnectedToServer())
	{
		ScorchedServer::instance()->getModFiles().clearData();
	}

	// Load the accessory files
	if (!ScorchedClient::instance()->getAccessoryStore().parseFile(
		ScorchedClient::instance()->getOptionsGame(),
		ProgressDialogSync::instance())) return false;

	// Load tank models here
	// This is after mods are complete but before any tanks models are used
	if (!ScorchedClient::instance()->getTankModels().loadTankMeshes(
		ScorchedClient::instance()->getContext(), 
		OptionsDisplay::instance()->getTankDetail(),
		ProgressDialogSync::instance()))
	{
		S3D::dialogMessage("Scorched 3D", "Failed to load all tank models");
		return false;
	}

	// Load the landscape definitions
	if (!ScorchedClient::instance()->getLandscapes().readLandscapeDefinitions()) return false;

	// Load textures
	if (!ExplosionTextures::instance()->createTextures(
		ProgressDialogSync::instance())) 
			S3D::dialogExit("Scorched3D", "Failed to load explosion textures");
	GLLenseFlare::instance()->init(
		ProgressDialogSync::instance());
		
	// Load game windows
	ProgressDialogSync::instance()->setNewPercentage(0.0f);
	ProgressDialogSync::instance()->setNewOp("Initializing Game Windows");
	if (!GLWWindowSkinManager::modinstance()->loadWindows())
	{
		S3D::dialogExit("Scorched3D", "Failed to load windows skins");
	}
	ClientWindowSetup::setupGameWindows(GLWWindowSkinManager::modinstance());
	GLWWindowManager::instance()->loadPositions();		

	// Move into the player setup state
	if (ClientParams::instance()->getSaveFile()[0])
	{
		ScorchedClient::instance()->getGameState().stimulate(ClientState::StimLoadPlayers);
	}
	else
	{
		ScorchedClient::instance()->getGameState().stimulate(ClientState::StimGetPlayers);
		// Show player dialog
		GLWWindowManager::instance()->showWindow(
			PlayerDialog::instance()->getId());	
	}
	ScorchedClient::instance()->getGameState().checkStimulate();
	return true;
}
