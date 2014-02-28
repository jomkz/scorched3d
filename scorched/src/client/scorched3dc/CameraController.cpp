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
#include <scorched3dc/ScorchedUI.h>
#include <scorched3dc/OgreSystem.h>
#include <scorched3dc/InputManager.h>
#include <client/ClientOptions.h>

CameraController::CameraController(
	Ogre::SceneManager* sceneMgr) : 
	sceneMgr_(sceneMgr), camera_(0), simulationTime_(0.0f)
{
	create();
	ScorchedUI::instance()->getInputManager().addMouseHandler(this);
}

CameraController::~CameraController()
{
	delete camera_;
	ScorchedUI::instance()->getInputManager().removeMouseHandler(this);
}

void CameraController::create()
{
	// Create camera
	camera_ = sceneMgr_->createCamera("PlayerCam");
	camera_->setNearClipDistance(5);
	camera_->setFarClipDistance(99999*6);

	// Setup the initial position
	rotationRound_ = 0.0;
	rotationUpDown_ = 3.14 / 3.0;
	zoom_ = 2000.0;
	wantedTarget_ = Ogre::Vector3(0.0, 0.0, 0.0);
	calculateWantedPosition();

	// Set camera position
	currentTarget_ = wantedTarget_;
	camera_->setPosition(wantedCamera_);
	camera_->lookAt(wantedTarget_);
}

void CameraController::calculateWantedPosition()
{
	if (rotationUpDown_ > 1.7f) rotationUpDown_ = 1.7f;
	else if (rotationUpDown_ < 0.1f) rotationUpDown_ = 0.1f;
	if (zoom_ < 100.0f) zoom_ = 100.0f;
	else if (zoom_ > 14000.0f) zoom_ = 14000.0f;

	wantedCamera_[0] = zoom_ * float(sin(rotationRound_) * sin(rotationUpDown_));
	wantedCamera_[1] = zoom_ * float(cos(rotationUpDown_));
	wantedCamera_[2] = zoom_ * float(cos(rotationRound_) * sin(rotationUpDown_));
	wantedCamera_ += wantedTarget_;
}

void CameraController::scroll(Ogre::Real distance, Ogre::Real rotation)
{
	wantedTarget_ += Ogre::Vector3(float(sin(rotationRound_ + rotation) * distance), 0.0f, 
		float(cos(rotationRound_ + rotation) * distance));
	calculateWantedPosition();
}

void CameraController::update(float elapsedTime)
{
	Ogre::PolygonMode wantedMode = ClientOptions::instance()->getCameraWireframe()?Ogre::PM_WIREFRAME:Ogre::PM_SOLID;
	if (camera_->getPolygonMode() != wantedMode)
	{
		camera_->setPolygonMode(wantedMode);
	}

	InputManager &inputManager = ScorchedUI::instance()->getInputManager();
	if (inputManager.isKeyDown(CEGUI::Key::W))
	{
		Ogre::Real distance = elapsedTime * zoom_ / -5.0f;
		scroll(distance, 0.0f);
	} 
	else if (inputManager.isKeyDown(CEGUI::Key::S))
	{
		Ogre::Real distance = elapsedTime * zoom_ / 5.0f;
		scroll(distance, 0.0f);
	}
	if (inputManager.isKeyDown(CEGUI::Key::A))
	{
		Ogre::Real distance = elapsedTime * zoom_ / -5.0f;
		scroll(distance, 1.57f);
	} 
	else if (inputManager.isKeyDown(CEGUI::Key::D))
	{
		Ogre::Real distance = elapsedTime * zoom_ / 5.0f;
		scroll(distance, 1.57f);
	}
	if (inputManager.isKeyDown(CEGUI::Key::R))
	{
		zoom_ += -1000.0f * elapsedTime;
		calculateWantedPosition();
	} 
	else if (inputManager.isKeyDown(CEGUI::Key::F))
	{
		zoom_ += 1000.0f * elapsedTime;
		calculateWantedPosition();
	}
	if (inputManager.isKeyDown(CEGUI::Key::Q))
	{
		rotationRound_ += 3.14f / 4.0f * elapsedTime;
		calculateWantedPosition();
	} 
	else if (inputManager.isKeyDown(CEGUI::Key::E))
	{
		rotationRound_ += 3.14f / -4.0f * elapsedTime;
		calculateWantedPosition();
	}

	// A simulation loop to keep the camera movement speed constant
	// regardless of frametime
	const Ogre::Real SimulationTimeStep = 1.0f / 66.0f;
	simulationTime_ += elapsedTime;
	while (simulationTime_ > SimulationTimeStep)
	{
		simulationTime_ -= SimulationTimeStep;

		Ogre::Real snap = 0.1;
		Ogre::Vector3 updateTarget = (wantedTarget_ - currentTarget_) * snap;
		updateTarget += currentTarget_;
		Ogre::Vector3 updateCamera = (wantedCamera_ - camera_->getPosition()) * snap;
		updateCamera += camera_->getPosition();

		Ogre::Real minHeight = 0.0f;
		if (heightProvider_) minHeight = heightProvider_->getHeight(updateCamera);
		if (updateCamera[1] < minHeight + 30.0f)
		{
			updateCamera[1] = minHeight + 30.0f;
		}

		camera_->setPosition(updateCamera);
		camera_->lookAt(updateTarget);
		currentTarget_ = updateTarget;
	}
}

void CameraController::mouseClick(int positionX, int positionY, int mouseButton)
{

}

void CameraController::mouseDrag(int positionX, int positionY, int positionDeltaX, 
	int positionDeltaY, int mouseButton)
{
	if (mouseButton == OIS::MB_Left)
	{
		scroll(((Ogre::Real) positionDeltaX) * zoom_ / -900.0f, 1.57f);
		scroll(((Ogre::Real) positionDeltaY) * zoom_ / -900.0f, 0.0f);
	}
	else if (mouseButton == OIS::MB_Right)
	{
		rotationRound_ += -3.14f / 400.0f * positionDeltaX;
		rotationUpDown_ += -3.14f / 400.0f * positionDeltaY;
		calculateWantedPosition();
	}
	else if (mouseButton == OIS::MB_Middle)
	{
		zoom_ += positionDeltaY * 5.0f;
		calculateWantedPosition();
	}
}

void CameraController::mouseWheel(int positionDelta)
{
	if (positionDelta < 0.0f)
	{
		zoom_ *= 1.15f;
	}
	else
	{
		zoom_ *= 0.85f;
	}
	calculateWantedPosition();
}
