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

#include <land/WaterVisibilityPatch.h>
#include <land/VisibilityPatchGrid.h>
#include <geomipmap/MipMapPatchIndexs.h>
#include <graph/OptionsDisplay.h>

WaterVisibilityPatch::WaterVisibilityPatch() : 
	visible_(false),
	leftPatch_(0), rightPatch_(0),
	topPatch_(0), bottomPatch_(0),
	visibilityIndex_(4)
{
}

WaterVisibilityPatch::~WaterVisibilityPatch()
{
}

void WaterVisibilityPatch::setLocation(int x, int y,
	WaterVisibilityPatch *leftPatch, 
	WaterVisibilityPatch *rightPatch, 
	WaterVisibilityPatch *topPatch, 
	WaterVisibilityPatch *bottomPatch)
{
	x_ = x; y_ = y;
	leftPatch_ = leftPatch;
	rightPatch_ = rightPatch;
	topPatch_ = topPatch;
	bottomPatch_ = bottomPatch;

	patchX_ = (abs(x_) / 128) % 2;
	patchY_ = (abs(y_) / 128) % 2;

	offset_ = Vector(x_, y_, 0);
	position_ = Vector(x_ + 64, y_ + 64, 5);
}

void WaterVisibilityPatch::setVisible(Vector &cameraPos, bool visible)
{ 
	visible_ = visible; 

	if (visible)
	{
		float distance = (cameraPos - position_).Magnitude();
		visibilityIndex_ = 6;
		if (!OptionsDisplay::instance()->getNoWaterLOD())
		{
			visibilityIndex_ = int(distance - 50.0f) / 130;
			if (OptionsDisplay::instance()->getNoWaterMovement())
			{
				visibilityIndex_ += 3;
			}
			visibilityIndex_ = MAX(0, MIN(visibilityIndex_, 6));
		}

		if (distance < 2000.0f)
		{
			VisibilityPatchGrid::instance()->addVisibleWaterPatch(this);
		}
	}
}
