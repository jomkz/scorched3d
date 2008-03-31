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

#if !defined(__INCLUDE_VisibilityPatchInfoh_INCLUDE__)
#define __INCLUDE_VisibilityPatchInfoh_INCLUDE__

#include <land/LandVisibilityPatch.h>
#include <land/WaterVisibilityPatch.h>

class VisibilityPatchInfo
{
public:
	VisibilityPatchInfo();
	~VisibilityPatchInfo();

	int getVisibleLandPatchesCount() { 
		return landVisibility_.visibleLandPatchesCount; }
	int getVisibleWaterPatchesCount() { 
		return waterVisibility_[0].visibleWaterPatchesCount +
			waterVisibility_[1].visibleWaterPatchesCount +
			waterVisibility_[2].visibleWaterPatchesCount +
			waterVisibility_[3].visibleWaterPatchesCount; }

	void reset();
	void generate(int maxLandPatches, int maxWaterPatches);

	struct LandVisibility
	{
		LandVisibility() : visibleLandPatchesCount(0), visibleLandPatches(0), 
			lastVisibleLandPatches(0) {}

		int visibleLandPatchesCount;
		LandVisibilityPatch **visibleLandPatches, **lastVisibleLandPatches;

	};
	LandVisibility landVisibility_;

	struct WaterVisibility
	{
		WaterVisibility() : visibleWaterPatchesCount(0), visibleWaterPatches(0), 
			lastVisibleWaterPatches(0) {}

		int visibleWaterPatchesCount;
		WaterVisibilityPatch **visibleWaterPatches, **lastVisibleWaterPatches;

	};
	WaterVisibility waterVisibility_[4];

	void addVisibleLandPatch(LandVisibilityPatch *patch)
	{
		*landVisibility_.lastVisibleLandPatches = patch;
		landVisibility_.visibleLandPatchesCount++;
		landVisibility_.lastVisibleLandPatches++;
	}

	void addVisibleWaterPatch(int index, WaterVisibilityPatch *patch)
	{
		DIALOG_ASSERT(index >= 0 && index <= 4);

		*waterVisibility_[index].lastVisibleWaterPatches = patch;
		waterVisibility_[index].visibleWaterPatchesCount++;
		waterVisibility_[index].lastVisibleWaterPatches++;
	}
protected:
	void clear();
};

#endif // __INCLUDE_VisibilityPatchInfoh_INCLUDE__
