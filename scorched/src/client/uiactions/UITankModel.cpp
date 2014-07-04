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

#include <uiactions/UITankModel.h>
#include <uiactions/UITankRenderer.h>
#include <client/ScorchedClient.h>
#include <scorched3dc/OgreSystem.h>
#include <scorched3dc/ScorchedUI.h>
#include <target/TargetContainer.h>
#include <target/TargetLife.h>
#include <tanket/TanketShotInfo.h>
#include <tank/Tank.h>
#include <tank/TankModel.h>
#include <tank/TankModelContainer.h>
#include <common/Logger.h>

UITankModel::UITankModel(UITankRenderer *tankRenderer) :
	UITargetModel(tankRenderer),
	tankRenderer_(tankRenderer),
	turretBone_(0), gunBone_(0)
{
}

UITankModel::~UITankModel()
{

}

void UITankModel::performUIActionAlive()
{
	UITargetModel::performUIActionAlive();

	Tank *tank = (Tank *) targetRenderer_->getTarget();
	if (!tank) return;

	fixed maxPower = tank->getShotInfo().getMaxOverallPower();

	tankRenderer_->getShotHistory().setMaxPower(maxPower);
	setRotations();
}

void UITankModel::create()
{
	Tank *tank = (Tank *) targetRenderer_->getTarget();
	TankModel *model = tank->getModelContainer().getTankModel();

	modelInstance_.create(model->getTankModel());
	UITargetModel::create();

	Ogre::SkeletonInstance* skel = modelInstance_.getModelEntity()->getSkeleton();
	gunBone_ = skel->getBone("Gun");
	gunBone_->setManuallyControlled(true);
	gunBone_->setInitialState();
	turretBone_ = skel->getBone("Turret");
	turretBone_->setManuallyControlled(true);
	turretBone_->setInitialState();

	setRotations();
}

void UITankModel::setRotations()
{
	turretBone_->resetToInitialState();
	turretBone_->yaw(Ogre::Degree(-tankRenderer_->getShotHistory().getCurrentValues().rot.asFloat()), Ogre::Node::TS_WORLD);
	gunBone_->resetToInitialState();
	gunBone_->pitch(Ogre::Degree(tankRenderer_->getShotHistory().getCurrentValues().ele.asFloat()), Ogre::Node::TS_LOCAL);
}
