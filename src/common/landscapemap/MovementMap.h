////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
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

#if !defined(__INCLUDE_MovementMaph_INCLUDE__)
#define __INCLUDE_MovementMaph_INCLUDE__

#include <landscapemap/HeightMap.h>
#include <list>
#include <queue>

class WeaponMoveTank;
class Tank;
class Target;
class ScorchedContext;
class MovementMap
{
public:
	enum MovementMapEntryType
	{
		eNotInitialized = 0,
		eNotSeen = 1,
		eNoMovement = 2,
		eMovement = 3
	};
	struct MovementMapEntry
	{
		MovementMapEntry() {}
		MovementMapEntry(
			MovementMapEntryType t,
			fixed d,
			unsigned int s,
			unsigned int e) : type(t), dist(d), srcEntry(s), epoc(e) {}

		MovementMapEntryType type;
		fixed dist;
		unsigned int srcEntry;
		unsigned int epoc;
	};
	struct QueuePosition
	{
		fixed distance;
		unsigned int square;

		bool operator<(const QueuePosition &rhs) const;
	};

	MovementMap(int width, int height, 
		Tank *tank, 
		ScorchedContext &context);
	virtual ~MovementMap();

	bool calculatePosition(FixedVector &position, fixed fuel);
	void calculateAllPositions(fixed fuel);
	MovementMapEntry &getEntry(int w, int h);

	// Create landscape textures that show where the tank can move to
	void movementTexture();
	static void limitTexture(FixedVector &center, int limit);
	fixed getFuel(WeaponMoveTank *weapon);

	// Functions returns true if a tank can move into the give position,
	// false otherwise.  Shields and obstacles my prevent a tank moving.
	static bool inShield(Target *target, Tank *tank, FixedVector &position);
	static bool movementProof(ScorchedContext &context, 
		Target *target, Tank *tank);
	static bool allowedPosition(ScorchedContext &context, 
		Tank *tank, FixedVector &position);

protected:
	MovementMapEntry *entries_;
	int width_, height_;
	fixed minHeight_;
	Tank *tank_;
	ScorchedContext &context_;
	std::list<Target *> checkTargets_;

	unsigned int POINT_TO_UINT(unsigned int x, unsigned int y);
	void addPoint(unsigned int x, unsigned int y, 
					 fixed height, fixed dist,
					 std::list<unsigned int> &edgeList,
					 unsigned int sourcePt,
					 unsigned int epoc);
	void addPoint(unsigned int x, unsigned int y, 
					 fixed height, fixed dist,
					 std::priority_queue<QueuePosition, 
						std::vector<QueuePosition>, 
						std::less<QueuePosition> > &priorityQueue,
					 unsigned int sourcePt,
					 FixedVector &position);

	fixed getWaterHeight();
	bool tankBurried();
	MovementMapEntry &getAndCheckEntry(int w, int h);


};

#endif
