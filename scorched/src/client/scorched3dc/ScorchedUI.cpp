////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2013
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

#include <scorched3dc/ScorchedUI.h>
#include <scorched3dc/UIState.h>
#include <scorched3dc/OgreSystem.h>
#include <scorched3dc/InputManager.h>
#include <engine/ThreadCallback.h>
#include <common/Clock.h>

#include <common/Logger.h>
#include <common/FileLogger.h>
#include <lang/Lang.h>

ScorchedUI *ScorchedUI::instance_(0);

ScorchedUI *ScorchedUI::instance()
{
	return instance_;
}

ScorchedUI::ScorchedUI() : 
	quit_(false)
{
	DIALOG_ASSERT(!instance_);
	instance_ = this;
	uiState_ = new UIState();
	ogreSystem_ = new OgreSystem();
	inputManager_ = new InputManager();
	uiThreadCallback_ = new ThreadCallback();
}

ScorchedUI::~ScorchedUI()
{
	delete uiState_;
	delete ogreSystem_;
	delete inputManager_;
	delete uiThreadCallback_;
	instance_ = 0;
}

bool ScorchedUI::go()
{
	// Setup S3D logger
	Logger::addLogger(new FileLogger("Scorched3D.log", ".", false), false);
	Logger::log(S3D::formatStringBuffer("Scorched3D - Version %s (%s) - %s",
		S3D::ScorchedVersion.c_str(), 
		S3D::ScorchedProtocolVersion.c_str(), 
		S3D::ScorchedBuildTime.c_str()));

	// Initialize Ogre
	if (!ogreSystem_->create()) return false;

	// Connect to the keyboard and mouse inputs
	inputManager_->create(ogreSystem_->getOgreRoot(), ogreSystem_->getOgreRenderWindow());

	// Create CEGUI
	if (!ogreSystem_->createUI()) return false;

	// Create the first scene
	uiState_->setState(UIState::StatePlaying);//UIState::StateMainMenu);

	Clock timer;
	while(!quit_)
	{
		float frameTime = timer.getTimeDifference();

		// Pump window messages for nice behaviour
		Ogre::WindowEventUtilities::messagePump();
 
		// Update UI
		uiThreadCallback_->processCallbacks();
 
		// Render a frame
		uiState_->updateState(frameTime);
		if(ogreSystem_->getOgreRenderWindow()->isClosed())
		{
			break;
		}
		if(!ogreSystem_->getOgreRoot()->renderOneFrame(frameTime)) 
		{
			break;
		}
	}

	Lang::instance()->saveUndefined();

	return true;
}
