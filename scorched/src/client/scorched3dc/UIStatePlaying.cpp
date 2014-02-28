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

#include <scorched3dc/UIStatePlaying.h>
#include <scorched3dc/UIStatePlayingEnv.h>
#include <scorched3dc/UIStatePlayingLand.h>
#include <scorched3dc/UIStatePlayingTargets.h>
#include <scorched3dc/CameraController.h>
#include <scorched3dc/ScorchedUI.h>
#include <scorched3dc/OgreSystem.h>
#include <common/DefinesMath.h>

UIStatePlaying::UIStatePlaying() : 
	UIStateI(UIState::StatePlaying),
	sceneMgr_(0), land_(0), env_(0), cameraController_(0)
{

}

UIStatePlaying::~UIStatePlaying()
{
	delete targets_;
	delete cameraController_;
	delete env_;
	delete land_;
}

void UIStatePlaying::createState()
{
	Ogre::LogManager::getSingletonPtr()->logMessage("*** Creating Playing Scene ***");

	sceneMgr_ = ScorchedUI::instance()->getOgreSystem().getOgreLandscapeSceneManager();
	Ogre::RenderWindow *ogreRenderWindow = ScorchedUI::instance()->getOgreSystem().getOgreRenderWindow();

	// Create camera 
	cameraController_ = new CameraController(sceneMgr_);
	cameraController_->setHeightProvider(this);

	// Create one viewport, entire window
	Ogre::Viewport* vp = ogreRenderWindow->addViewport(cameraController_->getCamera());
	vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

	// Alter the camera aspect ratio to match the viewport
	cameraController_->getCamera()->setAspectRatio(
		Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

	// Create the environment (sky, sea)
	Ogre::LogManager::getSingletonPtr()->logMessage("*** Creating Environment ***");
	env_ = new UIStatePlayingEnv(sceneMgr_, cameraController_->getCamera());

	// Create the land
	Ogre::LogManager::getSingletonPtr()->logMessage("*** Creating Landscape ***");
	land_ = new UIStatePlayingLand(sceneMgr_, cameraController_->getCamera(), 
		env_->getSunLight(), env_->getShadowLight(), env_->getHydraX());
	land_->updateLandscapeTextures();

	// Create the targets
	targets_ = new UIStatePlayingTargets(sceneMgr_);

	// Bloom compositor
	Ogre::CompositorManager::getSingleton().addCompositor(vp, "Bloom2");
	Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, "Bloom2", false);
}

void UIStatePlaying::destroyState()
{

}

void UIStatePlaying::updateState(float frameTime)
{
	cameraController_->update(frameTime);
	env_->update(frameTime);
	land_->update(frameTime);
	targets_->update(frameTime);
}

Ogre::Real UIStatePlaying::getHeight(const Ogre::Vector3 &position)
{
	return S3D_MAX(land_->getHeight(position), env_->getWaterHeight());
}

void UIStatePlaying::updateHeight(int x, int y, int w, int h)
{
	land_->updateHeight(x, y, w, h);
}
