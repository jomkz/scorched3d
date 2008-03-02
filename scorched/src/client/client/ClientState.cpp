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

#include <engine/ActionController.h>
#include <graph/FrameTimer.h>
#include <graph/ParticleEngine.h>
#include <graph/Main2DCamera.h>
#include <graph/SpeedChange.h>
#include <graph/MainCamera.h>
#include <graph/ShotCountDown.h>
#include <graph/SoftwareMouse.h>
#include <client/ClientState.h>
#include <client/ClientShotState.h>
#include <client/ClientWaitState.h>
#include <client/ClientLoadPlayersState.h>
#include <client/ClientSaveScreenState.h>
#include <client/ScorchedClient.h>
#include <sound/Sound.h>
#include <tankgraph/RenderTargets.h>
#include <tankgraph/TankKeyboardControl.h>
#include <GLW/GLWWindowManager.h>
#include <GLW/GLWToolTip.h>
#include <landscape/LandscapeStateHandler.h>
#include <landscape/LandscapeMusicManager.h>
#include <GLEXT/GLCameraFrustum.h>
#include <GLEXT/GLConsole.h>

void ClientState::addWindowManager(GameState &gameState, unsigned state)
{
	gameState.addStateKeyEntry(state, GLConsole::instance());
	gameState.addStateEntry(state, GLWWindowManager::instance());
	gameState.addStateLoop(state, Main2DCamera::instance(), 
		GLWWindowManager::instance());
	gameState.addStateMouseDownEntry(state, 
		GameState::MouseButtonLeft | GameState::MouseButtonRight |
		GameState::MouseButtonLeftDoubleClick | GameState::MouseButtonRightDoubleClick, 
		GLWWindowManager::instance());
	gameState.addStateMouseDragEntry(state, 
		GameState::MouseButtonLeft | GameState::MouseButtonRight, 
		GLWWindowManager::instance());
	gameState.addStateMouseUpEntry(state, 
		GameState::MouseButtonLeft | GameState::MouseButtonRight, 
		GLWWindowManager::instance());
	gameState.addStateKeyEntry(state, GLWWindowManager::instance());
	gameState.addStateMouseWheelEntry(state, GLWWindowManager::instance());
	gameState.addStateLoop(state, Main2DCamera::instance(),
		GLConsole::instance());
	gameState.addStateLoop(state, Main2DCamera::instance(), 
		Sound::instance()); // SOUND
	gameState.addStateLoop(state, Main2DCamera::instance(),
		LandscapeMusicManager::instance()); // MUSIC
}

void ClientState::addStandardComponents(GameState &gameState, unsigned state)
{
	gameState.addStateKeyEntry(state, SpeedChange::instance());
	gameState.addStateEntry(state, &RenderTargets::instance()->render3D);
	gameState.addStateLoop(state, 
		MainCamera::instance(), GLCameraFrustum::instance());
	gameState.addStateLoop(state, 
		MainCamera::instance(), new LandscapeStateLandHandler());
	gameState.addStateLoop(state,
		MainCamera::instance(), &RenderTargets::instance()->render3D);
	gameState.addStateLoop(state, 
		MainCamera::instance(), new LandscapeStateWaterHandler());
	gameState.addStateLoop(state, 
		MainCamera::instance(), new LandscapeStateObjectsHandler());
	gameState.addStateLoop(state, MainCamera::instance(), 
		&ScorchedClient::instance()->getActionController());
	gameState.addStateLoop(state, MainCamera::instance(), 
		&ScorchedClient::instance()->getParticleEngine());
	gameState.addStateLoop(state, 
		MainCamera::instance(), &MainCamera::instance()->precipitation_);
	gameState.addStateLoop(state, 
		Main2DCamera::instance(), &RenderTargets::instance()->render2D);
	gameState.addStateLoop(state, 
		MainCamera::instance(), FrameTimer::instance());
	gameState.addStateLoop(state, 
		Main2DCamera::instance(), SpeedChange::instance());
	addWindowManager(gameState, state);
	gameState.addStateLoop(state, Main2DCamera::instance(), 
		GLWToolTip::instance());
	gameState.addStateLoop(state, Main2DCamera::instance(), 
		SoftwareMouse::instance());
	gameState.addStateMouseDownEntry(state,
		GameState::MouseButtonLeft, MainCamera::instance());
	gameState.addStateMouseUpEntry(state,
		GameState::MouseButtonLeft, MainCamera::instance());
	gameState.addStateMouseDragEntry(state, 
		GameState::MouseButtonLeft | 
		GameState::MouseButtonRight | 
		GameState::MouseButtonMiddle, MainCamera::instance());
	gameState.addStateMouseWheelEntry(state, 
		MainCamera::instance());
	gameState.addStateKeyEntry(state, 
		MainCamera::instance());
	gameState.addStateLoop(state, 
		Main2DCamera::instance(), &MainCamera::instance()->saveScreen_);
}

