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

#if !defined(__INCLUDE_LandVisibilityPatchQuadh_INCLUDE__)
#define __INCLUDE_LandVisibilityPatchQuadh_INCLUDE__

#include <common/Vector.h>

class LandVisibilityPatch;
class LandVisibilityPatchGrid;
class LandVisibilityPatchQuad
{
public:
	LandVisibilityPatchQuad();
	~LandVisibilityPatchQuad();

	void setLocation(LandVisibilityPatchGrid *patchGrid, int x, int y, int size);
	void calculateVisibility();

	void setNotVisible();
	void setVisible();

protected:
	int x_, y_;
	int size_;
	Vector position_;

	LandVisibilityPatch *LandVisibilityPatch_;
	LandVisibilityPatchQuad *topLeft_, *topRight_;
	LandVisibilityPatchQuad *botLeft_, *botRight_;
};

#endif // __INCLUDE_LandVisibilityPatchQuadh_INCLUDE__
