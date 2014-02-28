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

#include <models/ModelFactory.h>
#include <scorched3dc/OgreSystem.h>
#include <scorched3dc/ScorchedUI.h>

void ModelFactory::attachModel(Ogre::SceneNode *sceneNode, XMLEntryModelID &modelId)
{
	Ogre::SceneManager *sceneManager = ScorchedUI::instance()->getOgreSystem().getOgreLandscapeSceneManager();
	Ogre::Entity *modelEntity = sceneManager->createEntity(modelId.meshName.getValue());
	sceneNode->attachObject(modelEntity);
	float scale = modelId.scale.getValue().asFloat();
	if (scale != 1.0f) sceneNode->setScale(scale, scale, scale);
}
