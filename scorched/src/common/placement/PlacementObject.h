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

#if !defined(__INCLUDE_PlacementObjecth_INCLUDE__)
#define __INCLUDE_PlacementObjecth_INCLUDE__

#include <placement/PlacementType.h>
#include <XML/XMLEntry.h>

class XMLNode;
class PlacementObject : public XMLEntryContainer
{
public:
	enum Type
	{
		eTarget,
		eGroup,
		eRandom,
		eHeight,
		eNone
	};

	PlacementObject(const char *typeName, const char *description);
	virtual ~PlacementObject();

	virtual Type getType() = 0;
	virtual void createObject(ScorchedContext &context,
		RandomGenerator &generator,
		unsigned int &playerId,
		PlacementType::Position &position) = 0;
};

class PlacementObjectChoice : public XMLEntryTypeChoice<PlacementObject>
{
public:
	PlacementObjectChoice();
	virtual ~PlacementObjectChoice();

	virtual PlacementObject *createXMLEntry(const std::string &type, void *xmlData);
	virtual void getAllTypes(std::set<std::string> &allTypes);
};

#endif // __INCLUDE_PlacementObjecth_INCLUDE__
