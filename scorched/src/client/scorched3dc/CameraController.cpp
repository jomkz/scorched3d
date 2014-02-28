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

#include <scorched3dc/CameraController.h>
#include <scorched3dc/Scorched3DC.h>

CameraController::CameraController(
	Ogre::SceneManager* sceneMgr) : 
	sceneMgr_(sceneMgr), camera_(0)
{
	create();
}

CameraController::~CameraController()
{
	cameraNode_->detachAllObjects ();
	delete camera_;
	sceneMgr_->destroySceneNode(cameraNode_);
	sceneMgr_->destroySceneNode(targetNode_);
	Ogre::Root *ogreRoot = Scorched3DC::instance()->getOgreRoot();
	ogreRoot->removeFrameListener(this);
}

void CameraController::create()
{
	// Create tacking nodes
	cameraNode_ = sceneMgr_->getRootSceneNode ()->createChildSceneNode("Camera_Node");
	targetNode_ = sceneMgr_->getRootSceneNode ()->createChildSceneNode("Camera_Target_Node");
	cameraNode_->setAutoTracking(true, targetNode_); // The camera will always look at the camera target
	cameraNode_->setFixedYawAxis(true); // Needed because of auto tracking

	// Create camera
	camera_ = sceneMgr_->createCamera("PlayerCam");
	camera_->setNearClipDistance(5);
	camera_->setFarClipDistance(99999*6);
	cameraNode_->attachObject(camera_);

	// Setup the initial position
	rotationRound_ = 0.0;
	rotationUpDown_ = 3.14 / 3.0;
	zoom_ = 500.0;
	wantedTarget_ = Ogre::Vector3(0.0, 0.0, 0.0);
	calculateWantedPosition();

	// Set camera position
	targetNode_->setPosition(wantedTarget_);
	cameraNode_->setPosition(wantedCamera_);

	Ogre::Root *ogreRoot = Scorched3DC::instance()->getOgreRoot();
	ogreRoot->addFrameListener(this);
}

void CameraController::calculateWantedPosition()
{
	wantedCamera_[0] = zoom_ * float(sin(rotationRound_) * sin(rotationUpDown_));
	wantedCamera_[1] = zoom_ * float(cos(rotationUpDown_));
	wantedCamera_[2] = zoom_ * float(cos(rotationRound_) * sin(rotationUpDown_));
	wantedCamera_ += wantedTarget_;
}

bool CameraController::frameStarted(const Ogre::FrameEvent &e)
{
	Ogre::Real elapsedTime = e.timeSinceLastFrame;

	OIS::Keyboard *keyboard = Scorched3DC::instance()->getKeyboard();
	if (keyboard->isKeyDown(OIS::KC_W))
	{
		wantedTarget_ += Ogre::Vector3(0.0f, 0.0f, -elapsedTime * zoom_ / 5.0f);
		calculateWantedPosition();
	} 
	else if (keyboard->isKeyDown(OIS::KC_S))
	{
		wantedTarget_ += Ogre::Vector3(0.0f, 0.0f, elapsedTime * zoom_ / 5.0f);
		calculateWantedPosition();
	}
	if (keyboard->isKeyDown(OIS::KC_A))
	{
		wantedTarget_ += Ogre::Vector3(-elapsedTime * zoom_ / 5.0f, 0.0f, 0.0f);
		calculateWantedPosition();
	} 
	else if (keyboard->isKeyDown(OIS::KC_D))
	{
		wantedTarget_ += Ogre::Vector3(elapsedTime * zoom_ / 5.0f, 0.0f, 0.0f);
		calculateWantedPosition();
	}
	if (keyboard->isKeyDown(OIS::KC_R))
	{
		zoom_ += 1000.0f * elapsedTime;
		calculateWantedPosition();
	} 
	else if (keyboard->isKeyDown(OIS::KC_F))
	{
		zoom_ += 1000.0f * -elapsedTime;
		if (zoom_ < 100.0f) zoom_ = 100.0f;
		calculateWantedPosition();
	}
	if (keyboard->isKeyDown(OIS::KC_Q))
	{
		rotationRound_ += 3.14f / 4.0f * elapsedTime;
		calculateWantedPosition();
	} 
	else if (keyboard->isKeyDown(OIS::KC_E))
	{
		rotationRound_ += 3.14f / 4.0f * -elapsedTime;
		calculateWantedPosition();
	}

	Ogre::Real snap = 0.1;
	Ogre::Vector3 updateTarget = (wantedTarget_ - targetNode_->getPosition()) * snap;
	updateTarget += targetNode_->getPosition();
	targetNode_->setPosition(updateTarget);
	Ogre::Vector3 updateCamera = (wantedCamera_ - cameraNode_->getPosition()) * snap;
	updateCamera += cameraNode_->getPosition();
	cameraNode_->setPosition(updateCamera);

	return true;
}
