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

#if !defined(__INCLUDE_SplinePathh_INCLUDE__)
#define __INCLUDE_SplinePathh_INCLUDE__

#include <common/FixedVector.h>
#include <vector>

class SplinePath
{
public:
	SplinePath();
	virtual ~SplinePath();

	void generate(
		std::vector<FixedVector> &inPoints, 
		int resolution = 200,
		int polynomials = 3,
		fixed pointsPerSecond = 5);

	void draw();
	void simulate(fixed frameTime);
	void getPathAttrs(FixedVector &position, FixedVector &direction);

	fixed getPathTime() { return pathTime_; }
	void setPathTime(fixed pathTime) { pathTime_ = pathTime; }

	std::vector<FixedVector> &getControlPoints() { return controlPoints_; }
	std::vector<FixedVector> &getPathPoints() { return pathPoints_; }

protected:
	std::vector<FixedVector> controlPoints_;
	std::vector<FixedVector> pathPoints_;
	fixed pathTime_;
	fixed pointsPerSecond_;
};

#endif // __INCLUDE_SplinePathh_INCLUDE__
