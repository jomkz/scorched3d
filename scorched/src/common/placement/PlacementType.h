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

#if !defined(__INCLUDE_PlacementTypeh_INCLUDE__)
#define __INCLUDE_PlacementTypeh_INCLUDE__

#include <common/FixedVector.h>
#include <string>
#include <list>

class XMLNode;
class ScorchedContext;
class ProgressCounter;
class RandomGenerator;
class PlacementObject;
class PlacementType
{
public:
	enum Type
	{
		eMask,
		eCount,
		eTree,
		eDirect,
		eBounds,
		eTankStart
	};
	struct Position
	{
		FixedVector position;
		FixedVector velocity;
	};

	static PlacementType *create(const char *type);

	PlacementType();
	virtual ~PlacementType();

	void createObjects(ScorchedContext &context,
		RandomGenerator &generator,
		unsigned int &playerId,
		ProgressCounter *counter = 0);

	virtual Type getType() = 0;
	virtual bool readXML(XMLNode *node);
	virtual void getPositions(ScorchedContext &context,
		RandomGenerator &generator,
		std::list<Position> &returnPositions,
		ProgressCounter *counter = 0) = 0;

protected:
	PlacementObject *placementobject;

	bool checkCloseness(FixedVector &position,
		ScorchedContext &context,
		std::list<Position> &returnPositions,
		fixed mincloseness);
};

#endif // __INCLUDE_PlacementTypeh_INCLUDE__
