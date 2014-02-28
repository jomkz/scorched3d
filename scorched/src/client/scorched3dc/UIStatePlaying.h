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

#if !defined(__INCLUDE_UIStatePlayingh_INCLUDE__)
#define __INCLUDE_UIStatePlayingh_INCLUDE__

#include <scorched3dc/UIStateI.h>
#include <scorched3dc/CameraController.h>

class UIStatePlayingEnv;
class UIStatePlayingLand;
class UIStatePlayingTargets;
class CameraController;
class UIStatePlaying : public UIStateI, public CameraControllerHeightProvider
{
public:
	UIStatePlaying();
	virtual ~UIStatePlaying();

	void updateHeight(int x, int y, int w, int h);

	// UIStateI
	virtual void createState();
	virtual void destroyState();
	virtual void updateState(float frameTime);

	// CameraControllerHeightProvider
	virtual Ogre::Real getHeight(const Ogre::Vector3 &position);
	virtual bool getIntersection(const Ogre::Ray &cameraRay, Ogre::Vector3 *outPosition);
protected:
	Ogre::SceneManager* sceneMgr_;
	CameraController *cameraController_;
	UIStatePlayingEnv *env_;
	UIStatePlayingLand *land_;
	UIStatePlayingTargets *targets_;
};

#endif // __INCLUDE_UIStatePlayingh_INCLUDE__
