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


// Variance.h: interface for the Variance class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VARIANCE_H__FA0A735A_E6CF_4AA5_90BE_6E8C6073755C__INCLUDED_)
#define AFX_VARIANCE_H__FA0A735A_E6CF_4AA5_90BE_6E8C6073755C__INCLUDED_


#include <common/Vector.h>
#include <landscapemap/HeightMap.h>

class Variance  
{
public:
	Variance(HeightMap *hMap, int left, int top, int width);
	virtual ~Variance();

	void computeVariance();
	int getVDepth() { return vDepth_; }
	unsigned int getVariance(int pos) { return variance_[pos]; }
	unsigned int getMaxPos() { return maxPos_; }

protected:
	int left_, top_, width_;
	int vDepth_;
	unsigned int maxPos_;
	unsigned int *variance_;
	HeightMap *hMap_;

	unsigned int recursComputeVariance( int leftX,  int leftY,  float leftZ,
										int rightX, int rightY, float rightZ,
										int apexX,  int apexY,  float apexZ,
										int node);
};

#endif // !defined(AFX_VARIANCE_H__FA0A735A_E6CF_4AA5_90BE_6E8C6073755C__INCLUDED_)
