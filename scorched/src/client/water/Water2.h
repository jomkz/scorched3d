////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2009
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

#if !defined(__INCLUDE_Water2h_INCLUDE__)
#define __INCLUDE_Water2h_INCLUDE__

#include <water/Water2Patches.h>

class LandscapeDefn;
class ImageHandle;
class LandscapeTexBorderWater;
class ProgressCounter;
class Water2
{
public:
	Water2();
	virtual ~Water2();

	void generate(LandscapeTexBorderWater *water, ProgressCounter *counter = 0);

	Water2Patches &getPatch(float time);
	MipMapPatchIndexs &getIndexs() { return indexs_; }
	float *getIndexErrors() { return indexErrors_; }

protected:
	int generatedPatches_;
	float indexErrors_[7];
	Water2Patches patches_[256];
	MipMapPatchIndexs indexs_;

	void generateAOF(Water2Points &wd, ImageHandle *aofImage, float *rndtab, 
		Water2Points *displacements, float *aof);
	void generateTransparency(Water2Points &wd, ImageHandle &oafImage, 
		LandscapeDefn &defn);
};

#endif // __INCLUDE_Water2h_INCLUDE__
