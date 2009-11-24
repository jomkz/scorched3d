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

#include <GLW/GLWWindowManager.h>
#include <GLW/GLWWindowSkinManager.h>
#include <GLW/GLWSelector.h>
#include <common/Keyboard.h>
#include <common/OptionsScorched.h>
#include <client/ClientParams.h>
#include <client/ClientState.h>
#include <client/ClientWindowSetup.h>
#include <client/ScorchedClient.h>
#include <dialogs/AnimatedBackdropDialog.h>
#include <dialogs/AutoDefenseDialog.h>
#include <dialogs/MainMenuDialog.h>
#include <dialogs/CameraDialog.h>
#include <dialogs/PlayerDialog.h>
#include <dialogs/QuitDialog.h>
#include <dialogs/LogDialog.h>
#include <dialogs/ConnectDialog.h>
#include <dialogs/AuthDialog.h>
#include <dialogs/BackdropDialog.h>
#include <dialogs/ProgressDialog.h>
#include <dialogs/GiftMoneyDialog.h>
#include <dialogs/BuyAccessoryDialog.h>
#include <dialogs/RulesDialog.h>
#include <dialogs/ScoreDialog.h>
#include <dialogs/KibitzingDialog.h>
#include <dialogs/InventoryDialog.h>
#include <dialogs/ResignDialog.h>
#include <dialogs/SkipDialog.h>
#include <dialogs/SkipAllDialog.h>
#include <dialogs/HUDDialog.h>
#include <dialogs/TipDialog.h>
#include <dialogs/SoundDialog.h>
#include <dialogs/HelpButtonDialog.h>
#include <dialogs/TutorialDialog.h>
#include <dialogs/MessageDialog.h>
#include <dialogs/StartDialog.h>
#include <dialogs/ModSelectDialog.h>
#include <dialogs/ModSubSelectDialog.h>
#include <dialogs/SaveSelectDialog.h>
#include <dialogs/NetworkSelectDialog.h>
#include <dialogs/MsgBoxDialog.h>
#include <dialogs/SettingsSelectDialog.h>
#include <dialogs/SettingsSubSelectDialog.h>
#include <dialogs/AdminCheckDialog.h>
#include <dialogs/AdminAuthDialog.h>
#include <dialogs/AdminDialog.h>

void ClientWindowSetup::addStateWindows(GLWWindowSkinManager *skinManager,
	unsigned int state, const char *windowState)
{
	std::list<GLWWindowSkin *> allStateWindows = 
		skinManager->getStateWindows(windowState);
	std::list<GLWWindowSkin *>::iterator itor;
	for (itor = allStateWindows.begin();
		itor != allStateWindows.end();
		itor++)
	{
		GLWWindowSkin *window = *itor;
		KeyboardKey *key = 0;
		if (window->getKey()[0]) key = 
			Keyboard::instance()->getKey(window->getKey());
		GLWWindowManager::instance()->addWindow(state, 
			window, key, window->getVisible());
	}
}

