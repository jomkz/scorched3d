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

#if !defined(AFX_PATCH_H__0714E972_F796_4BAF_878D_0D0288F10888__INCLUDED_)
#define AFX_PATCH_H__0714E972_F796_4BAF_878D_0D0288F10888__INCLUDED_

#include <landscape/PatchSide.h>
#include <landscapemap/HeightMap.h>

class Patch  
{
public:
	Patch(HeightMap *map, PatchTexCoord *coord, 
		int x, int y, int width);
	virtual ~Patch();

	void computeVariance();
	void tessalate(unsigned currentVariance);
	void reset();
	void draw(PatchSide::DrawType side);

	TriNode *getLeftTri() { return leftSide_.getTriNode(); }
	TriNode *getRightTri() { return rightSide_.getTriNode(); }
	Vector &getMidPoint() { return midPoint_; }

	bool &getForceVariance() { return forceVariance_; }
	bool &getRecalculate() { return recalculate_; }
	bool &getVisible() { return visible_; }

	int getX() { return x_; }
	int getY() { return y_; }
	int getWidth() { return width_; }

protected:
	bool forceVariance_;
	bool recalculate_;
	bool visible_;
	int x_, y_, width_;
	PatchSide leftSide_, rightSide_;
	Vector midPoint_;
};

#endif // !defined(AFX_PATCH_H__0714E972_F796_4BAF_878D_0D0288F10888__INCLUDED_)
