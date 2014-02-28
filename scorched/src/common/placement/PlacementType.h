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

#if !defined(__INCLUDE_PlacementTypeh_INCLUDE__)
#define __INCLUDE_PlacementTypeh_INCLUDE__

#include <XML/XMLEntrySimpleTypes.h>

class XMLNode;
class ScorchedContext;
class ProgressCounter;
class RandomGenerator;
class PlacementObjectChoice;
class PlacementType  : public XMLEntryContainer
{
public:
	struct Position
	{
		FixedVector position;
		FixedVector velocity;
	};

	PlacementType(const char *typeName, const char *description);
	virtual ~PlacementType();

	void createObjects(ScorchedContext &context,
		RandomGenerator &generator,
		unsigned int &playerId,
		ProgressCounter *counter = 0);

	virtual void getPositions(ScorchedContext &context,
		RandomGenerator &generator,
		std::list<Position> &returnPositions,
		ProgressCounter *counter = 0) = 0;

protected:
	PlacementObjectChoice *placementobject;

	bool checkCloseness(FixedVector &position,
		ScorchedContext &context,
		std::list<Position> &returnPositions,
		fixed mincloseness);
};

class PlacementTypeChoice : public XMLEntryTypeChoice<PlacementType>
{
public:
	PlacementTypeChoice();
	virtual ~PlacementTypeChoice();

	virtual PlacementType *createXMLEntry(const std::string &type, void *xmlData);
	virtual void getAllTypes(std::set<std::string> &allTypes);
};

class PlacementTypeList : public XMLEntryList<PlacementTypeChoice>
{
public:
	PlacementTypeList();
	virtual ~PlacementTypeList();

	virtual PlacementTypeChoice *createXMLEntry(void *xmlData);
};

#endif // __INCLUDE_PlacementTypeh_INCLUDE__
