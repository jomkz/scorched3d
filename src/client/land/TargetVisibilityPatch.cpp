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

#include <land/TargetVisibilityPatch.h>
#include <client/ScorchedClient.h>
#include <landscapemap/LandscapeMaps.h>

TargetVisibilityPatch::TargetVisibilityPatch() : 
	visible_(false)
{
}

TargetVisibilityPatch::~TargetVisibilityPatch()
{
}

void TargetVisibilityPatch::setLocation(int x, int y)
{
	// Set location and neighbors
	x_ = x; y_ = y;
	float heightRange = 0.0f;
	float minHeight = 0.0f;
	position_ = Vector(float(x_ + 16), float(y_ + 16), 
		heightRange / 2.0f + minHeight);
}

bool TargetVisibilityPatch::setVisible(float distance)
{ 
	visible_ = true;
	return true;
}

void TargetVisibilityPatch::setNotVisible()
{
	visible_ = false;
}
