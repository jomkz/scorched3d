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

#include <uiactions/UITargetRenderer.h>
#include <client/ScorchedClient.h>
#include <scorched3dc/OgreSystem.h>
#include <scorched3dc/ScorchedUI.h>
#include <target/TargetContainer.h>
#include <target/TargetLife.h>
#include <tank/Tank.h>
#include <common/Logger.h>

UITargetRenderer::UITargetRenderer(Target *target) :
	target_(target), registered_(-1), targetNode_(0), targetEntity_(0)
{
}

UITargetRenderer::~UITargetRenderer()
{
	if (registered_ != -1)
	{
		ScorchedClient::instance()->getClientUISync().removeActionFromClient(registered_);
	}
	if (targetNode_)
	{
		// TODO: Destroy node
		targetNode_ = 0;
		targetEntity_ = 0;
	}
}

void UITargetRenderer::performUIAction()
{
	if (!targetNode_) create();

	if (target_->getAlive())
	{
		if (!targetEntity_->isVisible()) targetEntity_->setVisible(true);

		Vector position = target_->getLife().getFloatPosition();
		position[0] *= OgreSystem::OGRE_WORLD_SCALE;
		position[1] *= OgreSystem::OGRE_WORLD_SCALE;
		position[2] *= OgreSystem::OGRE_WORLD_HEIGHT_SCALE;
		targetNode_->setPosition(position[0], position[2], position[1]);
	}
	else 
	{
		targetEntity_->setVisible(false);
	}

	registered_ = -1;
}

void UITargetRenderer::moved()
{
	if (registered_ == -1)
	{
		registered_ = ScorchedClient::instance()->getClientUISync().addActionFromClient(this);
	}
}

void UITargetRenderer::targetBurnt()
{
}

void UITargetRenderer::shieldHit()
{
}

void UITargetRenderer::fired()
{
}

void UITargetRenderer::create()
{
	Ogre::SceneManager *sceneManager = ScorchedUI::instance()->getOgreSystem().getOgreLandscapeSceneManager();

	std::string entityName = S3D::formatStringBuffer("Target%u", target_->getPlayerId());
	std::string nodeName = S3D::formatStringBuffer("TargetNode%u", target_->getPlayerId());
	targetEntity_ = sceneManager->createEntity(entityName.c_str(), "abrams.mesh", "Models");
	targetEntity_->setVisibilityFlags(OgreSystem::VisibiltyMaskTargets);
	targetNode_ = sceneManager->getRootSceneNode()->createChildSceneNode(nodeName);
	targetNode_->attachObject(targetEntity_);
	targetNode_->setScale(40.0f, 40.0f, 40.0f);
}
