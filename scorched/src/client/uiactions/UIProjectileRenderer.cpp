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

#include <uiactions/UIProjectileRenderer.h>
#include <scorched3dc/OgreSystem.h>
#include <scorched3dc/ScorchedUI.h>
#include <actions/ShotProjectile.h>

UIProjectileRenderer::UIProjectileRenderer(ShotProjectile *shotProjectile) :
	ClientUISyncActionRegisterable(true),
	shotProjectile_(shotProjectile)
{
}

UIProjectileRenderer::~UIProjectileRenderer()
{
	ENSURE_UI_THREAD
}

void UIProjectileRenderer::performUIAction()
{
	ClientUISyncActionRegisterable::performUIAction();
	if (!shotProjectile_) 
	{
		delete this;
		return;
	}
	if (!projectileInstance_.isCreated()) projectileInstance_.create(shotProjectile_->getWeapon()->getModel());

	FixedVector &position = shotProjectile_->getPhysics().getPosition();
	projectileInstance_.getSceneNode()->setPosition(
		position[0].getInternalData() * OgreSystem::OGRE_WORLD_SCALE_FIXED, 
		position[2].getInternalData() * OgreSystem::OGRE_WORLD_HEIGHT_SCALE_FIXED, 
		position[1].getInternalData() * OgreSystem::OGRE_WORLD_SCALE_FIXED);
	FixedVector &velocity = shotProjectile_->getPhysics().getVelocity();
	projectileInstance_.getSceneNode()->setDirection(
		velocity[0].asFloat(),
		velocity[2].asFloat(),
		velocity[1].asFloat(),
		Ogre::Node::TS_WORLD);
}

void UIProjectileRenderer::simulate(Action *action, float frametime, bool &removeAction)
{
	registerCallback();
}

void UIProjectileRenderer::deleteThis()
{
	shotProjectile_ = 0;
	registerCallback();
}
