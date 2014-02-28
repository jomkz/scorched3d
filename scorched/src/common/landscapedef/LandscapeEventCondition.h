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

#if !defined(__INCLUDE_LandscapeEventConditionsh_INCLUDE__)
#define __INCLUDE_LandscapeEventConditionsh_INCLUDE__

#include <common/fixed.h>
#include <XML/XMLEntrySimpleTypes.h>

class LandscapeEventConditionFactory : public XMLEntryFactory
{
public:
	static LandscapeEventConditionFactory *instance;

	// XMLEntryFactory
	virtual XMLEntry *createXMLEntry(const std::string &type);
};

class ScorchedContext;
class LandscapeEventCondition : public XMLEntryNamedContainer
{
public:
	LandscapeEventCondition(const std::string &name, const std::string &description);
	virtual ~LandscapeEventCondition();

	virtual fixed getNextEventTime(ScorchedContext &context, 
		int eventNumber) = 0;
	virtual bool fireEvent(ScorchedContext &context, 
		fixed timeLeft, int eventNumber) = 0;
};

class LandscapeEventConditionGroupSize : public LandscapeEventCondition
{
public:
	LandscapeEventConditionGroupSize();
	virtual ~LandscapeEventConditionGroupSize();

	XMLEntryInt groupsize;
	XMLEntryString groupname;

	virtual fixed getNextEventTime(ScorchedContext &context, 
		int eventNumber);
	virtual bool fireEvent(ScorchedContext &context, 
		fixed timeLeft, int eventNumber);
};

class LandscapeEventConditionTime : public LandscapeEventCondition
{
public:
	LandscapeEventConditionTime();
	virtual ~LandscapeEventConditionTime();

	XMLEntryFixed mintime;
	XMLEntryFixed maxtime;
	XMLEntryBool singletimeonly;

	virtual fixed getNextEventTime(ScorchedContext &context, 
		int eventNumber);
	virtual bool fireEvent(ScorchedContext &context, 
		fixed timeLeft, int eventNumber);
};

#endif // __INCLUDE_LandscapeEventConditionsh_INCLUDE__
