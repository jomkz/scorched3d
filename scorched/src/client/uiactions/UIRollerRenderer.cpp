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

#include <uiactions/UIRollerRenderer.h>
#include <scorched3dc/OgreSystem.h>
#include <scorched3dc/ScorchedUI.h>
#include <actions/ShotBounce.h>

UIRollerRenderer::UIRollerRenderer(ShotBounce *shotBounce) :
	ClientUISyncActionRegisterable(true),
	shotBounce_(shotBounce)
{
}

UIRollerRenderer::~UIRollerRenderer()
{
}

void UIRollerRenderer::performUIAction()
{
	ClientUISyncActionRegisterable::performUIAction();

	if (!shotBounce_) 
	{
		delete this;
		return;
	}
	if (!projectileInstance_.isCreated()) projectileInstance_.create(shotBounce_->getWeapon()->getRollerModel());

	FixedVector &position = shotBounce_->getPhysics().getPosition();
	projectileInstance_.getSceneNode()->setPosition(
		position[0].getInternalData() * OgreSystem::OGRE_WORLD_SCALE_FIXED, 
		position[2].getInternalData() * OgreSystem::OGRE_WORLD_HEIGHT_SCALE_FIXED, 
		position[1].getInternalData() * OgreSystem::OGRE_WORLD_SCALE_FIXED);

	FixedVector4 &quat = shotBounce_->getPhysics().getRotationQuat();
	projectileInstance_.getSceneNode()->setOrientation(quat[0].asFloat(), quat[1].asFloat(),
		quat[2].asFloat(), quat[3].asFloat());
}

void UIRollerRenderer::simulate(Action *action, float frametime, bool &removeAction)
{
	registerCallback();
}

void UIRollerRenderer::deleteThis()
{
	shotBounce_ = 0;
	registerCallback();
}
