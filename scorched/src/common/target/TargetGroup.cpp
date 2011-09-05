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

bool TargetGroup::writeMessage(NamedNetBuffer &buffer)
{
	NamedNetBufferSection section(buffer, "TargetGroup");
	buffer.addToBufferNamed("size", (int) groups_.size());

	std::set<std::string> groupNames;
	{
		std::set<TargetGroupsSetEntry *>::iterator itor;
		for (itor = groups_.begin();
			itor != groups_.end();
			++itor)
		{
			TargetGroupsSetEntry *group = *itor;
			groupNames.insert(group->getName());
		}	
	}
	{
		std::set<std::string>::iterator itor;
		for (itor = groupNames.begin();
			itor != groupNames.end();
			++itor)
		{			
			buffer.addToBufferNamed("name", *itor);
		}
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
		++itor)
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
		++itor)
	{
		TargetGroupsSetEntry *group = *itor;
		if (group->hasObject(this))
		{
			group->removeObject(this);
		}
	}
}
