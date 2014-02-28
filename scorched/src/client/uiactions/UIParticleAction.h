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

#if !defined(__INCLUDE_UIParticleActionh_INCLUDE__)
#define __INCLUDE_UIParticleActionh_INCLUDE__

#include <common/FixedVector.h>
#include <client/ClientUISync.h>
#include <ParticleUniverse/ParticleUniverseSystem.h>
#include <ParticleUniverse/ParticleUniverseSystemListener.h>

class UIParticleAction : public ClientUISyncAction, public ParticleUniverse::ParticleSystemListener
{
public:
	UIParticleAction(FixedVector &position, const std::string &particleScriptName);
	virtual ~UIParticleAction();

	// ClientUISyncAction
	virtual void performUIAction();

	// ParticleUniverse::ParticleSystemListener
	virtual void handleParticleSystemEvent(ParticleUniverse::ParticleSystem* particleSystem, 
		ParticleUniverse::ParticleUniverseEvent& particleUniverseEvent);
protected:
	static unsigned int particleNumber_;
	FixedVector position_;
	bool destroy_;
	std::string particleScriptName_;
	Ogre::SceneNode *particleNode_;
	ParticleUniverse::ParticleSystem* pSys_;

	void create();
};

#endif // __INCLUDE_UIParticleActionh_INCLUDE__