void ClientWindowSetup::addCommonComponents(GLWWindowSkinManager *skinManager, 
	unsigned state)
{
	addStateWindows(skinManager, state, "all");

	KEYBOARDKEY("HUD_ITEMS", hudItemsKey);
	GLWWindowManager::instance()->addWindow(state, 
 		HUDDialog::instance(), hudItemsKey, false);

	KEYBOARDKEY("TIP_ITEMS", tipItemsKey);
	GLWWindowManager::instance()->addWindow(state, 
 		TipDialog::instance(), tipItemsKey, false);

	KEYBOARDKEY("SHOW_SOUND_DIALOG", showSoundKey);
	GLWWindowManager::instance()->addWindow(state, 
 		SoundDialog::instance(), showSoundKey, false);

	KEYBOARDKEY("SHOW_INVENTORY_DIALOG", showInvKey);
	GLWWindowManager::instance()->addWindow(state, 
 		InventoryDialog::instance(), showInvKey, false);

	KEYBOARDKEY("SHOW_QUIT_DIALOG", quitKey);
	GLWWindowManager::instance()->addWindow(state, 
 		QuitDialog::instance(), quitKey, false);

	KEYBOARDKEY("SHOW_KIBITZ_DIALOG", kibitzKey);
	GLWWindowManager::instance()->addWindow(state, 
		new KibitzingDialog, kibitzKey, false);

	if (!ClientParams::instance()->getConnectedToServer())
	{
		KEYBOARDKEY("SHOW_KILL_DIALOG", killKey);
		GLWWindowManager::instance()->addWindow(state, 
 			QuitDialog::instance(), killKey, false);
	}

	KEYBOARDKEY("SHOW_CAMERA_DIALOG", cameraKey);
	GLWWindowManager::instance()->addWindow(state, 
		CameraDialog::instance(), cameraKey, false);

	if (state != ClientState::StateScore)
	{
		KEYBOARDKEY("SHOW_SCORE_DIALOG", scoreKey);
		GLWWindowManager::instance()->addWindow(state, 
			ScoreDialog::instance(), scoreKey, false);
	}
	GLWWindowManager::instance()->addWindow(state,
		MessageDialog::instance(), 0, true);

	GLWWindowManager::instance()->addWindow(state, 
		MainMenuDialog::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(state,
		GLWSelector::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(state,
		MsgBoxDialog::instance(), 0, false);

	KEYBOARDKEY("SHOW_TEAM_DIALOG", teamKey);
	GLWWindowManager::instance()->addWindow(state,
		PlayerDialog::instance(), teamKey, false);

	KEYBOARDKEY("SHOW_RULES_DIALOG", rulesKey);
	GLWWindowManager::instance()->addWindow(state, 
		RulesDialog::instance(), rulesKey, true);

	GLWWindowManager::instance()->addWindow(state, 
		AdminCheckDialog::instance(), 0, false);
	GLWWindowManager::instance()->addWindow(state, 
		AdminDialog::instance(), 0, false);
	GLWWindowManager::instance()->addWindow(state, 
		AdminAuthDialog::instance(), 0, false);

	if (ScorchedClient::instance()->getOptionsGame().getTutorial()[0])
	{
		GLWWindowManager::instance()->addWindow(state, 
 			TutorialDialog::instance(), 0, true);
	}
}

// This is called before any mod has been loaded
void ClientWindowSetup::setupStartWindows(GLWWindowSkinManager *skinManager)
{
	KEYBOARDKEY("SHOW_QUIT_DIALOG", quitKey);
	KEYBOARDKEY("SHOW_PLAYER_DIALOG", playerKey);
	KEYBOARDKEY("SHOW_AIM_DIALOG", aimKey);
	KEYBOARDKEY("SHOW_WEAPON_DIALOG", weaponKey);
	KEYBOARDKEY("SHOW_RESIGN_DIALOG", resignKey);
	KEYBOARDKEY("SHOW_SKIP_DIALOG", skipKey);
	KEYBOARDKEY("SHOW_SOUND_DIALOG", showSoundKey);

	// StateOptions
	GLWWindowManager::instance()->addWindow(ClientState::StateOptions, 
		AnimatedBackdropDialog::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(ClientState::StateOptions, 
		QuitDialog::instance(), quitKey, false);
	GLWWindowManager::instance()->addWindow(ClientState::StateOptions, 
 		SoundDialog::instance(), showSoundKey, false);
	GLWWindowManager::instance()->addWindow(ClientState::StateOptions, 
		StartDialog::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(ClientState::StateOptions,
		ModSelectDialog::instance(), 0, false);
	GLWWindowManager::instance()->addWindow(ClientState::StateOptions,
		ModSubSelectDialog::instance(), 0, false);
	GLWWindowManager::instance()->addWindow(ClientState::StateOptions,
		SaveSelectDialog::instance(), 0, false);
	GLWWindowManager::instance()->addWindow(ClientState::StateOptions,
		NetworkSelectDialog::instance(), 0, false);
	GLWWindowManager::instance()->addWindow(ClientState::StateOptions,
		SettingsSelectDialog::instance(), 0, false);
	GLWWindowManager::instance()->addWindow(ClientState::StateOptions,
		SettingsSubSelectDialog::instance(), 0, false);
	GLWWindowManager::instance()->addWindow(ClientState::StateOptions, 
		MainMenuDialog::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(ClientState::StateOptions, 
		GLWSelector::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(ClientState::StateOptions, 
		MsgBoxDialog::instance(), 0, false);

	// StateConnect
	GLWWindowManager::instance()->addWindow(ClientState::StateConnect, 
		BackdropDialog::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(ClientState::StateConnect, 
		QuitDialog::instance(), quitKey, false);
	GLWWindowManager::instance()->addWindow(ClientState::StateConnect, 
 		SoundDialog::instance(), showSoundKey, false);
	GLWWindowManager::instance()->addWindow(ClientState::StateConnect, 
		ProgressDialog::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(ClientState::StateConnect, 
		ConnectDialog::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(ClientState::StateConnect, 
		AuthDialog::instance(), 0, false);
	GLWWindowManager::instance()->addWindow(ClientState::StateConnect, 
		MainMenuDialog::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(ClientState::StateConnect, 
		GLWSelector::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(ClientState::StateConnect, 
		MsgBoxDialog::instance(), 0, false);

	// StateDisconnected
	GLWWindowManager::instance()->addWindow(ClientState::StateDisconnected, 
		BackdropDialog::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(ClientState::StateDisconnected, 
 		SoundDialog::instance(), showSoundKey, false);
	GLWWindowManager::instance()->addWindow(ClientState::StateDisconnected, 
		QuitDialog::instance(), quitKey, false);
	GLWWindowManager::instance()->addWindow(ClientState::StateDisconnected, 
		LogDialog::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(ClientState::StateDisconnected, 
		MainMenuDialog::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(ClientState::StateDisconnected, 
		GLWSelector::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(ClientState::StateDisconnected, 
		MsgBoxDialog::instance(), 0, false);

	// StateLoadFiles
	GLWWindowManager::instance()->addWindow(ClientState::StateLoadFiles,
		BackdropDialog::instance(), 0, true);
	addStateWindows(skinManager, ClientState::StateLoadFiles, "start");
	GLWWindowManager::instance()->addWindow(ClientState::StateLoadFiles,
		ProgressDialog::instance(), 0, true);
}

// This is called after the correct mod has been loaded
void ClientWindowSetup::setupGameWindows(GLWWindowSkinManager *skinManager)
{
	KEYBOARDKEY("SHOW_QUIT_DIALOG", quitKey);
	KEYBOARDKEY("SHOW_PLAYER_DIALOG", playerKey);
	KEYBOARDKEY("SHOW_AIM_DIALOG", aimKey);
	KEYBOARDKEY("SHOW_WEAPON_DIALOG", weaponKey);
	KEYBOARDKEY("SHOW_RESIGN_DIALOG", resignKey);
	KEYBOARDKEY("SHOW_SKIP_DIALOG", skipKey);
	KEYBOARDKEY("SHOW_RULES_DIALOG", rulesKey);
	KEYBOARDKEY("SHOW_SOUND_DIALOG", showSoundKey);

	// StateWaitNoLandscape
	GLWWindowManager::instance()->addWindow(ClientState::StateWaitNoLandscape,
		BackdropDialog::instance(), 0, true);
	addStateWindows(skinManager, ClientState::StateWaitNoLandscape, "start");
	GLWWindowManager::instance()->addWindow(ClientState::StateWaitNoLandscape, 
		ScoreDialog::instance2(), 0, true);
	GLWWindowManager::instance()->addWindow(ClientState::StateWaitNoLandscape, 
		RulesDialog::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(ClientState::StateWaitNoLandscape,
		PlayerDialog::instance(), 0, false);
	GLWWindowManager::instance()->addWindow(ClientState::StateWaitNoLandscape, 
		MainMenuDialog::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(ClientState::StateWaitNoLandscape, 
		GLWSelector::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(ClientState::StateWaitNoLandscape, 
		MsgBoxDialog::instance(), 0, false);

	// StateLoadLevel
	GLWWindowManager::instance()->addWindow(ClientState::StateLoadLevel,
		BackdropDialog::instance(), 0, true);
	addStateWindows(skinManager, ClientState::StateLoadLevel, "start");
	GLWWindowManager::instance()->addWindow(ClientState::StateLoadLevel,
		ProgressDialog::instance(), 0, true);

	// StateWait
	addCommonComponents(skinManager, ClientState::StateWait);

	// StateBuyWeapons
	addCommonComponents(skinManager, ClientState::StateBuyWeapons);
	GLWWindowManager::instance()->addWindow(ClientState::StateBuyWeapons, 
		BuyAccessoryDialog::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(ClientState::StateBuyWeapons, 
		GiftMoneyDialog::instance(), 0, false);

	// StateAutoDefense
	addCommonComponents(skinManager, ClientState::StateAutoDefense);
	GLWWindowManager::instance()->addWindow(ClientState::StateAutoDefense, 
		new AutoDefenseDialog, 0, false);

	// StatePlaying
	addStateWindows(skinManager, ClientState::StatePlaying, "playing");
	GLWWindowManager::instance()->addWindow(ClientState::StatePlaying, 
			ResignDialog::instance(), resignKey, false);
	GLWWindowManager::instance()->addWindow(ClientState::StatePlaying, 
			SkipDialog::instance(), skipKey, false);
	GLWWindowManager::instance()->addWindow(ClientState::StatePlaying, 
			SkipAllDialog::instance(), 0, false);
	addCommonComponents(skinManager, ClientState::StatePlaying);

	// StateScore
	addCommonComponents(skinManager, ClientState::StateScore);
	GLWWindowManager::instance()->addWindow(ClientState::StateScore,
		ScoreDialog::instance2(), 0, true);
}
