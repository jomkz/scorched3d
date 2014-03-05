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

#include <models/ModelInstance.h>
#include <scorched3dc/OgreSystem.h>
#include <scorched3dc/ScorchedUI.h>
#include <client/ScorchedClient.h>
#include <models/ModelStore.h>

ModelInstance::ModelInstance() :
	sceneNode_(0),
	pSys_(0)
{
}

ModelInstance::~ModelInstance()
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

void ModelInstance::create(XMLEntryModel &model)
{
	ENSURE_UI_THREAD
	fixed topLevelScale = 1;
	XMLEntryModelSpecDefinition *definition = 0;
	if (model.getChoiceType() == "reference")
	{
		XMLEntryModelSpecReference *reference = (XMLEntryModelSpecReference *) model.getValue();
		topLevelScale = reference->scale.getValue();
		definition = ScorchedClient::instance()->getModelStore().getModelDefinition(reference->modelName.getValue());
		if (!definition)
		{
			S3D::dialogExit("Scorched3D", S3D::formatStringBuffer(
				"Failed to find model reference %s", reference->modelName.getValue().c_str()));
		}
	}
	else if (model.getChoiceType() == "definition")
	{
		definition = (XMLEntryModelSpecDefinition *) model.getValue();
	}
	else if (model.getChoiceType() == "none")
	{
		definition = 0;
	}

	Ogre::SceneManager *sceneManager = ScorchedUI::instance()->getOgreSystem().getOgreLandscapeSceneManager();
	sceneNode_ = sceneManager->getRootSceneNode()->createChildSceneNode();
	if (definition == 0) return;

	Ogre::Entity *modelEntity = sceneManager->createEntity(definition->meshName.getValue());
	sceneNode_->attachObject(modelEntity);
	float scale = definition->scale.getValue().asFloat() * topLevelScale.asFloat() * OgreSystem::OGRE_WORLD_SCALE;
	if (scale != 1.0f) sceneNode_->setScale(scale, scale, scale);

	if (!definition->particles.getChildren().empty())
	{
		ParticleUniverse::ParticleSystemManager* pManager =
			ParticleUniverse::ParticleSystemManager::getSingletonPtr();

		pSys_.reserve(definition->particles.getChildren().size());
		std::list<XMLEntryModelParticle *>::iterator itor = definition->particles.getChildren().begin(),
			end = definition->particles.getChildren().end();
		for (; itor != end; ++itor)
		{
			static int particleNumber = 0;
			std::string particleName = S3D::formatStringBuffer("pr%u", ++particleNumber);

			ParticleUniverse::ParticleSystem *pSys =
				pManager->createParticleSystem(particleName, (*itor)->particle.particleName.getValue(), sceneManager);

			sceneNode_->attachObject(pSys);
			pSys->setScale(ParticleUniverse::Vector3(
				OgreSystem::OGRE_WORLD_SCALE,
				OgreSystem::OGRE_WORLD_SCALE,
				OgreSystem::OGRE_WORLD_SCALE));
			pSys->setScaleVelocity(
				OgreSystem::OGRE_WORLD_SCALE);
			pSys->start();
			pSys_.push_back(pSys);
		}
	}
}
