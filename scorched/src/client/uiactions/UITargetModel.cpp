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

#include <uiactions/UITargetModel.h>
#include <uiactions/UITargetRenderer.h>
#include <client/ScorchedClient.h>
#include <scorched3dc/OgreSystem.h>
#include <scorched3dc/ScorchedUI.h>
#include <target/TargetContainer.h>
#include <target/TargetLife.h>
#include <tank/Tank.h>
#include <common/Logger.h>

UITargetModel::UITargetModel(UITargetRenderer *targetRenderer) :
	targetRenderer_(targetRenderer)
{
	positionChangedRegisterable_ = 
		new ClientUISyncActionRegisterableAdapter<UITargetModel>(this, &UITargetModel::positionChangedSync, true);
}

UITargetModel::~UITargetModel()
{
	ENSURE_UI_THREAD
	delete positionChangedRegisterable_;
	positionChangedRegisterable_ = 0;
}

void UITargetModel::updateStateAndPosition()
{
	positionChangedRegisterable_->registerCallback();
}

void UITargetModel::positionChangedSync()
{
	if (!targetRenderer_->getTarget()) return;

	if (!modelInstance_.isCreated()) create();

	if (targetRenderer_->getTarget()->getAlive())
	{
		performUIActionAlive();
	}
	else 
	{
		performUIActionDead();
	}
}

void UITargetModel::performUIActionAlive()
{
	if (!modelInstance_.getModelEntity()->isVisible()) modelInstance_.getSceneNode()->setVisible(true);

	Vector position = targetRenderer_->getTarget()->getLife().getFloatPosition();
	position[0] *= OgreSystem::OGRE_WORLD_SCALE;
	position[1] *= OgreSystem::OGRE_WORLD_SCALE;
	position[2] *= OgreSystem::OGRE_WORLD_HEIGHT_SCALE;
	modelInstance_.getSceneNode()->setPosition(position[0], position[2], position[1]);
}

void UITargetModel::performUIActionDead()
{
	modelInstance_.getSceneNode()->setVisible(false);
}

void UITargetModel::create()
{
	modelInstance_.getModelEntity()->setVisibilityFlags(OgreSystem::VisibiltyMaskTargets);
}
