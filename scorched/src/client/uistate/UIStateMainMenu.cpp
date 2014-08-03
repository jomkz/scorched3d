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
#include <scorched3dc/OgreSystem.h>
#include <uistate/UIStateMainMenu.h>
#include <dialogs/GUIProgressCounter.h>
#include <dialogs/GUILogo.h>
#include <client/ClientState.h>
#include <client/ClientParams.h>
#include <client/ScorchedClient.h>

UIStateMainMenu::UIStateMainMenu() : 
	UIStateI(UIState::StateMainMenu),
	menuSceneManager_(0)
{
}

UIStateMainMenu::~UIStateMainMenu()
{
}

void UIStateMainMenu::createState()
{
	if (!menuSceneManager_) createSceneManager();

	// Attach camera to the window
	Ogre::RenderWindow *window = ScorchedUI::instance()->getOgreSystem().getOgreRenderWindow();
	Ogre::Viewport *vp = window->addViewport(camera_); //Our Viewport linked to the camera

	vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
	camera_->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

	GUILogo::instance()->setVisible(true);
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	{
		CEGUI::Window *start = wmgr.createWindow("OgreTray/Button", "StartButton");
		start->setText("Start");
		start->setPosition(CEGUI::UVector2(CEGUI::UDim(0.15f, 0.0f), CEGUI::UDim(0.00f, 0.0f)));
		start->setSize(CEGUI::USize(CEGUI::UDim(0.15f, 0.0f), CEGUI::UDim(0.05f, 0.0f)));
		start->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&UIStateMainMenu::start, this));

		CEGUI::Window *root = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
		root->addChild(start);
	}
}

bool UIStateMainMenu::start(const CEGUI::EventArgs &e)
{
	ScorchedUI::instance()->getUIState().setState(UIState::StateProgress);
	ClientParams::instance()->setStartCustom(true);
	ScorchedClient::startClient(GUIProgressCounter::instance());
	return true;
}

void UIStateMainMenu::destroyState()
{
	CEGUI::Window *root = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	wmgr.destroyWindow(root->getChild("StartButton"));
	GUILogo::instance()->setVisible(false);

	Ogre::RenderWindow *window = ScorchedUI::instance()->getOgreSystem().getOgreRenderWindow();
	window->removeAllViewports();
}

static void addDirection(const Ogre::Vector3 &position, const Ogre::Radian &dir, 
	float time, Ogre::NodeAnimationTrack* mCamTrack)
{
	Ogre::TransformKeyFrame *mCamKey = mCamTrack->createNodeKeyFrame(time);
	mCamKey->setTranslate(position);

	Ogre::Vector3 direction = position;
	direction.normalise();

	float res = 3.14f - atan2(direction.x, direction.z);

	//Ogre::Radian dir(res);

	Ogre::Quaternion quat(dir, Ogre::Vector3::UNIT_Y);
	mCamKey->setRotation(quat);
}

void UIStateMainMenu::createSceneManager()
{
	Ogre::Root *ogreRoot = ScorchedUI::instance()->getOgreSystem().getOgreRoot();
	menuSceneManager_ = ogreRoot->createSceneManager(Ogre::ST_GENERIC, "MainMenuSceneManager");

	// Add the tank
	Ogre::Entity *targetEntity = menuSceneManager_->createEntity("MainMenuTank", "abrams.mesh", "Models");
	Ogre::SceneNode *targetNode = menuSceneManager_->getRootSceneNode()->createChildSceneNode("MainMenuTankNode");
	targetNode->attachObject(targetEntity);
	targetNode->setScale(OgreSystem::OGRE_WORLD_SCALE, OgreSystem::OGRE_WORLD_SCALE, OgreSystem::OGRE_WORLD_SCALE);

	// Create viewing camera
	camera_ = menuSceneManager_->createCamera("MainMenuCamera");
	camera_->setNearClipDistance(5);

	// Create a light
	Ogre::Light* pointLight = menuSceneManager_->createLight("MainMenuLight");
	pointLight->setType(Ogre::Light::LT_POINT);
	pointLight->setPosition(Ogre::Vector3(0, 150, 250));
	pointLight->setDiffuseColour(1.0, 1.0, 1.0);
	pointLight->setSpecularColour(1.0, 1.0, 1.0);
	menuSceneManager_->setAmbientLight(Ogre::ColourValue(0.2f, 0.2f, 0.2f));
	Ogre::SceneNode* lightNode = menuSceneManager_->getRootSceneNode()->createChildSceneNode("AnimLightNode");
	lightNode->attachObject(pointLight);

	// Animate the camera
	std::vector<FixedVector> controlPoints;
	controlPoints.push_back(FixedVector());
	controlPoints.push_back(FixedVector(250, 50, 250));
	controlPoints.push_back(FixedVector(-250, 50, 250));
	controlPoints.push_back(FixedVector(-250, 50, -250));
	controlPoints.push_back(FixedVector(250, 50, -250));

	// Add a control point at the end to join the loop
	FixedVector midPt = (controlPoints[1] + controlPoints.back()) / 2;
	controlPoints.push_back(midPt);
	controlPoints.front() = midPt;

	path_.generate(controlPoints, 200, 3, 20);
	updateState(0.0f);
}

void UIStateMainMenu::updateState(float frameTime)
{
	path_.simulate(fixed::fromFloat(frameTime));
	FixedVector position;
	FixedVector direction;
	path_.getPathAttrs(position, direction);
	Vector vPosition;
	position.asVector(vPosition);
	camera_->setPosition(vPosition[0], vPosition[1], vPosition[2]);
	camera_->lookAt(0, 0, 0);
}