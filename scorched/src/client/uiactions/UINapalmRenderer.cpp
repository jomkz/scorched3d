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

#include <uiactions/UINapalmRenderer.h>
#include <scorched3dc/OgreSystem.h>
#include <scorched3dc/ScorchedUI.h>
#include <actions/Napalm.h>

UINapalmRenderer::UINapalmRenderer(Napalm *napalm) :
	ClientUISyncActionRegisterable(true),
	napalm_(napalm)
{
}

UINapalmRenderer::~UINapalmRenderer()
{
	ENSURE_UI_THREAD

	std::map<Napalm::NapalmEntry *, UINapalmEntry *>::iterator itor = current_.begin();
	std::map<Napalm::NapalmEntry *, UINapalmEntry *>::iterator end_itor = current_.end();
	for (;itor!=end_itor;++itor)
	{
		destroy(itor->second);
	}
	current_.clear();
}

void UINapalmRenderer::performUIAction()
{
	ClientUISyncActionRegisterable::performUIAction();
	if (!napalm_) 
	{
		delete this;
		return;
	}

	{
		std::set<Napalm::NapalmEntry *>::iterator itor = added_.begin();
		std::set<Napalm::NapalmEntry *>::iterator end_itor = added_.end();
		for (;itor!=end_itor;++itor)
		{
			create(*itor);
		}
		added_.clear();
	}
	{
		std::set<Napalm::NapalmEntry *>::iterator itor = removed_.begin();
		std::set<Napalm::NapalmEntry *>::iterator end_itor = removed_.end();
		for (;itor!=end_itor;++itor)
		{
			destroy(*itor);
		}
		removed_.clear();
	}
}

void UINapalmRenderer::simulate(Action *action, float frametime, bool &removeAction)
{
	registerCallback();
}

void UINapalmRenderer::deleteThis()
{
	napalm_ = 0;
	registerCallback();
}

void UINapalmRenderer::create(Napalm::NapalmEntry *entry)
{
	Ogre::SceneManager *sceneManager = ScorchedUI::instance()->getOgreSystem().getOgreLandscapeSceneManager();
	UINapalmEntry *uiEntry = new UINapalmEntry();
	uiEntry->node = sceneManager->getRootSceneNode()->createChildSceneNode();
	uiEntry->node->setPosition(
		entry->posX * OgreSystem::OGRE_WORLD_SCALE, 
		entry->posZ.getInternalData() * OgreSystem::OGRE_WORLD_HEIGHT_SCALE_FIXED, 
		entry->posY * OgreSystem::OGRE_WORLD_SCALE);

	ParticleUniverse::ParticleSystemManager* pManager =
		ParticleUniverse::ParticleSystemManager::getSingletonPtr();
	static int particleNumber = 0;
	std::string particleName = S3D::formatStringBuffer("pn%u", ++particleNumber);
	uiEntry->pSys = pManager->createParticleSystem(particleName, "mp2_fire_04", sceneManager);
	
	uiEntry->node->attachObject(uiEntry->pSys);
	uiEntry->pSys->setScale(ParticleUniverse::Vector3(
		OgreSystem::OGRE_WORLD_SCALE,
		OgreSystem::OGRE_WORLD_SCALE,
		OgreSystem::OGRE_WORLD_SCALE));
	uiEntry->pSys->setScaleVelocity(
		OgreSystem::OGRE_WORLD_SCALE);
	uiEntry->pSys->start();

	current_.insert(std::pair<Napalm::NapalmEntry *, UINapalmEntry *>(entry, uiEntry));
}

void UINapalmRenderer::destroy(Napalm::NapalmEntry *entry)
{
	std::map<Napalm::NapalmEntry *, UINapalmEntry *>::iterator itor = current_.find(entry);
	if (itor == current_.end()) return;
	destroy(itor->second);
	current_.erase(itor);
}

void UINapalmRenderer::destroy(UINapalmEntry *entry)
{
	entry->pSys->stop();
	entry->node->detachObject(entry->pSys);
	Ogre::SceneManager *sceneManager = 
		ScorchedUI::instance()->getOgreSystem().getOgreLandscapeSceneManager();
	ParticleUniverse::ParticleSystemManager* pManager =
		ParticleUniverse::ParticleSystemManager::getSingletonPtr();
	pManager->destroyParticleSystem(entry->pSys, sceneManager);

	OgreSystem::destroySceneNode(entry->node);
	delete entry;
}

void UINapalmRenderer::addEntry(Napalm::NapalmEntry *entry)
{
	added_.insert(entry);
}

void UINapalmRenderer::removeEntry(Napalm::NapalmEntry *entry)
{
	added_.erase(entry);
	removed_.insert(entry);
}
