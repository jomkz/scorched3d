////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011
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

#include <landscapemap/TargetGroups.h>
#include <landscapemap/LandscapeMaps.h>
#include <common/Defines.h>
#include <stdio.h>

TargetGroups::TargetGroups()
{
}

TargetGroups::~TargetGroups()
{
}

void TargetGroups::clearGroups()
{
	// Groups
	std::map<std::string, TargetGroupsGroupEntry*>::iterator itor;
	for (itor = groups_.begin();
		itor != groups_.end();
		itor++)
	{
		TargetGroupsGroupEntry *entry = (*itor).second;
		delete entry;
	}
	groups_.clear();
}

TargetGroupsGroupEntry *TargetGroups::getGroup(
	const char *name, HeightMap *create)
{
	std::map<std::string, TargetGroupsGroupEntry*>::iterator findItor =
		groups_.find(name);
	if (findItor != groups_.end())
	{
		return (*findItor).second;
	}
	if (create)
	{
		TargetGroupsGroupEntry *entry = new TargetGroupsGroupEntry(name, *create);
		groups_[name] = entry;
		return entry;
	}
	return 0;
}
