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

#if !defined(__INCLUDE_TargetGrouph_INCLUDE__)
#define __INCLUDE_TargetGrouph_INCLUDE__

#include <common/FixedVector.h>
#include <set>

class TargetGroupsSetEntry;
class ScorchedContext;
class NamedNetBuffer;
class NetBufferReader;
class Target;
class TargetGroup
{
public:
	TargetGroup(ScorchedContext &context);
	virtual ~TargetGroup();

	void setTarget(Target *target) { target_ = target; }
	Target *getTarget() { return target_; }

	std::set<TargetGroupsSetEntry *> &getAllGroups() { return groups_; }
	FixedVector &getPosition();

	void removeFromAllGroups();

	virtual bool writeMessage(NamedNetBuffer &buffer);
	virtual bool readMessage(NetBufferReader &reader);

private:
	friend class TargetGroupsSetEntry;

	void addToGroup(TargetGroupsSetEntry *group);
	void removeFromGroup(TargetGroupsSetEntry *group);

protected:
	ScorchedContext &context_;
	Target *target_;
	std::set<TargetGroupsSetEntry *> groups_;
};

#endif // __INCLUDE_TargetGrouph_INCLUDE__