void ClientState::setupGameState()
{
	GameState &gameState = ScorchedClient::instance()->getGameState();
	gameState.clear();

	// StateOptions
	addWindowManager(gameState, StateOptions);
	gameState.addStateLoop(StateOptions, 
		Main2DCamera::instance(), GLWToolTip::instance());
	gameState.addStateLoop(StateOptions, 
		Main2DCamera::instance(), SoftwareMouse::instance());
	gameState.addStateStimulus(StateOptions, 
		StimDisconnected, StateDisconnected);
	gameState.addStateStimulus(StateOptions, 
		StimConnect, StateConnect);

	// Set the start state
	gameState.setState(StateOptions);

	// StateConnect
	addWindowManager(gameState, StateConnect);
	gameState.addStateLoop(StateConnect, 
		Main2DCamera::instance(), SoftwareMouse::instance());
	gameState.addStateStimulus(StateConnect, 
		StimDisconnected, StateDisconnected);
	gameState.addStateStimulus(StateConnect, 
		StimGameStopped, StateConnect);
	gameState.addStateStimulus(StateConnect, 
		StimOptions, StateOptions);
	gameState.addStateStimulus(StateConnect, 
		StimGetPlayers, StateGetPlayers);
	gameState.addStateStimulus(StateConnect, 
		StimLoadPlayers, StateLoadPlayers);
	gameState.addStateStimulus(StateConnect, 
		StimLoadFiles, StateLoadFiles);

	// StateGetPlayers
	addWindowManager(gameState, StateGetPlayers);
	gameState.addStateLoop(StateGetPlayers, 
		Main2DCamera::instance(), GLWToolTip::instance());
	gameState.addStateLoop(StateGetPlayers, 
		Main2DCamera::instance(), SoftwareMouse::instance());
	gameState.addStateStimulus(StateGetPlayers, 
		StimGameStopped, StateGetPlayers);
	gameState.addStateStimulus(StateGetPlayers, 
		StimDisconnected, StateDisconnected);
	gameState.addStateStimulus(StateGetPlayers, 
		StimWait, StateWait);

	// StateLoadFiles
	addWindowManager(gameState, StateLoadFiles);
	gameState.addStateLoop(StateLoadFiles, 
		Main2DCamera::instance(), GLWToolTip::instance());
	gameState.addStateLoop(StateLoadFiles, 
		Main2DCamera::instance(), SoftwareMouse::instance());
	gameState.addStateStimulus(StateLoadFiles, 
		StimGetPlayers, StateGetPlayers);
	gameState.addStateStimulus(StateLoadFiles, 
		StimGameStopped, StateConnect);
	gameState.addStateStimulus(StateLoadFiles, 
		StimDisconnected, StateDisconnected);
	gameState.addStateStimulus(StateLoadFiles, 
		StimWait, StateWait);

	// StateLoadPlayers (Single Player Only)
	addWindowManager(gameState, StateLoadPlayers);
	gameState.addStateEntry(StateLoadPlayers,
		ClientLoadPlayersState::instance());
	gameState.addStateStimulus(StateLoadPlayers,
		StimWait, StateWait);

	// StateDisconnected
	addWindowManager(gameState, StateDisconnected);
	gameState.addStateLoop(StateDisconnected, 
		Main2DCamera::instance(), SoftwareMouse::instance());
	gameState.addStateStimulus(StateDisconnected, 
		StimDisconnected, StateDisconnected);
	gameState.addStateStimulus(StateDisconnected, 
		StimGameStopped, StateDisconnected);
	gameState.addStateStimulus(StateDisconnected, 
		StimGetPlayers, StateDisconnected);
	gameState.addStateStimulus(StateDisconnected, 
		StimWait, StateDisconnected);

	// StateWait
	addStandardComponents(gameState, StateWait);
	gameState.addStateLoop(StateWait,
		Main2DCamera::instance(), ShotCountDown::instance());
	gameState.addStateEntry(StateWait, 
		TankKeyboardControl::instance());
	gameState.addStateEntry(StateWait,
		ClientWaitState::instance());
	gameState.addStateStimulus(StateWait, 
		StimGameStopped, StateGetPlayers);
	gameState.addStateStimulus(StateWait, 
		StimDisconnected, StateDisconnected);
	gameState.addStateStimulus(StateWait, 
		StimAutoDefense, StateWait);
	gameState.addStateStimulus(StateWait, 
		StimBuyWeapons, StateBuyWeapons);
	gameState.addStateStimulus(StateWait, 
		StimPlaying, StatePlaying);
	gameState.addStateStimulus(StateWait, 
		StimShot, StateShot);
	gameState.addStateStimulus(StateWait, 
		StimWait, StateWait);
	gameState.addStateStimulus(StateWait, 
		StimScore, StateScore);
	gameState.addStateStimulus(StateWait, 
		StimGetPlayers, StateGetPlayers);

	// StateBuyWeapons
	addStandardComponents(gameState, StateBuyWeapons);
	gameState.addStateLoop(StateBuyWeapons,
		Main2DCamera::instance(), ShotCountDown::instance());
	gameState.addStateStimulus(StateBuyWeapons, 
		StimDisconnected, StateDisconnected);
	gameState.addStateStimulus(StateBuyWeapons, 
		StimGameStopped, StateGetPlayers);
	gameState.addStateStimulus(StateBuyWeapons, 
		StimAutoDefense, StateAutoDefense);
	gameState.addStateStimulus(StateBuyWeapons, 
		StimWait, StateWait);

	// StateAutoDefense
	addStandardComponents(gameState, StateAutoDefense);
	gameState.addStateLoop(StateAutoDefense,
		Main2DCamera::instance(), ShotCountDown::instance());
	gameState.addStateStimulus(StateAutoDefense, 
		StimDisconnected, StateDisconnected);
	gameState.addStateStimulus(StateAutoDefense, 
		StimGameStopped, StateGetPlayers);
	gameState.addStateStimulus(StateAutoDefense, 
		StimWait, StateWait);

	// StatePlaying
	addStandardComponents(gameState, StatePlaying);
	gameState.addStateKeyEntry(StatePlaying, 
		TankKeyboardControl::instance());
	gameState.addStateLoop(StatePlaying,
		Main2DCamera::instance(), ShotCountDown::instance());
	gameState.addStateStimulus(StatePlaying, 
		StimGameStopped, StateGetPlayers);
	gameState.addStateStimulus(StatePlaying, 
		StimDisconnected, StateDisconnected);
	gameState.addStateStimulus(StatePlaying, 
		StimWait, StateWait);

	// StateShot
	addStandardComponents(gameState, StateShot);
	gameState.addStateEntry(StateShot, 
		TankKeyboardControl::instance());
	gameState.addStateEntry(StateShot,
		ClientShotState::instance());
	gameState.addStateStimulus(StateShot,
		ClientShotState::instance(), StateWait);
	gameState.addStateStimulus(StateShot, 
		StimDisconnected, StateDisconnected);
	gameState.addStateStimulus(StateShot, 
		StimWait, StateWait);
	gameState.addStateStimulus(StateShot, 
		StimGameStopped, StateGetPlayers);

	// StateScore
	addStandardComponents(gameState, StateScore);
	gameState.addStateLoop(StateScore,
		MainCamera::instance(), ClientSaveScreenState::instance());
	gameState.addStateStimulus(StateScore, 
		StimDisconnected, StateDisconnected);
	gameState.addStateStimulus(StateScore, 
		StimGameStopped, StateGetPlayers);
	gameState.addStateStimulus(StateScore, 
		StimWait, StateWait);
}
