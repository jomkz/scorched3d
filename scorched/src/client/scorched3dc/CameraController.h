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

#if !defined(__INCLUDE_CameraControllerh_INCLUDE__)
#define __INCLUDE_CameraControllerh_INCLUDE__

#include <scorched3dc/InputHandlerMouse.h>

class CameraControllerHeightProvider 
{
public:
	virtual Ogre::Real getHeight(const Ogre::Vector3 &position) = 0;
	virtual bool getIntersection(const Ogre::Ray &cameraRay, Ogre::Vector3 *outPosition) = 0;
};

class CameraController : 
	public InputHandlerMouse
{
public:
	CameraController(Ogre::SceneManager* sceneMgr);
	virtual ~CameraController();

	Ogre::Camera* getCamera() { return camera_; }

	void update(float frameTime);

	void setWantedTarget(const Ogre::Vector3 &position);
	void setHeightProvider(CameraControllerHeightProvider *heightProvider) { heightProvider_ = heightProvider; }

	// InputHandlerMouse
	virtual void mouseClick(int positionX, int positionY, int mouseButton);
	virtual void mouseDrag(int positionX, int positionY, int positionDeltaX, 
		int positionDeltaY, int mouseButton);
	virtual void mouseDragStop(int positionX, int positionY, int mouseButton);
	virtual void mouseDragStart(int positionX, int positionY, int mouseButton);
	virtual void mouseWheel(int positionDelta);

protected:
	Ogre::SceneManager* sceneMgr_;
	Ogre::Camera* camera_;
	Ogre::Vector3 wantedTarget_, currentTarget_;
	Ogre::Vector3 wantedCamera_;
	Ogre::Real rotationRound_, rotationUpDown_;
	Ogre::Real zoom_;
	Ogre::Real simulationTime_;
	CameraControllerHeightProvider *heightProvider_;

	void create();
	void calculateWantedPosition();
	void scroll(Ogre::Real distance, Ogre::Real rotation);
};

#endif // __INCLUDE_CameraControllerh_INCLUDE__
