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
#include <simactions/TankAddSimAction.h>
#include <simactions/TankChangeSimAction.h>
#include <coms/ComsAddPlayerMessage.h>
#include <coms/ComsMessageSender.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <common/Logger.h>
#include <common/StatsLogger.h>
#include <common/Defines.h>
#include <server/ServerSimulator.h>

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
	do 
	{
		++id;
		if (id >= TargetID::MAX_TANK_ID) id = TargetID::START_TRANSIENT_TANK_ID;
	}
	while (context.getTargetContainer().getTargetById(id));

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
			if (context.getOptionsGame().getTeamBallance() ==
				OptionsGame::TeamBallanceBotsVs)
			{
				team = 1;
			}
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
		unsigned int playerId = getNextTankId(uniqueId, context.getContext());
		TankAvatar tankAvatar;
		tankAvatar.loadFromFile(S3D::getDataFile("data/avatars/computer.png"));

		// Tell the clients to create this tank
		ComsAddPlayerMessage addPlayerMessage(
			playerId,
			newname,
			color,
			tankModel->getName(),
			0,
			team,
			""); 
		addPlayerMessage.setPlayerIconName("data/avatars/computer.png");
		addPlayerMessage.getPlayerIcon().addDataToBuffer(
			tankAvatar.getFile().getBuffer(),
			tankAvatar.getFile().getBufferUsed());

		TankAddSimAction *simAction = new TankAddSimAction(addPlayerMessage,
			uniqueId, "", "AI", 0, aiName);
		context.getServerSimulator().addSimulatorAction(simAction);
		TankChangeSimAction *changeAction = new TankChangeSimAction(addPlayerMessage);
		context.getServerSimulator().addSimulatorAction(changeAction);
	}
}
