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

#if !defined(__INCLUDE_VisibilityPatchGridh_INCLUDE__)
#define __INCLUDE_VisibilityPatchGridh_INCLUDE__

#include <land/LandVisibilityPatch.h>
#include <land/WaterVisibilityPatch.h>
#include <land/VisibilityPatchQuad.h>
#include <geomipmap/MipMapPatchIndexs.h>

class VisibilityPatchGrid
{
public:
	static VisibilityPatchGrid *instance();

	void generate();
	void drawVisibility();
	void drawLand();

	LandVisibilityPatch *getLandVisibilityPatch(int x, int y);
	WaterVisibilityPatch *getWaterVisibilityPatch(int x, int y);

	int getLandWidth() { return landWidth_; }
	int getLandHeight() { return landHeight_; }
	LandVisibilityPatch *getLandPatches() { return landPatches_; }

	int getWaterWidth() { return waterWidth_; }
	int getWaterHeight() { return waterHeight_; }
	WaterVisibilityPatch *getWaterPatches() { return waterPatches_; }

protected:
	MipMapPatchIndexs landIndexs_;
	LandVisibilityPatch *landPatches_;
	WaterVisibilityPatch *waterPatches_;
	VisibilityPatchQuad *visibilityPatches_;

	int midX_, midY_;
	int landWidth_, landHeight_;
	int waterWidth_, waterHeight_;
	int visibilityWidth_, visibilityHeight_;

	void clear();

private:
	VisibilityPatchGrid();
	~VisibilityPatchGrid();
};

#endif // __INCLUDE_VisibilityPatchGridh_INCLUDE__
