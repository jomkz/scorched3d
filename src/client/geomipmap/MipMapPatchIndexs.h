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

#if !defined(__INCLUDE_MipMapPatchIndexsh_INCLUDE__)
#define __INCLUDE_MipMapPatchIndexsh_INCLUDE__

#include <geomipmap/MipMapPatchIndex.h>
#include <common/DefinesAssert.h>
#include <common/DefinesMath.h>
#include <vector>

class MipMapPatchIndexs
{
protected:

	class IndexLevel // One per LOD
	{
	public:
		std::vector<MipMapPatchIndex *> borderIndexs_;
	};

public:
	MipMapPatchIndexs();
	~MipMapPatchIndexs();

	MipMapPatchIndex &getIndex(int lod, int leftLod, int rightLod, int topLod, int bottomLod, int addLod = 0)
	{
		unsigned int borders = 0;
		if (leftLod != -1 && leftLod > lod) 
		{
			borders |= (leftLod - lod);
		}
		if (rightLod != -1 && rightLod > lod)
		{
			borders |= (rightLod - lod) << 3;
		}
		if (topLod != -1 && topLod > lod) 
		{
			borders |= (topLod - lod) << 9;
		}
		if (bottomLod != -1 && bottomLod > lod) 
		{
			borders |= (bottomLod - lod) << 6;
		}

		int newLod = MIN(lod + addLod, getNoLevels() - 1);
		return getIndex(newLod, borders);
	}

	MipMapPatchIndex &getIndex(int lod, int border) 
	{ 
		if (lod<0) lod=0;
		else if (lod >= getNoLevels()) lod = getNoLevels()-1;

		DIALOG_ASSERT(border < 4096);

		IndexLevel *level = levels_[lod];
		MipMapPatchIndex *index = level->borderIndexs_[border];

		DIALOG_ASSERT(index->getIndices());

		return *index;
	}
	int getNoLevels() { return (int) levels_.size(); }

	void generate(int size, int totalsize);

protected:

	std::vector<IndexLevel *> levels_;
};

#endif // __INCLUDE_MipMapPatchIndexsh_INCLUDE__
