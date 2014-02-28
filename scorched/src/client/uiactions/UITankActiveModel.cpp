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

#include <uiactions/UITankActiveModel.h>
#include <uiactions/UITankRenderer.h>
#include <dialogs/GUITankInfo.h>
#include <weapons/Accessory.h>
#include <scorched3dc/OgreSystem.h>
#include <scorched3dc/ScorchedUI.h>
#include <client/ScorchedClient.h>
#include <tanket/TanketShotPath.h>
#include <OgreManualObject.h>

UITankActiveModel::UITankActiveModel(UITankRenderer *tankRenderer) :
	tankRenderer_(tankRenderer),
	shotPathNode_(0)
{
	rotationChangedRegisterable_ = 
		new ClientUISyncActionRegisterableAdapter<UITankActiveModel>(this, &UITankActiveModel::rotationChangedSync, false);
	rotationOrPowerChanged();

	Tank *tank = (Tank *) tankRenderer_->getTarget();
	GUITankInfo::instance()->setVisible(tank->getCStrName());
	weaponChanged();
}

UITankActiveModel::~UITankActiveModel()
{
	delete rotationChangedRegisterable_;
	rotationChangedRegisterable_ = 0;
	if (shotPathNode_) OgreSystem::destroySceneNode(shotPathNode_);
	shotPathNode_ = 0;
	GUITankInfo::instance()->setInvisible();
}

void UITankActiveModel::rotationOrPowerChanged()
{
	rotationChangedRegisterable_->registerCallback();
}

void UITankActiveModel::weaponChanged()
{
	Tank *tank = (Tank *) tankRenderer_->getTarget();
	if (!tank) return;

	tankRenderer_->getTankWeapon().setWeapons(tank);
	Accessory *currentWeapon = tankRenderer_->getTankWeapon().getCurrentWeapon();

	if (currentWeapon) GUITankInfo::instance()->setWeaponName(currentWeapon->getName());
	else GUITankInfo::instance()->setWeaponName("");

	/*
	if (!context.getServerMode() && tanket->getType() == Target::TypeTank)
	{
		Tank *tank = (Tank *) tanket;

		// Only show this information on this tanks client
		if (ScorchedClient::instance()->getTargetContainer().getCurrentDestinationId() ==
			tank->getDestinationId() &&
			ScorchedClient::instance()->getClientState().getState() == ClientState::StatePlaying)
		{

		// Turn off fuel display (if any)
		if (newWeapon && 
			newWeapon->getPositionSelect() == Accessory::ePositionSelectNone)
		{
			Landscape::instance()->restoreLandscapeTexture();
		}
		// Turn on selection display (if any)
		if (newWeapon &&
			newWeapon->getPositionSelect() != Accessory::ePositionSelectNone)
		{
			if (newWeapon->getPositionSelect() == Accessory::ePositionSelectFuel)
			{
				WeaponMoveTank *moveWeapon = (WeaponMoveTank *)
					context.getAccessoryStore().findAccessoryPartByAccessoryId(
						newWeapon->getAccessoryId(), "WeaponMoveTank");
				if (moveWeapon)
				{
					MovementMap mmap(
						tank, 
						context);
					mmap.calculateAllPositions(mmap.getFuel(moveWeapon));
					mmap.movementTexture();	
				}
			}
			else if (newWeapon->getPositionSelect() == Accessory::ePositionSelectFuelLimit)
			{
				MovementMap mmap(
					tank, 
					context);
				mmap.calculateAllPositions(fixed(newWeapon->getPositionSelectLimit()));
				mmap.movementTexture();	
			}
			else if (newWeapon->getPositionSelect() == Accessory::ePositionSelectLimit)
			{
				MovementMap::limitTexture(tank->getLife().getTargetPosition(), 
					newWeapon->getPositionSelectLimit());
			}

			ChannelText text("banner",
				LANG_RESOURCE_1(
					"GROUND_WEAPON_ACTIVATE", 
					"Click ground to activate {0}",
					newWeapon->getName()));
			ChannelManager::showText(ScorchedClient::instance()->getContext(), 
				text);
		}

		}
	}
	*/
}

void UITankActiveModel::rotationChangedSync()
{
	if (!tankRenderer_->getTarget()) return;

	FixedVector result;
	TanketShotPath shotPath(ScorchedClient::instance()->getContext(), (Tanket *) tankRenderer_->getTarget(), true);
	if (shotPath.makeShot(tankRenderer_->getShotHistory().getCurrentValues().rot, 
		tankRenderer_->getShotHistory().getCurrentValues().ele, 
		tankRenderer_->getShotHistory().getCurrentValues().power, 
		result))
	{
		Ogre::SceneManager *sceneManager = ScorchedUI::instance()->getOgreSystem().getOgreLandscapeSceneManager();
		Ogre::ManualObject* manual = sceneManager->createManualObject();
		manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);

		//Ogre::BillboardChain *bbchain = sceneManager->createBillboardChain();
		//bbchain->setNumberOfChains(1);
		//bbchain->setMaxChainElements(shotPath.getPositions().size());
		//bbchain->setFaceCamera(true);

		std::vector<FixedVector>::iterator itor = shotPath.getPositions().begin();
		std::vector<FixedVector>::iterator end_itor = shotPath.getPositions().end();
		for (;itor!=end_itor;++itor)
		{
			Ogre::Vector3 position(
				OgreSystem::OGRE_WORLD_SCALE_FIXED * (*itor)[0].getInternalData(), 
				OgreSystem::OGRE_WORLD_HEIGHT_SCALE_FIXED * (*itor)[2].getInternalData(), 
				OgreSystem::OGRE_WORLD_SCALE_FIXED * (*itor)[1].getInternalData());
			//Ogre::BillboardChain::Element element(position, 10, 0, Ogre::ColourValue(1, 0, 0), Ogre::Quaternion());
			//bbchain->addChainElement(0, element);
			manual->position(position);
		}

		manual->end();

		if (shotPathNode_) OgreSystem::destroySceneNode(shotPathNode_);
		shotPathNode_ = sceneManager->getRootSceneNode()->createChildSceneNode();
		shotPathNode_->attachObject(manual);
	}
}
