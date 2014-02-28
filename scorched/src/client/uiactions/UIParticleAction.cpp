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

#include <uiactions/UIParticleAction.h>
#include <scorched3dc/ScorchedUI.h>
#include <scorched3dc/OgreSystem.h>
#include <client/ScorchedClient.h>

unsigned int UIParticleAction::particleNumber_(0);

UIParticleAction::UIParticleAction(FixedVector &position, const std::string &particleScriptName) :
	position_(position),
	particleScriptName_(particleScriptName),
	particleNode_(0), pSys_(0), destroy_(false)
{
	ENSURE_CLIENT_THREAD
}

UIParticleAction::~UIParticleAction()
{
	ENSURE_UI_THREAD
	pSys_->stop();
	particleNode_->detachObject(pSys_);
	Ogre::SceneManager *sceneManager = 
		ScorchedUI::instance()->getOgreSystem().getOgreLandscapeSceneManager();
	ParticleUniverse::ParticleSystemManager* pManager =
		ParticleUniverse::ParticleSystemManager::getSingletonPtr();
	pManager->destroyParticleSystem(pSys_, sceneManager);
	pSys_ = 0;	
	OgreSystem::destroySceneNode(particleNode_);
	particleNode_ = 0;

}

void UIParticleAction::performUIAction()
{
	if (!destroy_) create();
	else delete this;
}

void UIParticleAction::create()
{
	std::string particleName = S3D::formatStringBuffer("p%u", ++particleNumber_);

	Ogre::SceneManager *sceneManager = 
		ScorchedUI::instance()->getOgreSystem().getOgreLandscapeSceneManager();
	ParticleUniverse::ParticleSystemManager* pManager =
		ParticleUniverse::ParticleSystemManager::getSingletonPtr();

	pSys_ = pManager->createParticleSystem(particleName, particleScriptName_, sceneManager);

	particleNode_ = sceneManager->getRootSceneNode()->createChildSceneNode();
	particleNode_->setPosition(
		position_[0].getInternalData() * OgreSystem::OGRE_WORLD_SCALE_FIXED,
		position_[2].getInternalData() * OgreSystem::OGRE_WORLD_HEIGHT_SCALE_FIXED + 10.0f,
		position_[1].getInternalData() * OgreSystem::OGRE_WORLD_SCALE_FIXED);
	particleNode_->attachObject(pSys_);
	
	pSys_->setScale(ParticleUniverse::Vector3(
		OgreSystem::OGRE_WORLD_SCALE,
		OgreSystem::OGRE_WORLD_SCALE,
		OgreSystem::OGRE_WORLD_SCALE));
	pSys_->setScaleVelocity(OgreSystem::OGRE_WORLD_SCALE);
	pSys_->addParticleSystemListener(this);
	pSys_->start(10.0);
}

void UIParticleAction::handleParticleSystemEvent(ParticleUniverse::ParticleSystem* particleSystem, 
	ParticleUniverse::ParticleUniverseEvent& particleUniverseEvent)
{
	switch(particleUniverseEvent.eventType)
	{
	case ParticleUniverse::PU_EVT_SYSTEM_STOPPED:
		DIALOG_ASSERT(!destroy_);
		destroy_ = true;
		ScorchedClient::getClientUISyncExternal().addActionFromUI(this);
		break;
	}
}
