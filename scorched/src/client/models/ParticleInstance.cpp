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

#include <models/ParticleInstance.h>
#include <scorched3dc/OgreSystem.h>
#include <scorched3dc/ScorchedUI.h>
#include <client/ScorchedClient.h>
#include <models/ModelStore.h>

ParticleInstance::ParticleInstance() :
	sceneNode_(0),
	pSys_(0),
	listener_(0)
{
}

ParticleInstance::~ParticleInstance()
{
	ENSURE_UI_THREAD
	if (!pSys_.empty())
	{
		Ogre::SceneManager *sceneManager =
			ScorchedUI::instance()->getOgreSystem().getOgreLandscapeSceneManager();
		ParticleUniverse::ParticleSystemManager* pManager =
			ParticleUniverse::ParticleSystemManager::getSingletonPtr();

		std::vector<ParticleUniverse::ParticleSystem *>::iterator itor = pSys_.begin(),
			end = pSys_.end();
		for (; itor != end; ++itor)
		{
			ParticleUniverse::ParticleSystem *pSys = *itor;
			pSys->stop();
			sceneNode_->detachObject(pSys);
			pManager->destroyParticleSystem(pSys, sceneManager);
		}
		pSys_.clear();
	}
	if (sceneNode_)
	{
		OgreSystem::destroySceneNode(sceneNode_);
		sceneNode_ = 0;
	}
}

void ParticleInstance::create(XMLEntryParticleIDList &particles)
{
	ENSURE_UI_THREAD

	Ogre::SceneManager *sceneManager = ScorchedUI::instance()->getOgreSystem().getOgreLandscapeSceneManager();
	sceneNode_ = sceneManager->getRootSceneNode()->createChildSceneNode();

	if (!particles.getChildren().empty())
	{
		ParticleUniverse::ParticleSystemManager* pManager =
			ParticleUniverse::ParticleSystemManager::getSingletonPtr();

		pSys_.reserve(particles.getChildren().size());
		std::list<XMLEntryParticleID *>::iterator itor = particles.getChildren().begin(),
			end = particles.getChildren().end();
		for (; itor != end; ++itor)
		{
			static int particleNumber = 0;
			std::string particleName = S3D::formatStringBuffer("pr%u", ++particleNumber);

			ParticleUniverse::ParticleSystem *pSys =
				pManager->createParticleSystem(particleName, (*itor)->particleName.getValue(), sceneManager);

			runningParticles_++;
			sceneNode_->attachObject(pSys);
			pSys->setScale(ParticleUniverse::Vector3(
				OgreSystem::OGRE_WORLD_SCALE,
				OgreSystem::OGRE_WORLD_SCALE,
				OgreSystem::OGRE_WORLD_SCALE));
			pSys->setScaleVelocity(
				OgreSystem::OGRE_WORLD_SCALE);
			pSys_.push_back(pSys);
			pSys->addParticleSystemListener(this);
			pSys->startAndStopFade(120);
		}
	}
}

void ParticleInstance::handleParticleSystemEvent(ParticleUniverse::ParticleSystem* particleSystem,
	ParticleUniverse::ParticleUniverseEvent& particleUniverseEvent)
{
	switch (particleUniverseEvent.eventType)
	{
	case ParticleUniverse::PU_EVT_SYSTEM_STOPPED:
		DIALOG_ASSERT(isCreated());
		if (--runningParticles_ == 0)
		{
			if (listener_) listener_->finished(this);
		}
		break;
	}
}
