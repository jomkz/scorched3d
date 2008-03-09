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

#if !defined(__INCLUDE_Water2PatchVisibilityh_INCLUDE__)
#define __INCLUDE_Water2PatchVisibilityh_INCLUDE__

#include <geomipmap/MipMapPatchIndexs.h>
#include <common/Vector.h>

class GLSLShaderSetup;
class Water2Patches;
class Water2PatchVisibility
{
public:
	Water2PatchVisibility();
	~Water2PatchVisibility();

	void generate(Vector &offset, unsigned int totalSize, 
		unsigned int patchesSize, unsigned int patchSize);
	void draw(Water2Patches &patches, 
		MipMapPatchIndexs &indexes, Vector &cameraPosition, 
		Vector landscapeSize,
		GLSLShaderSetup *waterShader);

	unsigned int getVisibility(int x, int y)
	{
		if (x < 0 || x >= size_ || y < 0 || y >= size_) return NotVisible;
		return visibility_[x + y * size_].visibilityIndex;
	}

	static const unsigned int NotVisible;

protected:
	struct VisibilityEntry
	{
		VisibilityEntry();

		unsigned int visibilityIndex;
		Vector position;
		Vector offset;
		bool anyoffset;
		bool onScreen;
		bool ignore;
	};

	int size_;
	VisibilityEntry *visibility_;
};

#endif // __INCLUDE_Water2PatchVisibilityh_INCLUDE__
