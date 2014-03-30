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

#include <uistate/UIStateProgress.h>
#include <dialogs/GUIProgressCounter.h>
#include <scorched3dc/ScorchedUI.h>
#include <scorched3dc/OgreSystem.h>

UIStateProgress::UIStateProgress() : 
	UIStateI(UIState::StateProgress),
	sceneManagerCreated_(false)
{
}

UIStateProgress::~UIStateProgress()
{
}

void UIStateProgress::createState()
{
	if (!sceneManagerCreated_)
	{
		createSceneManager();
		sceneManagerCreated_ = true;
	}

	// Attach camera to the window
	Ogre::RenderWindow *window = ScorchedUI::instance()->getOgreSystem().getOgreRenderWindow();
	Ogre::Viewport *vp = window->addViewport(camera_); //Our Viewport linked to the camera

	vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
	camera_->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

	GUIProgress::instance()->setVisible(true);
}

void UIStateProgress::destroyState()
{
	GUIProgress::instance()->setVisible(false);

	Ogre::RenderWindow *window = ScorchedUI::instance()->getOgreSystem().getOgreRenderWindow();
	window->removeAllViewports();
}

void UIStateProgress::createSceneManager()
{
	// Create background rectangle covering the whole screen
	Ogre::Rectangle2D* rect = new Ogre::Rectangle2D(true);
	rect->setCorners(-1.0, 1.0, 1.0, -1.0);
	rect->setMaterial("General/Background");

	// Render the background before everything else
	rect->setRenderQueueGroup(Ogre::RENDER_QUEUE_BACKGROUND);

	// Use infinite AAB to always stay visible
	Ogre::AxisAlignedBox aabInf;
	aabInf.setInfinite();
	rect->setBoundingBox(aabInf);

	// Attach background to the scene
	Ogre::SceneManager *sceneMgr = ScorchedUI::instance()->getOgreSystem().getOgreProgressSceneManager();
	Ogre::SceneNode* node = sceneMgr->getRootSceneNode()->createChildSceneNode("Background");
	node->attachObject(rect);

	// Create viewing camera
	camera_ = sceneMgr->createCamera("ProgressCamera");
}