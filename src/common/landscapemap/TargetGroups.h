////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
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
//    You should have received a copy of the GNU General Public License
//    along with Scorched3D; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

#if !defined(__INCLUDE_TargetGroupsh_INCLUDE__)
#define __INCLUDE_TargetGroupsh_INCLUDE__

#include <landscapemap/TargetGroupsGroupEntry.h>
#include <target/TargetGroup.h>
#include <placement/PlacementShadowDefinition.h>
#include <map>
#include <list>
#include <string>

class LandscapePlace;
class HeightMap;
class TargetGroups
{
public:
	TargetGroups();
	virtual ~TargetGroups();

	// Groups
	void removeFromGroups(TargetGroup *obj);
	TargetGroupsGroupEntry *getGroup(const char *name, HeightMap *create = 0);
	void clearGroups();

	std::map<std::string, TargetGroupsGroupEntry*> &getGroups() { return groups_; }

	// Accessors
	std::list<PlacementShadowDefinition::Entry> &getShadows() { return shadows_; }

protected:
	std::map<std::string, TargetGroupsGroupEntry*> groups_;
	std::list<PlacementShadowDefinition::Entry> shadows_;

};

#endif
