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

#if !defined(__INCLUDE_LandVisibilityPatchh_INCLUDE__)
#define __INCLUDE_LandVisibilityPatchh_INCLUDE__

#include <common/Vector.h>

class MipMapPatchIndexs;
class MipMapPatchIndex;

class LandVisibilityPatch
{
public:
	LandVisibilityPatch();
	~LandVisibilityPatch();

	void setLocation(int x, int y,
		LandVisibilityPatch *leftPatch, 
		LandVisibilityPatch *rightPatch, 
		LandVisibilityPatch *topPatch, 
		LandVisibilityPatch *bottomPatch);
	void setVisible(Vector &cameraPos, bool visible);
	int getVisibilityIndex() { return visible_?visibilityIndex_:-1; }

	bool hasData() { return (heightMapData_ != 0); }

	LandVisibilityPatch *getLeftPatch() { return leftPatch_; }
	LandVisibilityPatch *getRightPatch() { return rightPatch_; }
	LandVisibilityPatch *getTopPatch() { return topPatch_; }
	LandVisibilityPatch *getBottomPatch() { return bottomPatch_; }

	void draw(MipMapPatchIndexs &indexes, 
		int indexPosition, int borders);
	void drawSurround();

protected:
	int x_, y_;
	int *heightMapData_;
	int visibilityIndex_;
	bool visible_;
	Vector position_;
	LandVisibilityPatch *leftPatch_;
	LandVisibilityPatch *rightPatch_; 
	LandVisibilityPatch *topPatch_;
	LandVisibilityPatch *bottomPatch_;

	void draw(MipMapPatchIndex &index);
};

#endif // __INCLUDE_LandVisibilityPatchh_INCLUDE__
