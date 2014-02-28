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

#include <scorched3dc/UIStatePlayingTargets.h>
#include <scorched3dc/ScorchedUI.h>
#include <scorched3dc/InputManager.h>
#include <scorched3dc/OgreSystem.h>
#include <client/ScorchedClient.h>
#include <client/ClientOptions.h>
#include <uiactions/UITankRenderer.h>

UIStatePlayingTargets::UIStatePlayingTargets(Ogre::SceneManager* sceneMgr) :
	sceneMgr_(sceneMgr), tankRenderer_(0)
{
	create();
}

UIStatePlayingTargets::~UIStatePlayingTargets()
{

}

void UIStatePlayingTargets::update(float frameTime)
{
	bool targetsEnabled = (sceneMgr_->getVisibilityMask() & OgreSystem::VisibiltyMaskTargets) != 0;
	if (targetsEnabled != ClientOptions::instance()->getTargetsDraw())
	{
		sceneMgr_->setVisibilityMask(sceneMgr_->getVisibilityMask() ^ OgreSystem::VisibiltyMaskTargets);
	}

	if (tankRenderer_)
	{
		fixed fixedFrameTime = fixed::fromFloat(frameTime);

		bool changedValues = false;
		InputManager &inputManager = ScorchedUI::instance()->getInputManager();
		if (inputManager.isKeyDown(OIS::KC_LEFT))
		{
			changedValues = true;
			tankRenderer_->getShotHistory().rotateGunXY(fixedFrameTime * 45, true);
		}
		else if (inputManager.isKeyDown(OIS::KC_RIGHT))
		{
			changedValues = true;
			tankRenderer_->getShotHistory().rotateGunXY(fixedFrameTime * -45, true);
		}

		if (inputManager.isKeyDown(OIS::KC_UP))
		{
			changedValues = true;
			tankRenderer_->getShotHistory().rotateGunYZ(fixedFrameTime * 20, true);
		}
		else if (inputManager.isKeyDown(OIS::KC_DOWN))
		{
			changedValues = true;
			tankRenderer_->getShotHistory().rotateGunYZ(fixedFrameTime * -20, true);
		}

		if (inputManager.isKeyDown(OIS::KC_EQUALS))
		{
			changedValues = true;
			tankRenderer_->getShotHistory().changePower(fixedFrameTime * 100, true);
		}
		else if (inputManager.isKeyDown(OIS::KC_MINUS))
		{
			changedValues = true;
			tankRenderer_->getShotHistory().changePower(fixedFrameTime * -100, true);
		}

		if (changedValues)
		{
			tankRenderer_->setRotations();
		}
	}
}

void UIStatePlayingTargets::setCurrentTank(UITankRenderer *tankRenderer)
{
	tankRenderer_ = tankRenderer;
}

void UIStatePlayingTargets::create()
{

}
