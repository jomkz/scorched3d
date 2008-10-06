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

#include <dialogs/QuitDialog.h>
#include <dialogs/SaveDialog.h>
#include <GLW/GLWWindowManager.h>
#include <client/ClientParams.h>
#include <server/ServerCommon.h>
#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <engine/MainLoop.h>

QuitDialog *QuitDialog::instance_ = 0;

QuitDialog *QuitDialog::instance()
{
	if (!instance_)
	{
		instance_ = new QuitDialog;
	}
	return instance_;
}

QuitDialog::QuitDialog() : 
	GLWWindow("Quit", 210.0f, 150.0f, 0,
		"Allows the player to quit the game.")
{
	killButton_ = (GLWTextButton *) 
		addWidget(new GLWTextButton(LANG_RESOURCE("MASS_TANK_KILL", "Mass Tank Kill"), 10, 115, 190, this, 
		GLWButton::ButtonFlagCenterX));
	killButton_->setToolTip(new ToolTip(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("MASS_TANK_KILL", "Mass tank kill"),
		LANG_RESOURCE("MADD_TANK_KILL_TOOLTIP", "Kills all the tanks and starts the next\n"
		"round.  Only available in single player\n"
		"games.")));

	saveButton_ = (GLWTextButton *) 
		addWidget(new GLWTextButton(LANG_RESOURCE("SAVE_GAME", "Save Game"), 10, 80, 190, this,
		GLWButton::ButtonFlagCenterX));
	saveButton_->setToolTip(new ToolTip(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("SAVE_GAME", "Save Game"),
		LANG_RESOURCE("SAVE_GAME_TOOLTIP", "Saves the games.\n"
		"Only available in single player games.")));

	quitButton_ = (GLWTextButton *) 
		addWidget(new GLWTextButton(LANG_RESOURCE("QUIT_GAME", "Quit Game"), 10, 45, 190, this, 
		GLWButton::ButtonFlagOk | GLWButton::ButtonFlagCenterX));
	quitButton_->setToolTip(new ToolTip(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("QUIT_GAME", "Quit Game"),
		LANG_RESOURCE("QUIT_GAME_TOOLTIP", "Quits Scorched3D")));

	okButton_ = (GLWTextButton *) 
		addWidget(new GLWTextButton(LANG_RESOURCE("CANCEL", "Cancel"), 95, 10, 105, this, 
		GLWButton::ButtonFlagCancel | GLWButton::ButtonFlagCenterX));
	okButton_->setToolTip(new ToolTip(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("CANCEL", "Cancel"),
		LANG_RESOURCE("CANCEL_TOOLTIP", "Return to the game.")));
}

QuitDialog::~QuitDialog()
{

}

void QuitDialog::display()
{
	GLWWindow::display();

	unsigned int state = ScorchedClient::instance()->getGameState().getState();
	bool disable = (ClientParams::instance()->getConnectedToServer() ||
		state == ClientState::StateOptions ||
		state == ClientState::StateConnect ||
		state == ClientState::StateGetPlayers ||
		state == ClientState::StateLoadPlayers);
	saveButton_->setEnabled(!disable);
	killButton_->setEnabled(!disable);
}

void QuitDialog::buttonDown(unsigned int id)
{
	if (id == okButton_->getId())
	{
		GLWWindowManager::instance()->hideWindow(id_);
	}
	else if (id == saveButton_->getId())
	{
		GLWWindowManager::instance()->showWindow(
			SaveDialog::instance()->getId());
		GLWWindowManager::instance()->hideWindow(id_);
	}
	else if (id == killButton_->getId())
	{
		ServerCommon::killAll();
		GLWWindowManager::instance()->hideWindow(id_);
	}
	else if (id == quitButton_->getId())
	{
		ScorchedClient::instance()->getMainLoop().exitLoop();
	}
}
