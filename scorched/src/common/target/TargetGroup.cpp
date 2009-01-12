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

#include <target/TargetGroup.h>
#include <target/Target.h>
#include <target/TargetLife.h>
#include <engine/ScorchedContext.h>
#include <landscapemap/TargetGroupsSetEntry.h>
#include <landscapemap/LandscapeMaps.h>
#include <net/NetBuffer.h>
#include <set>

TargetGroup::TargetGroup(ScorchedContext &context) :
	context_(context)
{
}

TargetGroup::~TargetGroup()
{
}

FixedVector &TargetGroup::getPosition()
{
	return target_->getLife().getTargetPosition();
}

bool TargetGroup::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer((int) groups_.size());
	std::set<TargetGroupsSetEntry *>::iterator itor;
	for (itor = groups_.begin();
		itor != groups_.end();
		itor++)
	{
		TargetGroupsSetEntry *group = *itor;
		buffer.addToBuffer(group->getName());
	}	
	return true;
}

bool TargetGroup::readMessage(NetBufferReader &reader)
{
	std::set<std::string> goodGroups;

	int groupNo = 0;
	if (!reader.getFromBuffer(groupNo)) return false;
	for (int g=0; g<groupNo; g++)
	{
		std::string groupName;
		if (!reader.getFromBuffer(groupName)) return false;

		TargetGroupsSetEntry *group = 
			context_.getLandscapeMaps().getGroundMaps().getGroups().getGroup(groupName.c_str());
		if (group && !group->hasObject(this))
		{
			group->addObject(this, false);
		}
		goodGroups.insert(groupName);
	}

	std::set<TargetGroupsSetEntry *> groupsCopy = groups_;
	std::set<TargetGroupsSetEntry *>::iterator itor;
	for (itor = groupsCopy.begin();
		itor != groupsCopy.end();
		itor++)
	{
		TargetGroupsSetEntry *group = *itor;
		if (goodGroups.find(group->getName()) == goodGroups.end())
		{
			if (group->hasObject(this))
			{
				group->removeObject(this);
			}
		}
	}

	return true;
}

void TargetGroup::addToGroup(TargetGroupsSetEntry *group)
{
	groups_.insert(group);
}

void TargetGroup::removeFromGroup(TargetGroupsSetEntry *group)
{
	groups_.erase(group);
}

void TargetGroup::removeFromAllGroups()
{
	if (groups_.empty()) return;

	std::set<TargetGroupsSetEntry *> groupsCopy = groups_;
	std::set<TargetGroupsSetEntry *>::iterator itor;
	for (itor = groupsCopy.begin();
		itor != groupsCopy.end();
		itor++)
	{
		TargetGroupsSetEntry *group = *itor;
		if (group->hasObject(this))
		{
			group->removeObject(this);
		}
	}
}
