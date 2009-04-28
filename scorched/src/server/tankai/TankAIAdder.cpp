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

#include <set>
#include <tankai/TankAIAdder.h>
#include <tankai/TankAIStore.h>
#include <tankai/TankAIStrings.h>
#include <tank/TankColorGenerator.h>
#include <tank/TankContainer.h>
#include <tank/TankModelStore.h>
#include <tank/TankAvatar.h>
#include <tank/TankModelContainer.h>
#include <tank/TankState.h>
#include <server/ServerState.h>
#include <coms/ComsAddPlayerMessage.h>
#include <coms/ComsMessageSender.h>
#include <coms/ComsPlayerStateMessage.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <common/Logger.h>
#include <common/StatsLogger.h>
#include <common/Defines.h>

unsigned int TankAIAdder::getNextTankId(const char *uniqueId, ScorchedContext &context)
{
	// Try to use the persistent stats id
	if (uniqueId[0])
	{
		unsigned int id = StatsLogger::instance()->getStatsId(uniqueId);
		if (id != 0 &&
			!context.getTargetContainer().getTargetById(id))
		{
			DIALOG_ASSERT(id >= TargetID::MIN_TANK_ID && id <= TargetID::MAX_TANK_ID);
			return id;
		}
	}

	// Get the transient id
	static unsigned int id = TargetID::START_TRANSIENT_TANK_ID;
	while (context.getTargetContainer().getTargetById(id))
	{
		++id;
		if (id >= TargetID::MAX_TANK_ID) id = TargetID::START_TRANSIENT_TANK_ID;
	}

	DIALOG_ASSERT(id >= TargetID::START_TRANSIENT_TANK_ID && id <= TargetID::MAX_TANK_ID);
	return id;
}

unsigned int TankAIAdder::getNextTargetId(ScorchedContext &context)
{
	unsigned int targetId_ = TargetID::MIN_TARGET_TRANSIENT_ID;
	while (context.getTargetContainer().getTargetById(targetId_))
	{
		++targetId_;
		if (targetId_ >= TargetID::MAX_TARGET_ID) targetId_ = TargetID::MIN_TARGET_TRANSIENT_ID;
	}

	return targetId_;
}

void TankAIAdder::addTankAIs(ScorchedServer &context)
{
	// On the server
	// Ensure that we cannot add more ais than the server is setup for
	int maxComputerAIs = context.getOptionsGame().getNoMaxPlayers();
	for (int i=0; i<maxComputerAIs; i++)
	{
		const char *playerType = 
			context.getOptionsGame().getPlayerType(i);
		if (0 != stricmp(playerType, "Human"))
		{
			addTankAI(context, playerType);
		}
	}
}

void TankAIAdder::addTankAI(ScorchedServer &context, const char *aiName)
{
	TankAI *ai = context.getTankAIs().getAIByName(aiName);
	if (ai)
	{
		// Create our uniqueid
		char uniqueId[256];
		{
			std::set<int> usedIds;
			snprintf(uniqueId, 256, "%s - computer - %%i", aiName);
			std::map<unsigned int, Tank *> &playingTanks = 
				context.getTankContainer().getPlayingTanks();
			std::map<unsigned int, Tank *>::iterator playingItor;
			for (playingItor = playingTanks.begin();
				playingItor != playingTanks.end();
				playingItor++)
			{
				Tank *current = (*playingItor).second;
				if (current->getDestinationId() == 0)
				{
					int id = 1;
					if (sscanf(current->getUniqueId(), uniqueId, &id) == 1)
					{
						usedIds.insert(id);
					}
				}
			}

			int uniqueIdCount = 1;
			while (usedIds.find(uniqueIdCount) != usedIds.end()) uniqueIdCount++;

			snprintf(uniqueId, 256, "%s - computer - %i", aiName, uniqueIdCount);
		}

		// Chose this tanks team
		int team = 0;
		if (context.getOptionsGame().getTeams() > 1)
		{
			team = context.getOptionsTransient().getLeastUsedTeam(
				context.getTankContainer());
		}

		// For the tank ai's name
		LangString newname = 
			LANG_STRING(context.getOptionsGame().getBotNamePrefix());
		newname += LANG_STRING(TankAIStrings::instance()->getAIPlayerName(
			ScorchedServer::instance()->getContext()));

		// Form the tank ai model
		Vector color = TankColorGenerator::instance()->getNextColor(
			context.getTankContainer().getPlayingTanks());
		TankModel *tankModel = 
			context.getTankModels().getRandomModel(team, false);

		// Create the new tank
		Tank *tank = new Tank(
			context.getContext(),
			getNextTankId(uniqueId, context.getContext()),
			0,
			newname,
			color,
			tankModel->getName(),
			tankModel->getTypeName());

		tank->getAvatar().loadFromFile(S3D::getDataFile("data/avatars/computer.png"));
		tank->setUniqueId(uniqueId);
		tank->setTankAI(ai->createCopy(tank));
		tank->getState().setState(TankState::sInitializingMod);
		tank->getState().setState(TankState::sPending);
		context.getTankContainer().addTank(tank);

		if (context.getOptionsGame().getTeams() > 1)
		{
			tank->setTeam(team);
		}

		Logger::log(S3D::formatStringBuffer("Player connected dest=\"%i\" id=\"%i\" name=\"%s\" unique=[%s]",
			tank->getDestinationId(),
			tank->getPlayerId(),
			tank->getCStrName().c_str(),
			tank->getUniqueId()));

		StatsLogger::instance()->tankConnected(tank);
		StatsLogger::instance()->tankJoined(tank);

		if (true) // Raise an event
		{
			// Tell the clients to create this tank
			ComsAddPlayerMessage addPlayerMessage(
				tank->getPlayerId(),
				tank->getTargetName(),
				tank->getColor(),
				tank->getModelContainer().getTankModelName(),
				tank->getModelContainer().getTankTypeName(),
				tank->getDestinationId(),
				tank->getTeam(),
				""); 
			addPlayerMessage.setPlayerIconName(tank->getAvatar().getName());
			addPlayerMessage.getPlayerIcon().addDataToBuffer(
				tank->getAvatar().getFile().getBuffer(),
				tank->getAvatar().getFile().getBufferUsed());
			ComsMessageSender::sendToAllConnectedClients(addPlayerMessage);
		}

		if (context.getGameState().getState() == ServerState::ServerStateTooFewPlayers ||
			context.getGameState().getState() == ServerState::ServerStateStarting)
		{
			ComsPlayerStateMessage message(false, false);
			ComsMessageSender::sendToAllConnectedClients(message);
		}
	}
}
