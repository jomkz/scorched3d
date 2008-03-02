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


// Variance.cpp: implementation of the Variance class.
//
//////////////////////////////////////////////////////////////////////

#include <math.h>
#include <landscape/Variance.h>
#include <common/Defines.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Variance::Variance(HeightMap *hMap, int left, int top, int width) : 
	hMap_(hMap), left_(left), top_(top), width_(width)
{
	vDepth_ = 6;
	maxPos_ = 1 << (vDepth_ + 1);
	variance_ = new unsigned int[maxPos_];
	memset(variance_, 0, maxPos_ * sizeof(unsigned int));
}

Variance::~Variance()
{
	delete [] variance_;
}

unsigned int Variance::recursComputeVariance( int leftX,  int leftY,  float leftZ,
											   int rightX, int rightY, float rightZ,
											   int apexX,  int apexY,  float apexZ,
											   int node)
{
	//        /||
	//      /  |  |
	//    /    |    |
	//  /      |      |
	//  ~~~~~~~*~~~~~~~  <-- Compute the X and Y coordinates of '*'
	//
	int centerX = (leftX + rightX) >>1;		// Compute X coordinate of center of Hypotenuse
	int centerY = (leftY + rightY) >>1;		// Compute Y coord.

	// Get the height value at the middle of the Hypotenuse
	float centerZ  = hMap_->getHeight(centerX, centerY).asFloat();

	// Variance of this triangle is the actual height at it's hypotenuse midpoint minus the interpolated height.
	// Use values passed on the stack instead of re-accessing the Height Field.
	float part = centerZ - ((leftZ + rightZ) / 2.0f);
	unsigned int myVariance = (unsigned int) (part < 0.0f?part * -50.0f:part * 50.0f);

	if ( ((leftX - rightX) >= 2) || ((leftX - rightX) <= -2) ||
		 ((leftY - rightY) >= 2) || ((leftY - rightY) <= -2))
	{
		// Final Variance for this node is the max of it's own variance and that of it's children.
		myVariance = MAX( myVariance, recursComputeVariance( apexX,   apexY,  apexZ, leftX, leftY, leftZ, centerX, centerY, centerZ,    node<<1 ) );
		myVariance = MAX( myVariance, recursComputeVariance( rightX, rightY, rightZ, apexX, apexY, apexZ, centerX, centerY, centerZ, 1+(node<<1)) );
	}

	// Store the final variance for this node.  Note Variance is never zero.
	if ((unsigned int) node < maxPos_) 
	{
		variance_[node] = 1 + myVariance;
	}

	return myVariance;
}

void Variance::computeVariance()
{
	memset(variance_, 0, maxPos_ * sizeof(unsigned int));
	recursComputeVariance(	left_ + width_,		top_ + width_, 	hMap_->getHeight(left_+ width_, top_ + width_).asFloat(),
							left_,				top_, hMap_->getHeight(left_, top_).asFloat(),
							left_ + width_,				top_, hMap_->getHeight(left_ + width_, top_).asFloat(),
							1);	
}
