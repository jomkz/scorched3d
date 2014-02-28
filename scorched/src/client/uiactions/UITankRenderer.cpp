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
#include <tanket/TanketShotPath.h>
#include <scorched3dc/ScorchedUI.h>
#include <scorched3dc/OgreSystem.h>
#include <client/ScorchedClient.h>
#include <OgreManualObject.h>
#include <OgreBillboardChain.h>

UITankRenderer::UITankRenderer(Tank *tank) :
	UITargetRenderer(tank), 
	gunBone_(0), turretBone_(0),
	active_(false)
{
	rotationChangedRegisterable_ = 
		new ClientUISyncActionRegisterableAdapter<UITankRenderer>(this, &UITankRenderer::rotationChangedSync, false);
}

UITankRenderer::~UITankRenderer()
{
	delete rotationChangedRegisterable_;
}

void UITankRenderer::setRotations()
{
	turretBone_->resetToInitialState();
	turretBone_->yaw(Ogre::Degree(-getShotHistory().getCurrentValues().rot.asFloat()), Ogre::Node::TS_WORLD);
	gunBone_->resetToInitialState();
	gunBone_->pitch(Ogre::Degree(getShotHistory().getCurrentValues().ele.asFloat()), Ogre::Node::TS_LOCAL);

	if (active_)
	{
		rotationChangedRegisterable_->registerCallback();
	}
}

void UITankRenderer::create()
{
	UITargetRenderer::create();

	Ogre::SceneManager *sceneManager = ScorchedUI::instance()->getOgreSystem().getOgreLandscapeSceneManager();

	std::string entityName = S3D::formatStringBuffer("ActiveTankMarker%u", target_->getPlayerId());
	std::string nodeName = S3D::formatStringBuffer("ActiveTankMarkerNode%u", target_->getPlayerId());
	activeTankMarkerEntity_ = sceneManager->createEntity(entityName.c_str(), "cone.mesh", "Models");
	activeTankMarkerEntity_->setVisibilityFlags(OgreSystem::VisibiltyMaskTargets);
	Ogre::SceneNode *activeTankMarkerNode = targetNode_->createChildSceneNode(nodeName);
	activeTankMarkerNode->attachObject(activeTankMarkerEntity_);
	activeTankMarkerNode->setInheritScale(false);
	activeTankMarkerNode->setPosition(0.0f, 2.0f, 0.0f);
	activeTankMarkerNode->setScale(30.0f, 30.0f, 30.0f);

	Ogre::SkeletonInstance* skel = targetEntity_->getSkeleton();
	gunBone_ = skel->getBone("Gun");
	gunBone_->setManuallyControlled(true);
	gunBone_->setInitialState();
	turretBone_ = skel->getBone("Turret");
	turretBone_->setManuallyControlled(true);
	turretBone_->setInitialState();
	
	setRotations();
}

void UITankRenderer::setActive(bool active)
{
	activeTankMarkerEntity_->setVisible(active);
	active_ = active;
	if (active_)
	{
		rotationChangedRegisterable_->registerCallback();
	}
}

void UITankRenderer::performUIActionAlive()
{
	UITargetRenderer::performUIActionAlive();

	Tank *tank = (Tank *) target_;
	fixed maxPower = tank->getShotInfo().getMaxOverallPower();

	getShotHistory().setMaxPower(maxPower);
	setRotations();
}

void UITankRenderer::rotationChangedSync()
{
	FixedVector result;
	TanketShotPath shotPath(ScorchedClient::instance()->getContext(), (Tanket *) target_, true);
	if (shotPath.makeShot(getShotHistory().getCurrentValues().rot, 
		getShotHistory().getCurrentValues().ele, 
		getShotHistory().getCurrentValues().power, 
		result))
	{
		Ogre::SceneManager *sceneManager = ScorchedUI::instance()->getOgreSystem().getOgreLandscapeSceneManager();
		Ogre::SceneNode *shotPathNode = sceneManager->getRootSceneNode()->createChildSceneNode();

		Ogre::BillboardChain *bbchain = sceneManager->createBillboardChain();
		
		//Ogre::ManualObject* manual = sceneManager->createManualObject();
		//manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);
		bbchain->setNumberOfChains(1);
		bbchain->setMaxChainElements(shotPath.getPositions().size());
		bbchain->setFaceCamera(true);

		std::vector<FixedVector>::iterator itor = shotPath.getPositions().begin();
		std::vector<FixedVector>::iterator end_itor = shotPath.getPositions().end();
		for (;itor!=end_itor;++itor)
		{
			Ogre::Vector3 position(
				OgreSystem::OGRE_WORLD_SCALE_FIXED * (*itor)[0].getInternalData(), 
				OgreSystem::OGRE_WORLD_HEIGHT_SCALE_FIXED * (*itor)[2].getInternalData(), 
				OgreSystem::OGRE_WORLD_SCALE_FIXED * (*itor)[1].getInternalData());
			Ogre::BillboardChain::Element element(position, 10, 0, Ogre::ColourValue(1, 0, 0), Ogre::Quaternion());
			bbchain->addChainElement(0, element);
		}

		//manual->end();
		shotPathNode->attachObject(bbchain);	
	}
}
