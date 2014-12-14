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
#include <models/MiniLandscape.h>
#include <scorched3dc/ScorchedUI.h>
#include <scorched3dc/OgreSystem.h>

UIStateProgress::UIStateProgress() : 
	UIStateI(UIState::StateProgress),
	progressSceneManager_(0)
{
}

UIStateProgress::~UIStateProgress()
{
}

void UIStateProgress::createState()
{
	if (!progressSceneManager_) createSceneManager();

	// Attach camera to the window
	Ogre::RenderWindow *window = ScorchedUI::instance()->getOgreSystem().getOgreRenderWindow();
	Ogre::Viewport *vp = window->addViewport(camera_); //Our Viewport linked to the camera

	vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
	camera_->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

	titleText_->setText("Loading...");
	descriptionText_->setText("");
	progressBar_->setProgress(0.0f);
	window_->setVisible(true);
}

void UIStateProgress::destroyState()
{
	window_->setVisible(false);
	Ogre::RenderWindow *window = ScorchedUI::instance()->getOgreSystem().getOgreRenderWindow();
	window->removeAllViewports();
}

void UIStateProgress::createSceneManager()
{
	MiniLandscape::instance()->create();
	CEGUI::BasicImage *ceGuiTex = MiniLandscape::instance()->createGUITexture();

	Ogre::Root *ogreRoot = ScorchedUI::instance()->getOgreSystem().getOgreRoot();
	progressSceneManager_ = ogreRoot->createSceneManager(Ogre::ST_GENERIC, "ProgressSceneManager");

	// Create background rectangle covering the whole screen
	Ogre::Rectangle2D* rect = new Ogre::Rectangle2D(true);
	rect->setCorners(-1.0, 1.0, 1.0, -1.0);
	rect->setMaterial("General/Black"); 

	// Render the background before everything else
	rect->setRenderQueueGroup(Ogre::RENDER_QUEUE_BACKGROUND);

	// Use infinite AAB to always stay visible
	Ogre::AxisAlignedBox aabInf;
	aabInf.setInfinite();
	rect->setBoundingBox(aabInf);

	// Attach background to the scene
	Ogre::SceneNode* node = progressSceneManager_->getRootSceneNode()->createChildSceneNode("Background");
	node->attachObject(rect);

	// Create viewing camera
	camera_ = progressSceneManager_->createCamera("ProgressCamera");

	// Window code
	CEGUI::WindowManager *pWindowManager = CEGUI::WindowManager::getSingletonPtr();
	window_ = pWindowManager->loadLayoutFromFile("Progress2.layout");
	DIALOG_ASSERT(window_);
	CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->addChild(window_);
	titleText_ = static_cast<CEGUI::Window *>(window_->getChild("ProgressImage/TitleText"));;
	descriptionText_ = static_cast<CEGUI::Window *>(window_->getChild("ProgressImage/DescriptionText"));;
	progressBar_ = static_cast<CEGUI::ProgressBar*>(window_->getChild("ProgressImage/ProgressBar"));

	CEGUI::Window *landscapeImage = static_cast<CEGUI::Window*>(window_->getChild("LandscapeImage"));
	landscapeImage->setProperty("Image", ceGuiTex->getName());
}

void UIStateProgress::updateProgress(const LangString &op, float progress)
{
	
}

void UIStateProgress::updateDescription(const std::string &name, const std::string &description)
{
	titleText_->setText(name);
	descriptionText_->setText(description);
}

void UIStateProgress::updateLandscape()
{
	MiniLandscape::instance()->update();
}

void UIStateProgress::updateState(float frameTime)
{

}

GUIProgressUpdateThreadCallback::GUIProgressUpdateThreadCallback(const LangString &op, const float percentage) :
	op_(op), percentage_(percentage)
{
}

GUIProgressUpdateThreadCallback::~GUIProgressUpdateThreadCallback()
{
}

void GUIProgressUpdateThreadCallback::callbackInvoked()
{
	if (ScorchedUI::instance()->getUIState().getState() != UIState::StateProgress)
	{
		return;
	}

	UIStateProgress *progress = (UIStateProgress *)ScorchedUI::instance()->getUIState().getCurrentState();
	progress->updateProgress(op_, percentage_);
	delete this;
}

GUIProgressDescriptionThreadCallback::GUIProgressDescriptionThreadCallback(const std::string &name, const std::string &description) :
	name_(name), description_(description)
{
}

GUIProgressDescriptionThreadCallback::~GUIProgressDescriptionThreadCallback()
{
}

void GUIProgressDescriptionThreadCallback::performUIAction()
{
	if (ScorchedUI::instance()->getUIState().getState() != UIState::StateProgress)
	{
		return;
	}

	UIStateProgress *progress = (UIStateProgress *)ScorchedUI::instance()->getUIState().getCurrentState();
	progress->updateDescription(name_, description_);
	delete this;
}

GUIProgressLandscapeThreadCallback::GUIProgressLandscapeThreadCallback()
{
}

GUIProgressLandscapeThreadCallback::~GUIProgressLandscapeThreadCallback()
{
}

void GUIProgressLandscapeThreadCallback::performUIAction()
{
	if (ScorchedUI::instance()->getUIState().getState() != UIState::StateProgress)
	{
		return;
	}

	UIStateProgress *progress = (UIStateProgress *)ScorchedUI::instance()->getUIState().getCurrentState();
	progress->updateLandscape();
	delete this;
}
