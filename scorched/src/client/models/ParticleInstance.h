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

#if !defined(AFX_ParticleInstance_H__516D85F7_420B_43EB_B0BE_563DCBE1B143__INCLUDED_)
#define AFX_ParticleInstance_H__516D85F7_420B_43EB_B0BE_563DCBE1B143__INCLUDED_

#include <XML/XMLEntryComplexTypes.h>
#include <ParticleUniverse/ParticleUniverseSystem.h>
#include <ParticleUniverse/ParticleUniverseSystemListener.h>

class ParticleInstance;
class ParticleInstanceI
{
public:
	ParticleInstanceI() {}
	virtual ~ParticleInstanceI() {}

	virtual void finished(ParticleInstance *instance) = 0;
};

class ParticleInstance : public ParticleUniverse::ParticleSystemListener
{
public:
	ParticleInstance();
	virtual ~ParticleInstance();

	void create(XMLEntryParticleIDList &particles);
	Ogre::SceneNode *getSceneNode() { return sceneNode_;  }

	bool isCreated() { return sceneNode_ != 0; }
	void setListener(ParticleInstanceI *listener) { listener_ = listener;  }

	// ParticleUniverse::ParticleSystemListener
	virtual void handleParticleSystemEvent(ParticleUniverse::ParticleSystem* particleSystem,
		ParticleUniverse::ParticleUniverseEvent& particleUniverseEvent);
protected:
	ParticleInstanceI *listener_;
	int runningParticles_;
	Ogre::SceneNode *sceneNode_;
	std::vector<ParticleUniverse::ParticleSystem *> pSys_;
};

#endif // !defined(AFX_ParticleInstance_H__516D85F7_420B_43EB_B0BE_563DCBE1B143__INCLUDED_)
