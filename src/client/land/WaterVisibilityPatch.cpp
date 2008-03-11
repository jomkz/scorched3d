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
#include <geomipmap/MipMapPatchIndexs.h>

WaterVisibilityPatch::WaterVisibilityPatch() : visible_(false)
{
}

WaterVisibilityPatch::~WaterVisibilityPatch()
{
}

void WaterVisibilityPatch::setLocation(int x, int y)
{
	x_ = x; y_ = y;

	int offX = (x / 256) * 256;
	if (x < 0 && x % 256 != 0) offX -= 256;
	int offY = (y / 256) * 256;
	if (y < 0 && y % 256 != 0) offY -= 256;

	offset_ = Vector(offX, offY, 0);
	anyOffset_ = (offset_ != Vector::getNullVector());
}

void WaterVisibilityPatch::draw(MipMapPatchIndexs &indexes, 
	int indexPosition, int borders)
{

}
