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

#include <dialogs/GiftMoneyDialog.h>
#include <dialogs/BuyAccessoryDialog.h>
#include <graph/OptionsDisplay.h>
#include <client/ScorchedClient.h>
#include <tank/TankContainer.h>
#include <tank/TankScore.h>
#include <tank/TankState.h>
#include <GLW/GLWTextButton.h>
#include <GLW/GLWWindowManager.h>
#include <coms/ComsGiftMoneyMessage.h>
#include <coms/ComsMessageSender.h>

GiftMoneyDialog *GiftMoneyDialog::instance_ = 0;

GiftMoneyDialog *GiftMoneyDialog::instance()
{
	if (!instance_)
	{
		instance_ = new GiftMoneyDialog;
	}
	return instance_;
}

GiftMoneyDialog::GiftMoneyDialog() :
	GLWWindow("", 10.0f, 10.0f, 300.0f, 70.0f, eSmallTitle,
		"Send money to other team players")
{
	needCentered_ = true;

	GLWPanel *mainPanel = new GLWPanel(0.0f, 0.0f, 0.0f, 0.0f, false, false);
	mainPanel->addWidget(
		new GLWLabel(0.0f, 0.0f, LANG_RESOURCE("GIFT", "Gift")), 
		0, SpaceRight, 10.0f);
	money_ = (GLWDropDownText *) mainPanel->addWidget(
		new GLWDropDownText(0.0f, 0.0f, 150.0f), 
		0, SpaceRight, 10.0f);
	mainPanel->addWidget(
		new GLWLabel(0.0f, 0.0f, LANG_RESOURCE("TO", "to")), 
		0, SpaceRight, 10.0f);
	players_ = (GLWDropDownText *) mainPanel->addWidget(
		new GLWDropDownText(0.0f, 0.0f, 150.0f));
	mainPanel->setLayout(GLWPanel::LayoutHorizontal);
	addWidget(mainPanel, 0, SpaceLeft | SpaceRight | SpaceTop, 10.0f);

	GLWPanel *buttonPanel = new GLWPanel(0.0f, 0.0f, 0.0f, 0.0f, false, false);
	GLWButton *cancelButton = new GLWTextButton(LANG_RESOURCE("CANCEL", "Cancel"), 95, 10, 105, this, 
		GLWButton::ButtonFlagCancel | GLWButton::ButtonFlagCenterX);
	cancelId_ = cancelButton->getId();
	buttonPanel->addWidget(cancelButton, 0, SpaceRight, 10.0f);
	GLWButton *okButton = new GLWTextButton(LANG_RESOURCE("OK", "Ok"), 235, 10, 55, this, 
		GLWButton::ButtonFlagOk | GLWButton::ButtonFlagCenterX);
	okId_ = okButton->getId();
	buttonPanel->addWidget(okButton);
	buttonPanel->setLayout(GLWPanel::LayoutHorizontal);
	addWidget(buttonPanel, 0, SpaceAll, 10.0f);

	setLayout(GLWPanel::LayoutVerticle);
	layout();
}

GiftMoneyDialog::~GiftMoneyDialog()
{
}

void GiftMoneyDialog::display()
{
	GLWWindow::display();

	players_->clear();
	money_->clear();

	// Get the current playing tank
	Tank *currentTank = 
		ScorchedClient::instance()->getTankContainer().getCurrentTank();
	if (!currentTank)
	{
		GLWWindowManager::instance()->hideWindow(getId());
		return;
	}

	// Add available amounts of money
	if (currentTank->getScore().getMoney() >= 1000)
		money_->addEntry(GLWSelectorEntry(LANG_RESOURCE("1000_DOLLARS", "$1000"), 0, false, 0, (void *) 1000));
	if (currentTank->getScore().getMoney() >= 2500)
		money_->addEntry(GLWSelectorEntry(LANG_RESOURCE("2500_DOLLARS", "$2500"), 0, false, 0, (void *) 2500));
	if (currentTank->getScore().getMoney() >= 5000)
		money_->addEntry(GLWSelectorEntry(LANG_RESOURCE("5000_DOLLARS", "$5000"), 0, false, 0, (void *) 5000));
	if (currentTank->getScore().getMoney() >= 10000)
		money_->addEntry(GLWSelectorEntry(LANG_RESOURCE("10000_DOLLARS", "$10000"), 0, false, 0, (void *) 10000));
	if (currentTank->getScore().getMoney() >= 15000)
		money_->addEntry(GLWSelectorEntry(LANG_RESOURCE("15000_DOLLARS", "$15000"), 0, false, 0, (void *) 15000));
	if (currentTank->getScore().getMoney() >= 20000)
		money_->addEntry(GLWSelectorEntry(LANG_RESOURCE("20000_DOLLARS", "$20000"), 0, false, 0, (void *) 20000));
	if (currentTank->getScore().getMoney() >= 25000)
		money_->addEntry(GLWSelectorEntry(LANG_RESOURCE("25000_DOLLARS", "$25000"), 0, false, 0, (void *) 25000));

	// Add all tanks in the same team as the current
	std::map<unsigned int, Tank *> &tanks = 
		ScorchedClient::instance()->getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		Tank *tank = itor->second;
		if (tank->getTeam() == currentTank->getTeam() &&
			!tank->getState().getSpectator() &&
			tank != currentTank &&
			(tank->getState().getState() == TankState::sDead ||
			tank->getState().getState() == TankState::sNormal))
		{
			players_->addEntry(GLWSelectorEntry(
				tank->getTargetName(), 0, false, 0, (void *) tank->getPlayerId()));
		}
	}
}

void GiftMoneyDialog::buttonDown(unsigned int id)
{
	if (id == okId_)
	{
		if (money_->getCurrentEntry() &&
			players_->getCurrentEntry())
		{
			int money = (int) 
				long(money_->getCurrentEntry()->getUserData());
			unsigned int playerId = (unsigned int)
				long(players_->getCurrentEntry()->getUserData());

			Tank *currentTank = 
				ScorchedClient::instance()->getTankContainer().getCurrentTank();
			if (currentTank)
			{
				ComsGiftMoneyMessage message(
					currentTank->getPlayerId(), playerId, money);
				ComsMessageSender::sendToServer(message);
			}
		}
	}
	GLWWindowManager::instance()->hideWindow(getId());
	GLWWindowManager::instance()->showWindow(
		BuyAccessoryDialog::instance()->getId());
}
