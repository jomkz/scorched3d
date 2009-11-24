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

#include <client/ClientLoadLevelHandler.h>
#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <client/ClientReloadAdaptor.h>
#include <client/ClientSimulator.h>
#include <common/Clock.h>
#include <common/OptionsScorched.h>
#include <common/Logger.h>
#include <common/ChannelText.h>
#include <common/ChannelManager.h>
#include <engine/MainLoop.h>
#include <dialogs/ProgressDialog.h>
#include <dialogs/CameraDialog.h>
#include <coms/ComsLoadLevelMessage.h>
#include <coms/ComsLevelLoadedMessage.h>
#include <coms/ComsMessageSender.h>
#include <graph/OptionsDisplayConsole.h>
#include <graph/ParticleEngine.h>
#include <graph/MainCamera.h>
#include <graph/SpeedChange.h>
#include <tankgraph/RenderTracer.h>
#include <landscapedef/LandscapeDefinitions.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscape/Landscape.h>
#include <lang/LangResource.h>
#include <tank/TankContainer.h>
#include <tank/TankCamera.h>
#include <target/TargetRenderer.h>
#include <net/NetInterface.h>

ClientLoadLevelHandler *ClientLoadLevelHandler::instance_ = 0;

ClientLoadLevelHandler *ClientLoadLevelHandler::instance()
{
	if (!instance_)
	{
	  instance_ = new ClientLoadLevelHandler();
	}

	return instance_;
}

ClientLoadLevelHandler::ClientLoadLevelHandler()
{
	ScorchedClient::instance()->getComsMessageHandler().addHandler(
		ComsLoadLevelMessage::ComsLoadLevelMessageType,
		this);
}

ClientLoadLevelHandler::~ClientLoadLevelHandler()
{

}

bool ClientLoadLevelHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	Clock generateClock;
	bool result = actualProcessMessage(netMessage, messageType, reader);
	float generateTime = generateClock.getTimeDifference();
	int idleTime = ScorchedClient::instance()->getOptionsGame().getBuyingTime();

	if (idleTime > 0 && int(generateTime) > idleTime - 5)
	{
		LangString message = LANG_RESOURCE("LEVEL_TIMEOUT_WARNING",
			"Warning: Your PC is taking a long time to generate levels.\n"
			"This may cause you to be kicked by some servers.\n"
			"You can fix this by lowering your display settings");

		Logger::log(LangStringUtil::convertFromLang(message));
		ChannelText text("info", message);
		ChannelManager::showText(ScorchedClient::instance()->getContext(), text);
	} 
	else
	{
		Logger::log(S3D::formatStringBuffer(
			"Finished loading landscape %.2f seconds", generateTime));
	}

	return result;
}

bool ClientLoadLevelHandler::actualProcessMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	// Move into the load level state
	ScorchedClient::instance()->getGameState().stimulate(ClientState::StimLoadLevel);

	// Read the message
	ComsLoadLevelMessage message;
	if (!message.readMessage(reader)) return false;

	// Read the state from the message
	if (!message.loadState(ScorchedClient::instance()->getContext())) return false;

	// make sure we can only see the correct settings
	OptionsDisplayConsole::instance()->addDisplayToConsole();

	// Set the progress dialog nicities
	ProgressDialog::instance()->changeTip();
	LandscapeDefinitionsEntry *landscapeDefinition =
		ScorchedClient::instance()->getLandscapes().getLandscapeByName(
		message.getLandscapeDefinition().getName());
	if (landscapeDefinition)
	{
		std::string fileName = S3D::getModFile(
			S3D::formatStringBuffer("data/landscapes/%s", 
			landscapeDefinition->picture.c_str()));
		ProgressDialog::instance()->setIcon(fileName.c_str());
	}

	// Generate new landscape
	ScorchedClient::instance()->getLandscapeMaps().generateMaps(
		ScorchedClient::instance()->getContext(),
		message.getLandscapeDefinition(),
		ProgressDialogSync::noevents_instance());

	// Reset and initialize simulator and
	ScorchedClient::instance()->getClientSimulator().newLevel();

	// Calculate all the new landscape settings (graphics)
	Landscape::instance()->generate(ProgressDialogSync::noevents_instance());

	// Add all missed actions to the simulator
	std::list<ComsSimulateMessage *> simulateMessages;
	std::list<ComsSimulateMessage *>::iterator messageItor;
	if (!message.getSimulations(simulateMessages)) return false;
	for (messageItor = simulateMessages.begin();
		messageItor != simulateMessages.end();
		messageItor++)
	{
		ComsSimulateMessage *simMessage = *messageItor;
		ScorchedClient::instance()->getClientSimulator().
			addComsSimulateMessage(*simMessage, true);
		delete simMessage;
	}
	simulateMessages.clear();

	// Sync the simulator
	Clock generateClock;
	ScorchedClient::instance()->getClientSimulator().setSimulationTime(message.getActualTime());
	float deformTime = generateClock.getTimeDifference();
	Logger::log(S3D::formatStringBuffer("Landscape sync event time %.2f seconds", deformTime));

	// Make sure the landscape has been optimized
	Landscape::instance()->recalculate();

	// Reset stuff
	ScorchedClient::instance()->
		getParticleEngine().killAll();
	MainCamera::instance()->getTarget().
		getPrecipitationEngine().killAll();
	CameraDialog::instance()->getCamera().
		getPrecipitationEngine().killAll();
	RenderTracer::instance()->newGame();
	SpeedChange::instance()->resetSpeed();

	// Process any outstanding coms messages
	ScorchedClient::instance()->getNetInterface().processMessages();

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

	// Tell the server we have finished processing the landscape
	ComsLevelLoadedMessage levelLoadedMessage;
	ComsMessageSender::sendToServer(levelLoadedMessage);

	// Move into the wait state
	if (ScorchedClient::instance()->getGameState().getState() == ClientState::StateLoadLevel)
	{
		if (strcmp(message.getLandscapeDefinition().getName(), "blank") == 0)
		{
			ScorchedClient::instance()->getGameState().stimulate(ClientState::StimWaitNoLandscape);
		}
		else
		{
			ScorchedClient::instance()->getGameState().stimulate(ClientState::StimWait);
		}
	}
	return true;
}
