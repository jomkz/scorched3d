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

#include <engine/ObjectGroupEntry.h>
#include <engine/ObjectGroups.h>
#include <common/Defines.h>
#include <stdio.h>

ObjectGroups::ObjectGroups()
{
}

ObjectGroups::~ObjectGroups()
{
}

void ObjectGroups::clearGroups()
{
	// Groups
	std::map<std::string, ObjectGroup*>::iterator itor;
	for (itor = groups_.begin();
		itor != groups_.end();
		++itor)
	{
		ObjectGroup *entry = (*itor).second;
		delete entry;
	}
	groups_.clear();
}

ObjectGroup *ObjectGroups::getGroup(const char *name, bool create)
{
	std::map<std::string, ObjectGroup*>::iterator findItor = groups_.find(name);
	if (findItor != groups_.end())
	{
		return (*findItor).second;
	}
	if (create)
	{
		ObjectGroup *entry = new ObjectGroup(name);
		groups_[name] = entry;
		return entry;
	}
	return 0;
}
