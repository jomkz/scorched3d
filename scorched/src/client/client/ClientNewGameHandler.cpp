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

#include <client/ScorchedClient.h>
#include <client/ClientNewGameHandler.h>
#include <client/ClientState.h>
#include <client/ClientReloadAdaptor.h>
#include <graph/SpeedChange.h>
#include <graph/OptionsDisplayConsole.h>
#include <graph/MainCamera.h>
#include <tankgraph/RenderTracer.h>
#include <weapons/AccessoryStore.h>
#include <engine/ActionController.h>
#include <engine/MainLoop.h>
#include <GLW/GLWWindowManager.h>
#include <client/ClientParams.h>
#include <common/OptionsScorched.h>
#include <common/ChannelManager.h>
#include <common/Clock.h>
#include <common/Logger.h>
#include <lang/LangResource.h>
#include <coms/ComsNewGameMessage.h>
#include <dialogs/ProgressDialog.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapemap/DeformLandscape.h>
#include <landscapedef/LandscapeDefinitions.h>
#include <landscape/Landscape.h>
#include <tank/TankContainer.h>
#include <tank/TankCamera.h>
#include <target/TargetRenderer.h>
#include <tankai/TankAIAdder.h>

ClientNewGameHandler *ClientNewGameHandler::instance_ = 0;

ClientNewGameHandler *ClientNewGameHandler::instance()
{
	if (!instance_)
	{
	  instance_ = new ClientNewGameHandler();
	}

	return instance_;
}

ClientNewGameHandler::ClientNewGameHandler()
{
	ScorchedClient::instance()->getComsMessageHandler().addHandler(
		ComsNewGameMessage::ComsNewGameMessageType,
		this);
}

ClientNewGameHandler::~ClientNewGameHandler()
{

}

bool ClientNewGameHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	Clock generateClock;
	bool result = actualProcessMessage(netMessage, messageType, reader);
	float generateTime = generateClock.getTimeDifference();
	int idleTime = ScorchedClient::instance()->getOptionsGame().getIdleKickTime();

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

	return result;
}

bool ClientNewGameHandler::actualProcessMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	ComsNewGameMessage message;
	if (!message.readMessage(reader)) return false;

	// Update all of the tank state
	// Do this at the begining as the ProgressDialog processes network
	// events and tanks may leave an join meantime
	if (!message.parsePlayerStateMessage())
	{
		Logger::log("ClientNewGameHandler: Failed to parse playerstate");
		return false;
	}

	// make sure we can only see the correct settings
	OptionsDisplayConsole::instance()->addDisplayToConsole();

	// Set the progress dialog nicities
	ProgressDialog::instance()->changeTip();
	LandscapeDefinitionsEntry *landscapeDefinition =
		ScorchedClient::instance()->getLandscapes().getLandscapeByName(
			message.getLevelMessage().getGroundMapsDefn().getName());
	if (landscapeDefinition)
	{
		std::string fileName = S3D::getModFile(
			S3D::formatStringBuffer("data/landscapes/%s", 
			landscapeDefinition->picture.c_str()));
		ProgressDialog::instance()->setIcon(fileName.c_str());
	}

	// Remove any old targets
	removeTargets();

	// Generate new landscape
	ScorchedClient::instance()->getLandscapeMaps().generateMaps(
		ScorchedClient::instance()->getContext(),
		message.getLevelMessage().getGroundMapsDefn(),
		ProgressDialogSync::events_instance());

	Clock generateClock;
	DeformLandscape::applyInfos(
		ScorchedClient::instance()->getContext(),
		message.getLevelMessage().getDeformInfos(),
		ProgressDialogSync::events_instance());
	float deformTime = generateClock.getTimeDifference();
	Logger::log(S3D::formatStringBuffer("Landscape deformation time %.2f seconds", deformTime));

	// Calculate all the new landscape settings (graphics)
	Landscape::instance()->generate(ProgressDialogSync::events_instance());

	// Remove any targets that have been removed due to game play
	std::map<unsigned int, Target *> targets =
		ScorchedClient::instance()->getTargetContainer().getTargets(); // Copy
	std::map<unsigned int, Target *>::iterator targetItor;
	for (targetItor = targets.begin();
		targetItor != targets.end();
		targetItor++)
	{
		Target *target = targetItor->second;
		if (target->getPlayerId() >= TargetID::MIN_TARGET_ID &&
			target->getPlayerId() < TargetID::MIN_TARGET_TRANSIENT_ID)
		{
			if (message.getLevelMessage().getTargetIds().find(target->getPlayerId()) ==
				message.getLevelMessage().getTargetIds().end())
			{
				if (target->isTarget())
				{
					Target *removedTarget = 
						ScorchedClient::instance()->getTargetContainer().removeTarget(target->getPlayerId());
					delete removedTarget;
				}
				else
				{
					// Should never happen as tanks (even temporary ones) aren't removed
					// from the container set when killed
					Tank *removedTank = 
						ScorchedClient::instance()->getTankContainer().removeTank(target->getPlayerId());
					delete removedTank;
				}
			}
		}
	}

	// Add any targets that have been added due to game play
	NetBufferReader newTargets(message.getLevelMessage().getNewTargets());
	unsigned int newTargetId = 0;
	while (newTargets.getFromBuffer(newTargetId))
	{
		Target *newTarget = new Target(
			newTargetId, 
			LangString(), 
			ScorchedClient::instance()->getContext());
		if (!newTarget->readMessage(newTargets))
		{
			Logger::log("ClientNewGameHandler: Failed to parse new target message");
			return false;
		}

		ScorchedClient::instance()->getTargetContainer().addTarget(newTarget);
	}

	// Change any target values that may have been altered
	NetBufferReader oldTargets(message.getLevelMessage().getOldTargets());
	unsigned int oldTargetId = 0;
	while (oldTargets.getFromBuffer(oldTargetId))
	{
		Target *oldTarget = ScorchedClient::instance()->
			getTargetContainer().getTargetById(oldTargetId);
		if (!oldTarget)
		{
			Logger::log("ClientNewGameHandler: Failed to find old target");
			return false;
		}

		if (!oldTarget->readMessage(oldTargets))
		{
			Logger::log("ClientNewGameHandler: Failed to parse old target message");
			return false;
		}
	}

	// Make sure the landscape has been optimized
	Landscape::instance()->reset(ProgressDialogSync::events_instance());

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

	// Tell the server we have finished processing the landscape
	ScorchedClient::instance()->getGameState().stimulate(ClientState::StimWait);
	ClientReloadAdaptor::instance();

	// Move into the wait state
	ScorchedClient::instance()->getGameState().stimulate(ClientState::StimWait);
	ScorchedClient::instance()->getGameState().checkStimulate();

	return true;
}

void ClientNewGameHandler::removeTargets()
{
	std::map<unsigned int, Target *> targets = // Note copy
		ScorchedClient::instance()->getTargetContainer().getTargets();
	std::map<unsigned int, Target *>::iterator itor;
	for (itor = targets.begin();
		itor != targets.end();
		itor++)
	{
		unsigned int playerId = (*itor).first;
		Target *target = (*itor).second;
		if (target->isTemp())
		{
			if (target->isTarget())
			{
				Target *removedTarget = 
					ScorchedClient::instance()->getTargetContainer().removeTarget(playerId);
				delete removedTarget;
			}
			else
			{
				Tank *removedTank = 
					ScorchedClient::instance()->getTankContainer().removeTank(playerId);
				delete removedTank;
			}
		}
	}
}
