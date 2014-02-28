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

#if !defined(__INCLUDE_TanketShotPathh_INCLUDE__)
#define __INCLUDE_TanketShotPathh_INCLUDE__

#include <engine/PhysicsParticleObject.h>
#include <engine/ScorchedContext.h>
#include <tanket/Tanket.h>
#include <vector>

class TanketShotPath : public PhysicsParticleObjectHandler
{
public:
	TanketShotPath(ScorchedContext &context, Tanket *tanket, bool keepPath = false);
	virtual ~TanketShotPath();

	bool makeShot(fixed rotation, fixed elevation, fixed power, FixedVector &result);

	std::vector<FixedVector> &getPositions() { return positions_; }

	// PhysicsParticleObjectHandler
	virtual void collision(PhysicsParticleObject &position, 
		ScorchedCollisionId collisionId);
	virtual void wallCollision(PhysicsParticleObject &position,
		ScorchedCollisionId collisionId);
protected:
	bool keepPath_;
	std::vector<FixedVector> positions_;
	FixedVector resultPosition_;
	bool collision_;
	Tanket *tanket_;
	ScorchedContext &context_;
};

#endif
