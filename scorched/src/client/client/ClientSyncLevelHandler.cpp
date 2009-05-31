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

#include <client/ClientSyncLevelHandler.h>
#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <client/ClientReloadAdaptor.h>
#include <common/Logger.h>
#include <common/Clock.h>
#include <coms/ComsMessageHandler.h>
#include <coms/ComsSyncLevelMessage.h>
#include <dialogs/ProgressDialog.h>
#include <landscape/Landscape.h>
#include <tankgraph/RenderTracer.h>
#include <graph/SpeedChange.h>
#include <graph/MainCamera.h>
#include <engine/ActionController.h>
#include <engine/MainLoop.h>
#include <tank/TankContainer.h>
#include <tank/TankCamera.h>
#include <target/TargetRenderer.h>

REGISTER_HANDLER(
	ComsSyncLevelMessage, 
	ComsMessageHandlerIRegistration::eClient, 
	new ClientSyncLevelHandler());

ClientSyncLevelHandler::ClientSyncLevelHandler()
{
}

ClientSyncLevelHandler::~ClientSyncLevelHandler()
{
}

bool ClientSyncLevelHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType, 
	NetBufferReader &reader)
{
	// Parsing the messages syncs the players
	ComsSyncLevelMessage message;
	if (!message.readMessage(reader)) return false;

	// Now sync the landscape
	Clock generateClock;
	DeformLandscape::applyInfos(
		ScorchedClient::instance()->getContext(),
		message.getDeformInfos(),
		ProgressDialogSync::noevents_instance());
	float deformTime = generateClock.getTimeDifference();
	Logger::log(S3D::formatStringBuffer("Landscape sync deformation time %.2f seconds", deformTime));

	// Make sure the landscape has been optimized
	Landscape::instance()->reset(ProgressDialogSync::noevents_instance());

	RenderTracer::instance()->newGame();
	SpeedChange::instance()->resetSpeed();

	// Remove all actions (graphical objects) from the last round
	ScorchedClient::instance()->getActionController().clear();

	// Tell all tanks to update transient settings
	ScorchedClient::instance()->getTankContainer().clientNewGame();

	// As we have not returned to the main loop for ages the
	// timer will have a lot of time in it
	// Get rid of this time so we don't screw things up
	ScorchedClient::instance()->getMainLoop().getTimer().getTimeDifference();

	// Reset camera positions for each tank
	std::map<unsigned int, Tank *>::iterator tankItor;
	for (tankItor = ScorchedClient::instance()->getTankContainer().getAllTanks().begin();
		tankItor != ScorchedClient::instance()->getTankContainer().getAllTanks().end();
		tankItor++)
	{
		Tank *current = (*tankItor).second;
		current->getRenderer()->moved();
		current->getCamera().setCameraType(1);
	}
	MainCamera::instance()->getTarget().setCameraType(TargetCamera::CamSpectator);
	ClientReloadAdaptor::instance();

	// Move into the wait state
	ScorchedClient::instance()->getGameState().stimulate(ClientState::StimWait);
	ScorchedClient::instance()->getGameState().checkStimulate();

	return true;
}
