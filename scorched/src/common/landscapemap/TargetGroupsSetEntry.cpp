////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2009
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

#include <landscapemap/TargetGroupsSetEntry.h>
#include <target/Target.h>
#include <common/Defines.h>

TargetGroupsSetEntry::TargetGroupsSetEntry(const char *name) :
	name_(name)
{
}

TargetGroupsSetEntry::~TargetGroupsSetEntry()
{
}

void TargetGroupsSetEntry::addObject(TargetGroup *object, bool thin)
{
	object->addToGroup(this);
	objects_[object->getTarget()->getPlayerId()] = object;
}

bool TargetGroupsSetEntry::removeObject(TargetGroup *object)
{
	object->removeFromGroup(this);
	return (objects_.erase(object->getTarget()->getPlayerId()) > 0);
}

TargetGroup *TargetGroupsSetEntry::getObjectById(unsigned int playerId)
{
	std::map<unsigned int, TargetGroup *>::iterator itor =
		objects_.find(playerId);
	if (itor == objects_.end()) return 0;
	return (*itor).second;
}

TargetGroup *TargetGroupsSetEntry::getObjectByPos(int position)
{
	int pos = position % int(objects_.size());
	std::map<unsigned int, TargetGroup *>::iterator itor;
	for (itor = objects_.begin();
		itor != objects_.end();
		itor ++, pos--)
	{
		if (pos <=0) return (*itor).second;
	}
	return 0;
}

bool TargetGroupsSetEntry::hasObject(TargetGroup *object)
{
	return (objects_.find(object->getTarget()->getPlayerId()) != objects_.end());
}

bool TargetGroupsSetEntry::hasObjectById(unsigned int playerId)
{
	return (objects_.find(playerId) != objects_.end());
}

int TargetGroupsSetEntry::getObjectCount()
{ 
	return (int) objects_.size(); 
}
