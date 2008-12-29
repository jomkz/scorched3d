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

#if !defined(__INCLUDE_ASEModelFactoryh_INCLUDE__)
#define __INCLUDE_ASEModelFactoryh_INCLUDE__

#include <3dsparse/Model.h>

class ASEModelFactory
{
public:
	ASEModelFactory();
	virtual ~ASEModelFactory();

	Model *createModel(const char *fileName,
		const char *texName);

	// Used by parser
	static ASEModelFactory *getCurrent();
	void addMesh(char *meshName);
	Mesh *getCurrentMesh();

protected:
	enum MaxMag
	{
		MagX,
		MagY,
		MagZ
	};

	bool loadFile(const char *fileName);
	void calculateTexCoords(const char *texName);
	Vector getTexCoord(Vector &tri, MaxMag mag, Vector &max, Vector &min);
};

#endif // __INCLUDE_ASEModelFactoryh_INCLUDE__
