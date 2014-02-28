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
	sceneNode_(0)
{
}

ModelInstance::~ModelInstance()
{
	ENSURE_UI_THREAD
	if (sceneNode_)
	{
		OgreSystem::destroySceneNode(sceneNode_);
		sceneNode_ = 0;
	}
}

void ModelInstance::create(XMLEntryModel &model)
{
	ENSURE_UI_THREAD
	XMLEntryModelSpecDefinition *definition = 0;
	if (model.getChoiceType() == "reference")
	{
		XMLEntryModelSpecReference *reference = (XMLEntryModelSpecReference *) model.getValue();
		definition = ScorchedClient::instance()->getModelStore().getModelDefinition(reference->modelName.getValue());
		if (!definition)
		{
			S3D::dialogExit("Scorched3D", S3D::formatStringBuffer(
				"Failed to find model reference %s", reference->modelName.getValue().c_str()));
		}
	}
	else
	{
		definition = (XMLEntryModelSpecDefinition *) model.getValue();
	}

	Ogre::SceneManager *sceneManager = ScorchedUI::instance()->getOgreSystem().getOgreLandscapeSceneManager();
	sceneNode_ = sceneManager->getRootSceneNode()->createChildSceneNode();

	Ogre::Entity *modelEntity = sceneManager->createEntity(definition->meshName.getValue());
	sceneNode_->attachObject(modelEntity);
	float scale = definition->scale.getValue().asFloat();
	if (scale != 1.0f) sceneNode_->setScale(scale, scale, scale);
}
