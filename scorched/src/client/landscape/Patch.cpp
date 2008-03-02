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

#include <GLEXT/GLState.h>
#include <landscape/Patch.h>

Patch::Patch(HeightMap *map, PatchTexCoord *coord, 
	int x, int y, int width) :
	x_(x), y_(y), width_(width),
	leftSide_(map, coord, x, y, width),
	rightSide_(map, coord, x+width, y+width, -width),
	forceVariance_(false), recalculate_(false), visible_(true),
	midPoint_(x + width / 2, y + width / 2, 0)
{

}

Patch::~Patch()
{

}

void Patch::reset()
{
	leftSide_.reset();
	rightSide_.reset();

	leftSide_.getTriNode()->BaseNeighbor = rightSide_.getTriNode();
	rightSide_.getTriNode()->BaseNeighbor = leftSide_.getTriNode();
}

void Patch::computeVariance()
{
	leftSide_.computeVariance();
	rightSide_.computeVariance();
}

void Patch::tessalate(unsigned currentVariance)
{
	leftSide_.tesselate(currentVariance);
	rightSide_.tesselate(currentVariance);
}

void Patch::draw(PatchSide::DrawType side)
{
	leftSide_.draw(side);
	rightSide_.draw(side);
}
