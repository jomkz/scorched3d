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

#if !defined(__INCLUDE_PatchTexCoordh_INCLUDE__)
#define __INCLUDE_PatchTexCoordh_INCLUDE__

#include <common/DefinesAssert.h>
#include <landscapemap/HeightMap.h>

class PatchTexCoord
{
public:
	PatchTexCoord();
	virtual ~PatchTexCoord();

	struct PatchTexCoordEntry
	{
		float txa;
		float txb;
	};

	void generate(HeightMap *map);
	inline PatchTexCoordEntry &getWidthEntry(int i)
	{
		DIALOG_ASSERT(i >=0 && i <= map_->getMapWidth());
		return widthEntries_[i];
	};
	inline PatchTexCoordEntry &getHeightEntry(int i)
	{
		DIALOG_ASSERT(i >= 0 && i <= map_->getMapHeight());
		return heightEntries_[i];
	};

protected:
	HeightMap *map_;
	PatchTexCoordEntry *widthEntries_;
	PatchTexCoordEntry *heightEntries_;
};

#endif // __INCLUDE_PatchTexCoordh_INCLUDE__
