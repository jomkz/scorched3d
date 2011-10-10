////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011
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
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
////////////////////////////////////////////////////////////////////////////////

#include <dialogs/PlayerInGameDialog.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankAvatar.h>
#include <tank/TankModelContainer.h>
#include <tank/TankModelStore.h>
#include <client/ScorchedClient.h>
#include <client/ClientParams.h>
#include <graph/OptionsDisplay.h>
#include <common/OptionsGame.h>
#include <common/OptionsTransient.h>
#include <common/Logger.h>
#include <coms/ComsTankChangeMessage.h>
#include <coms/ComsMessageSender.h>
#include <client/ClientChannelManager.h>
#include <GLW/GLWWindowManager.h>

PlayerInGameDialog *PlayerInGameDialog::instance()
{
	static PlayerInGameDialog instance_;
	return &instance_;
}

PlayerInGameDialog::PlayerInGameDialog() :
	PlayerDialog("Player", eSmallTitle, 
		"Allows the player to make changes to their\n"
		"name, their tank and to change teams.")
{
}

PlayerInGameDialog::~PlayerInGameDialog()
{
}

void PlayerInGameDialog::displayDialog()
{
	GLWWindowManager::instance()->showWindow(getId());
}

void PlayerInGameDialog::display()
{
	// Set cancel/spectate
	if (ClientParams::instance()->getConnectedToServer())
	{
		cancelButton_->setEnabled(true);
		spectateButton_->setEnabled(true);
	}
	else
	{
		cancelButton_->setEnabled(true);
		spectateButton_->setEnabled(false);
	}

	Tank *tank = getCurrentPlayer();
	if (!tank) 
	{
		GLWWindowManager::instance()->hideWindow(getId());
		return;
	}

	initializeFromTank(tank);
}

void PlayerInGameDialog::okButton(bool spectate)
{
	Tank *tank = getCurrentPlayer();
	if (!tank) 
	{
		GLWWindowManager::instance()->hideWindow(getId());
		return;
	}

	int currentTeam = ((ScorchedClient::instance()->getOptionsGame().getTeams() > 1)?
		teamDropDown_->getCurrentPosition() + 1:0);

	// Check the current model exists or get a random one
	TankModel *model = 
		ScorchedClient::instance()->getTankModels().
			getModelByName(viewer_->getModelName());
	if (!model)
	{
		model = ScorchedClient::instance()->getTankModels().
			getRandomModel(currentTeam, false, tankTypeDropDown_->getCurrentDataText());
	}

	// Get the player type
	const char *playerType = aiTypeDropDown_->getCurrentDataText();

	// Add this player
	ComsTankChangeMessage message(tank->getPlayerId(),
		playerName_->getLangString(),
		colorDropDown_->getCurrentColor(),
		tankTypeDropDown_->getCurrentDataText(),
		model->getName(),
		ScorchedClient::instance()->getTargetContainer().getCurrentDestinationId(),
		currentTeam,
		playerType,
		spectate);
	// Add avatar (if not one)
	if (tank && 
		strcmp(tank->getAvatar().getName(), imageList_->getCurrentShortPath()) != 0)
	{
		if (tank->getAvatar().loadFromFile(imageList_->getCurrentLongPath()))
		{
			if (tank->getAvatar().getFile().getBufferUsed() <=
				(unsigned) ScorchedClient::instance()->getOptionsGame().getMaxAvatarSize())
			{
				message.setPlayerIconName(imageList_->getCurrentShortPath());
				message.getPlayerIcon().addDataToBuffer(
					tank->getAvatar().getFile().getBuffer(),
					tank->getAvatar().getFile().getBufferUsed());
			}
			else
			{
				ChannelText text("general", 
					LANG_RESOURCE_2("AVATAR_TOO_LARGE", 
					"Warning: Avatar too large to send to server, is {0} should be < {1}",
					tank->getAvatar().getFile().getBufferUsed(),
					ScorchedClient::instance()->getOptionsGame().getMaxAvatarSize()));
				ClientChannelManager::instance()->showText(text);
				Logger::log( "Warning: Avatar too large to send to server");
			}
		}
	}
	
	ComsMessageSender::sendToServer(message);
	GLWWindowManager::instance()->hideWindow(getId());
}

void PlayerInGameDialog::cancelButton()
{
	GLWWindowManager::instance()->hideWindow(getId());
}

Tank *PlayerInGameDialog::getCurrentPlayer()
{
	Tank *result = ScorchedClient::instance()->getTargetContainer().getCurrentTank();
	if (result) return result;
	if (ClientParams::instance()->getConnectedToServer())
	{
		std::map<unsigned int, Tank *> &tanks =
			ScorchedClient::instance()->getTargetContainer().getTanks();
		std::map<unsigned int, Tank *>::iterator itor;
		for (itor = tanks.begin();
			itor != tanks.end();
			++itor)
		{
			Tank *tank = itor->second;
			if (tank->getDestinationId() == 
				ScorchedClient::instance()->getTargetContainer().getCurrentDestinationId())
			{
				return tank;
			}
		}
	}
	return 0;
}
