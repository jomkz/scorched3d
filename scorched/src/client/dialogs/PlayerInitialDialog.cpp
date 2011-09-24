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

#include <dialogs/PlayerInitialDialog.h>
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

PlayerInitialDialog *PlayerInitialDialog::instance()
{
	static PlayerInitialDialog instance_;
	return &instance_;
}

PlayerInitialDialog::PlayerInitialDialog() :
	PlayerDialog("Player", eSmallTitle | eHideName, ""),
	currentPlayerId_(0)
{
}

PlayerInitialDialog::~PlayerInitialDialog()
{
}

void PlayerInitialDialog::displayDialog()
{
	// Set cancel/spectate
	if (ClientParams::instance()->getConnectedToServer())
	{
		cancelButton_->setEnabled(false);
		spectateButton_->setEnabled(true);
	}
	else
	{
		cancelButton_->setEnabled(false);
		spectateButton_->setEnabled(false);
	}

	GLWWindowManager::instance()->showWindow(getId());

	currentPlayerId_ = 0;
	nextPlayer();

	if (ClientParams::instance()->getConnectAcceptDefaults())
	{
		okButton(false);
	}
}

void PlayerInitialDialog::okButton(bool spectate)
{
	// If we are connected online save this players name
	if (ClientParams::instance()->getConnectedToServer())
	{
		OptionsDisplay::instance()->getOnlineUserNameEntry().setValue(
			playerName_->getText().c_str());
		OptionsDisplay::instance()->getOnlineTankModelEntry().setValue(
			viewer_->getModelName());
		OptionsDisplay::instance()->getOnlineUserIconEntry().setValue(
			imageList_->getCurrentShortPath());
		if (ScorchedClient::instance()->getOptionsGame().getTeams() == 1)
		{
			OptionsDisplay::instance()->getOnlineColorEntry().setValue(
				colorDropDown_->getCurrentColor());
		}
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
	ComsTankChangeMessage message(currentPlayerId_,
		playerName_->getLangString(),
		colorDropDown_->getCurrentColor(),
		tankTypeDropDown_->getCurrentDataText(),
		model->getName(),
		ScorchedClient::instance()->getTargetContainer().getCurrentDestinationId(),
		currentTeam,
		playerType,
		spectate);
	// Add avatar (if not one)
	Tank *tank = ScorchedClient::instance()->getTargetContainer().
		getTankById(currentPlayerId_);
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

	nextPlayer();
}

void PlayerInitialDialog::cancelButton()
{

}

void PlayerInitialDialog::initializeDefaults(Tank *tank)
{
	if (ScorchedClient::instance()->getOptionsGame().getTeams() != 1)
	{
		// Auto select the team with the least players
		unsigned int newTeam = 
			ScorchedClient::instance()->getOptionsTransient().getLeastUsedTeam(
			ScorchedClient::instance()->getTargetContainer());
		tank->setTeam(newTeam);
	}

	if (ClientParams::instance()->getConnectedToServer())
	{
		// If we are connected online then use the online name
		tank->setName(LANG_STRING(OptionsDisplay::instance()->getOnlineUserName()));
		tank->getAvatar().loadFromFile(S3D::getDataFile(S3D::formatStringBuffer("data/avatars/%s", 
			OptionsDisplay::instance()->getOnlineUserIcon())));
		tank->getModelContainer().setTankModelName(OptionsDisplay::instance()->getOnlineTankModel());
		tank->setColor(OptionsDisplay::instance()->getOnlineColor());
	}
}

void PlayerInitialDialog::nextPlayer()
{
	getNextPlayer();
	if (currentPlayerId_ == 0) 
	{
		GLWWindowManager::instance()->hideWindow(getId());
	}
}

void PlayerInitialDialog::getNextPlayer()
{
	unsigned int current = currentPlayerId_;
	currentPlayerId_ = 0;

	std::map<unsigned int, Tank *> &tanks = 
		ScorchedClient::instance()->getTargetContainer().getTanks();
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		++itor)
	{
		Tank *tank = (*itor).second;
		if ((tank->getDestinationId() == 
			ScorchedClient::instance()->getTargetContainer().getCurrentDestinationId()) &&
			(tank->getPlayerId() != TargetID::SPEC_TANK_ID))
		{
			if (current == 0)
			{
				currentPlayerId_ = tank->getPlayerId();
				initializeDefaults(tank);
				initializeFromTank(tank);
				break;
			}
			else if (tank->getPlayerId() == current) 
			{
				current = 0;
			}
		}
	}
}
