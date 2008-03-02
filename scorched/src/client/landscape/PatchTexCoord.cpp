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

#include <landscape/PatchTexCoord.h>
#include <landscapemap/HeightMap.h>

PatchTexCoord::PatchTexCoord() : 
	widthEntries_(0), heightEntries_(0), map_(0)
{
}

PatchTexCoord::~PatchTexCoord()
{
}

void PatchTexCoord::generate(HeightMap *map)
{
	map_ = map;
	delete [] widthEntries_;
	delete [] heightEntries_;
	widthEntries_ = new PatchTexCoordEntry[map->getMapWidth() + 1];
	heightEntries_ = new PatchTexCoordEntry[map->getMapHeight() + 1];

	for (int i=0; i<=map->getMapWidth(); i++)
	{
		widthEntries_[i].txa = float(i) / float(map->getMapWidth());
		widthEntries_[i].txb = (float(i) / float(map->getMapWidth())) * 64.0f;
	}
	for (int i=0; i<=map->getMapHeight(); i++)
	{
		heightEntries_[i].txa = float(i) / float(map->getMapHeight());
		heightEntries_[i].txb = (float(i) / float(map->getMapHeight())) * 64.0f;
	}
}

