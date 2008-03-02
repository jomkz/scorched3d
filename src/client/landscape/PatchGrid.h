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

#if !defined(AFX_PATCHGRID_H__78A69B7D_B848_4675_B017_13626E4856D1__INCLUDED_)
#define AFX_PATCHGRID_H__78A69B7D_B848_4675_B017_13626E4856D1__INCLUDED_

#include <landscapemap/HeightMap.h>
#include <landscape/Patch.h>
#include <vector>

class PatchGrid  
{
public:
	PatchGrid(HeightMap *hMap, int patchSize);
	virtual ~PatchGrid();
	
	void generate();
	void reset(ProgressCounter *counter = 0);
	void recalculateTankVariance();
	void recalculate(int posX, int posY, int dist);
	void draw(PatchSide::DrawType sides);
	void simulate(float frameTime);

	int getDrawnPatches() { return drawnPatches_; }

protected:
	Vector lastPos_;
	int drawnPatches_;
	int width_, height_;
	int patchSize_;
	float simulationTime_;
	Patch **patches_;
	HeightMap *hMap_;
	PatchTexCoord coord_;

	void tesselate();
	void visibility();
};

#endif // !defined(AFX_PATCHGRID_H__78A69B7D_B848_4675_B017_13626E4856D1__INCLUDED_)
