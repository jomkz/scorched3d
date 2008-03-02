////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
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

#include <placement/PlacementGroupDefinition.h>
#include <engine/ScorchedContext.h>
#include <landscapemap/LandscapeMaps.h>

PlacementGroupDefinition::PlacementGroupDefinition()
{
}

PlacementGroupDefinition::~PlacementGroupDefinition()
{
}

bool PlacementGroupDefinition::readXML(XMLNode *node)
{
	std::string groupname;
	while (node->getNamedChild("groupname", groupname, false))
	{
		groupnames_.push_back(groupname);
	}
	return true;
}

void PlacementGroupDefinition::addToGroups(
	ScorchedContext &context,
	TargetGroup *objectEntry,
	bool thin)
{
	for (unsigned int i=0; i<groupnames_.size(); i++)
	{
		std::string groupname = groupnames_[i];
		addToGroup(groupname.c_str(), context, objectEntry, thin);
	}
}

void PlacementGroupDefinition::addToGroup(
	const char *groupName,
	ScorchedContext &context,
	TargetGroup *objectEntry,
	bool thin)
{
	TargetGroupsGroupEntry *group =
		context.landscapeMaps->getGroundMaps().getGroups().getGroup(
			groupName, 
			&context.landscapeMaps->getGroundMaps().getHeightMap());
	if (group)
	{
		group->addObject(objectEntry, thin);
		objectEntry->getGroups().push_back(group);
	}
}
