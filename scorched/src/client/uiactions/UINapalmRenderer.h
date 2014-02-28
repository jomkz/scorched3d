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

#if !defined(__INCLUDE_UINapalmRendererh_INCLUDE__)
#define __INCLUDE_UINapalmRendererh_INCLUDE__

#include <client/ClientUISync.h>
#include <actions/Napalm.h>
#include <set>

class UINapalmRenderer : public ClientUISyncActionRegisterable, public ActionRenderer
{
public:
	UINapalmRenderer(Napalm *napalm);
	virtual ~UINapalmRenderer();

	void addEntry(Napalm::NapalmEntry *entry);
	void removeEntry(Napalm::NapalmEntry *entry);

	// ClientUISyncAction (UI and Client Thread)
	virtual void performUIAction();

	// ActionRenderer
	virtual void simulate(Action *action, float frametime, bool &removeAction);
	virtual void deleteThis();
protected:
	struct UINapalmEntry
	{
		Ogre::SceneNode *node;
		ParticleUniverse::ParticleSystem* pSys;
	};
	Napalm *napalm_;
	std::set<Napalm::NapalmEntry *> added_, removed_;
	std::map<Napalm::NapalmEntry *, UINapalmEntry *> current_;

	void create(Napalm::NapalmEntry *entry);
	void destroy(Napalm::NapalmEntry *entry);
	void destroy(UINapalmEntry *entry);
};

#endif
