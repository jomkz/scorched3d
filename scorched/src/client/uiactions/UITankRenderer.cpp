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

#include <uiactions/UITankRenderer.h>
#include <tanket/TanketShotInfo.h>

UITankRenderer::UITankRenderer(Tank *tank) :
	UITargetRenderer(tank), 
		gunBone_(0), turretBone_(0)
{
}

UITankRenderer::~UITankRenderer()
{

}

void UITankRenderer::setRotations()
{
	turretBone_->resetToInitialState();
	turretBone_->yaw(Ogre::Degree(getShotHistory().getCurrentValues().rot.asFloat()), Ogre::Node::TS_WORLD);
	gunBone_->resetToInitialState();
	gunBone_->pitch(Ogre::Degree(getShotHistory().getCurrentValues().ele.asFloat()), Ogre::Node::TS_LOCAL);
}

void UITankRenderer::create()
{
	UITargetRenderer::create();

	Ogre::SkeletonInstance* skel = targetEntity_->getSkeleton();
	gunBone_ = skel->getBone("Gun");
	gunBone_->setManuallyControlled(true);
	gunBone_->setInitialState();
	turretBone_ = skel->getBone("Turret");
	turretBone_->setManuallyControlled(true);
	turretBone_->setInitialState();
	
	setRotations();
}

void UITankRenderer::performUIActionAlive()
{
	UITargetRenderer::performUIActionAlive();

	Tank *tank = (Tank *) target_;
	fixed maxPower = tank->getShotInfo().getMaxOverallPower();

	getShotHistory().setMaxPower(maxPower);
	setRotations();
}
