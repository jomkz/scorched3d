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

#include <land/VisibilityPatchInfo.h>

VisibilityPatchInfo::VisibilityPatchInfo()
{
}

VisibilityPatchInfo::~VisibilityPatchInfo()
{
}

void VisibilityPatchInfo::clear()
{
	delete [] landVisibility_.visibleLandPatches;
	landVisibility_.visibleLandPatches = 0;
	landVisibility_.visibleLandPatchesCount = 0;

	for (int i=0; i<4; i++)
	{
		delete waterVisibility_[i].visibleWaterPatches;
		waterVisibility_[i].visibleWaterPatches = 0;
		waterVisibility_[i].visibleWaterPatchesCount = 0;
	}
}

void VisibilityPatchInfo::generate(int maxLandPatches, int maxWaterPatches)
{
	clear();

	{
		// Create the patches
		landVisibility_.visibleLandPatches = landVisibility_.lastVisibleLandPatches =
			new LandVisibilityPatch*[maxLandPatches];
	}

	{
		// Create the patches
		for (int i=0; i<4; i++)
		{
			waterVisibility_[i].visibleWaterPatches = waterVisibility_[i].lastVisibleWaterPatches = 
				new WaterVisibilityPatch*[maxWaterPatches];
		}
	}

	reset();
}

void VisibilityPatchInfo::reset()
{
	landVisibility_.visibleLandPatchesCount = 0;
	landVisibility_.lastVisibleLandPatches = landVisibility_.visibleLandPatches;
	for (int i=0; i<4; i++)
	{
		waterVisibility_[i].visibleWaterPatchesCount = 0;
		waterVisibility_[i].lastVisibleWaterPatches = waterVisibility_[i].visibleWaterPatches;
	}
}
