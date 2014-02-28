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

#include <landscapedef/LandscapeEventCondition.h>
#include <engine/ObjectGroups.h>
#include <engine/ObjectGroupEntry.h>
#include <engine/Simulator.h>

LandscapeEventConditionChoice::LandscapeEventConditionChoice(const char *name, const char *description) :
	XMLEntryTypeChoice<LandscapeEventCondition>(name, description)
{
}

LandscapeEventConditionChoice::~LandscapeEventConditionChoice()
{
}

LandscapeEventCondition *LandscapeEventConditionChoice::createXMLEntry(const std::string &type)
{
	if (0 == strcmp(type.c_str(), "time")) return new LandscapeEventConditionTime;
	if (0 == strcmp(type.c_str(), "groupsize")) return new LandscapeEventConditionGroupSize;
	S3D::dialogMessage("LandscapeEventCondition", S3D::formatStringBuffer("Unknown event condition type %s", type));
	return 0;
}

LandscapeEventCondition::LandscapeEventCondition(const char *name, const char *description) :
	XMLEntryContainer(name, description)
{
}

LandscapeEventCondition::~LandscapeEventCondition()
{
}

// LandscapeEventConditionGroupSize
LandscapeEventConditionGroupSize::LandscapeEventConditionGroupSize() :
	LandscapeEventCondition("LandscapeEventConditionGroupSize", 
		"Fires an event when the number of objects in the specified group falls below a given number"),
	groupsize("groupsize", "The size threshold that will fire the event."),
	groupname("groupname", "The name of the group to check the size for.")
{
	addChildXMLEntry(&groupsize, &groupname);
}

LandscapeEventConditionGroupSize::~LandscapeEventConditionGroupSize()
{
}

fixed LandscapeEventConditionGroupSize::getNextEventTime(ScorchedContext &context, int eventNumber)
{
	return fixed::MAX_FIXED;
}

bool LandscapeEventConditionGroupSize::fireEvent(ScorchedContext &context, 
	fixed timeLeft, int eventNumber)
{
	if (eventNumber == 1) // i.e. the first event
	{
		ObjectGroup *objectGroup =
			context.getObjectGroups().getGroup(groupname.getValue().c_str());
		if (objectGroup)
		{
			int groupCount = objectGroup->getObjectCount();
			if (groupCount <= groupsize.getValue()) return true;
		}
	}

	return false;
}

// LandscapeEventConditionTime
LandscapeEventConditionTime::LandscapeEventConditionTime() :
	LandscapeEventCondition("LandscapeEventConditionTime", 
		"Fires an event during the specified time interval."
		"  The interval is defined as a minimum and maximum value."
		"  The event will be fired at some point during the specified interval."),
	mintime("mintime", "The minimum amount of time that must pass before the event is fired."),
	maxtime("maxtime", "The maximum amount of time that can pass before the event is fired."),
	singletimeonly("singletimeonly", "If this event only fires once (at the start) or continues to fire.")
{
	addChildXMLEntry(&mintime, &maxtime, &singletimeonly);
}

LandscapeEventConditionTime::~LandscapeEventConditionTime()
{
}

fixed LandscapeEventConditionTime::getNextEventTime(ScorchedContext &context, int eventNumber)
{
	if (eventNumber > 1 &&
		singletimeonly.getValue())
	{
		return fixed::MAX_FIXED;
	}

	return context.getSimulator().getRandomGenerator().getRandFixed("LandscapeEvents") * 
		(maxtime.getValue() - mintime.getValue()) + mintime.getValue();
}

bool LandscapeEventConditionTime::fireEvent(ScorchedContext &context, 
	fixed timeLeft, int eventNumber)
{
	return (timeLeft < fixed(0));
}
