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

#include <geomipmap/MipMapPatchIndexs.h>

MipMapPatchIndexs::MipMapPatchIndexs()
{
}

MipMapPatchIndexs::~MipMapPatchIndexs()
{
}

void MipMapPatchIndexs::generate(int size, int totalsize)
{
	
	for (int lod=1; lod<=size; lod*=2)
	{
		IndexLevel *level = new IndexLevel();
		levels_.push_back(level);

		// borders
		for (unsigned int borders=0; borders<=4095; borders++)
		{
			MipMapPatchIndex *index = new MipMapPatchIndex();
			index->generate(size, totalsize, lod, borders);
			level->borderIndexs_.push_back(index);
		}
	}
}
