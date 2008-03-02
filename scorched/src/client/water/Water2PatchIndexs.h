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

#if !defined(__INCLUDE_Water2PatchIndexsh_INCLUDE__)
#define __INCLUDE_Water2PatchIndexsh_INCLUDE__

#include <water/Water2PatchIndex.h>
#include <vector>

class Water2PatchIndexs
{
public:
	Water2PatchIndexs();
	~Water2PatchIndexs();

	Water2PatchIndex &getIndex(int position, int border) 
	{ 
		if (position<0) position=0;
		else if (position >= getNoPositions()) position = getNoPositions()-1;
		if (border<0) border=0;
		if (border>15) border=15;

		return *indexs_[position * 16 + border]; 
	}
	int getNoPositions() { return noPositions_; }

	void generate(int size);

protected:
	int noPositions_;
	std::vector<Water2PatchIndex *> indexs_;
};

#endif // __INCLUDE_Water2PatchIndexsh_INCLUDE__
