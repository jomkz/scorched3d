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

#include <simactions/TankChangeSimAction.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <common/StatsLogger.h>
#include <common/Logger.h>
#include <common/ChannelManager.h>
#include <tank/TankAvatar.h>
#include <tank/TankState.h>
#include <tank/TankContainer.h>
#include <tank/TankColorGenerator.h>
#include <tank/TankModelContainer.h>
#include <tank/TankModelStore.h>

REGISTER_CLASS_SOURCE(TankChangeSimAction);

TankChangeSimAction::TankChangeSimAction()
{
}

TankChangeSimAction::TankChangeSimAction(ComsAddPlayerMessage &message) :
	message_(message)
{

}

TankChangeSimAction::~TankChangeSimAction()
{

}

bool TankChangeSimAction::invokeAction(ScorchedContext &context)
{
	// Validate player
	unsigned int playerId = message_.getPlayerId();
	Tank *tank = context.getTankContainer().getTankById(playerId);
	if (!tank || tank->getState().getState() == TankState::sNormal)
	{
		return true;
	}

	// Make sure no-one has the same name
	LangString postFix = LANG_STRING("");
	LangString sentname = message_.getPlayerName();
	for (int p=1;;p++)
	{
		bool found = false;
		std::map<unsigned int, Tank *>::iterator mainitor;
		std::map<unsigned int, Tank *> tanks = 
			context.getTankContainer().getAllTanks();
		for (mainitor = tanks.begin();
			mainitor != tanks.end();
			mainitor++)
		{
			Tank *currentTank = (*mainitor).second;
			if (currentTank->getTargetName() == (sentname + postFix) &&
				tank != currentTank) 
			{
				found = true;
				break;
			}
		}

		if (!found) break;
		postFix = LANG_STRING(S3D::formatStringBuffer("(%i)", p));
	}
	sentname += postFix;

	// Setup the new player
	LangString oldName = tank->getTargetName();
	tank->setName(sentname);

	// Player has set a new color
	if (tank->getTeam() == 0 &&
		message_.getPlayerColor() != tank->getColor())
	{
		// Check the color is not already in use
		std::map<unsigned int, Tank *> &tanks = 
			context.getTankContainer().getPlayingTanks();
		if (TankColorGenerator::instance()->colorAvailable(
			message_.getPlayerColor(), tanks, tank))
		{
			// Set this color
			tank->setColor(message_.getPlayerColor());
		}
	}

	// Set avatar
	if (message_.getPlayerIconName()[0]) 
	{
		if (message_.getPlayerIcon().getBufferUsed() <=
			(unsigned) context.getOptionsGame().getMaxAvatarSize())
		{
			tank->getAvatar().setFromBuffer(
				message_.getPlayerIconName(),
				message_.getPlayerIcon());
		}
	}

	// Tell the logger about a new tank
	StatsLogger::instance()->tankJoined(tank);

	// Choose a team (if applicable)
	if (context.getOptionsGame().getTeams() > 1)
	{
		if (message_.getPlayerTeam() > 0 && message_.getPlayerTeam() <=
			(unsigned int) context.getOptionsGame().getTeams())
		{
			tank->setTeam(message_.getPlayerTeam());
		}
		else
		{
			tank->setTeam(context.getOptionsTransient().getLeastUsedTeam(
				context.getTankContainer()));
		}
	}

	// Make sure the model is available and for the correct team
	// Do this AFTER the team has been set
	TankModel *tankModel = 
		context.getTankModels().
			getModelByName(message_.getModelName(), 
				tank->getTeam(),
				tank->isTemp());
	tank->getModelContainer().setTankModelName(
		tankModel->getName(), message_.getModelName(), tankModel->getTypeName());

	// Show some infor
	if (!tank->getState().getNotSpectator())
	{
#ifdef S3D_SERVER
	Logger::log(S3D::formatStringBuffer(
		"Player playing dest=\"%i\" id=\"%i\" \"%s\"",
		tank->getDestinationId(), tank->getPlayerId(),
		tank->getCStrName().c_str()));
#endif // #ifdef S3D_SERVER

		ChannelManager::showText(
			context,
			ChannelText("info",
				"PLAYER_PLAYING",
				"Player playing [p:{0}]",
				sentname));
	} 
	else if (oldName != tank->getTargetName())
	{
		ChannelManager::showText(
			context,
			ChannelText("info",
				"PLAYER_NAME_CHANGE",
				"Player \"{0}\" changed name to \"[p:{1}]\"",
				oldName, tank->getTargetName()));
	}

	// Set state
	tank->getState().setState(TankState::sDead);
	tank->getState().setNotSpectator(true);

	return true;
}

bool TankChangeSimAction::writeMessage(NetBuffer &buffer)
{
	message_.writeMessage(buffer);
	return true;
}

bool TankChangeSimAction::readMessage(NetBufferReader &reader)
{
	if (!message_.readMessage(reader)) return false;
	return true;
}
