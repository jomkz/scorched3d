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
#include <landscapemap/TargetGroupsGroupEntry.h>
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
	std::set<std::string> names;
	std::vector<TargetGroupsGroupEntry *>::iterator itor;
	for (itor = groups.begin();
		itor != groups.end();
		itor++)
	{
		TargetGroupsGroupEntry *group = *itor;
		names.insert(group->getName());
	}
	buffer.addToBuffer((int) names.size());
	std::set<std::string>::iterator itor2;
	for (itor2 = names.begin();
		itor2 != names.end();
		itor2++)
	{
		std::string name = *itor2;
		buffer.addToBuffer(name);
	}
	
	return true;
}

bool TargetGroup::readMessage(NetBufferReader &reader)
{
	int groupNo = 0;
	if (!reader.getFromBuffer(groupNo)) return false;
	for (int g=0; g<groupNo; g++)
	{
		std::string groupName;
		if (!reader.getFromBuffer(groupName)) return false;

		TargetGroupsGroupEntry *group = 
			context_.getLandscapeMaps().getGroundMaps().getGroups().getGroup(groupName.c_str());
		if (group && !group->hasObject(this))
		{
			group->addObject(this, false);
		}
	}

	return true;
}
