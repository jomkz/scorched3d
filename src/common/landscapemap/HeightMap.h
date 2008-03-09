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

#if !defined(AFX_HEIGHTMAP_H__F4CB4CAD_C592_4183_AFB2_016FC66C144A__INCLUDED_)
#define AFX_HEIGHTMAP_H__F4CB4CAD_C592_4183_AFB2_016FC66C144A__INCLUDED_

#include <stdlib.h>
#include <common/ProgressCounter.h>
#include <common/FixedVector.h>
#include <common/Vector.h>

class Line;

class HeightMap  
{
public:
	struct HeightData
	{
		FixedVector position;
		FixedVector normal;
		float texCoord1x, texCoord1y;
		float texCoord2x, texCoord2y;
	};

	HeightMap();
	virtual ~HeightMap();

	void create(int width, int height);
	void reset();

	// Height map size fns
	int getMapWidth() { return width_; }
	int getMapHeight() { return height_; }

	// Get height fns (z values)
	inline fixed getHeight(int w, int h) { 
		if (w >= 0 && h >= 0 && w<=width_ && h<=height_) 
			return heightData_[(width_+1) * h + w].position[2]; 
		return fixed(0); }
	fixed getInterpHeight(fixed w, fixed h);

	// Get normal functions
	FixedVector &getNormal(int w, int h);
	void getInterpNormal(fixed w, fixed h, FixedVector &normal);

	bool getIntersect(Line &direction, Vector &intersect);

	// Alters the actual internal HeightMap points
	void setHeight(int w, int h, fixed height);

	// Should only be used read only
	HeightData *getHeightData() { return heightData_; }

protected:
	int width_, height_;
	HeightData *heightData_;

	bool getVector(FixedVector &vec, int x, int y);
	void getVectorPos(int pos, int &x, int &y, int dist=1);
};

#endif // !defined(AFX_HEIGHTMAP_H__F4CB4CAD_C592_4183_AFB2_016FC66C144A__INCLUDED_)

