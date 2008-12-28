////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
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


#if !defined(__INCLUDE_Wallh_INCLUDE__)
#define __INCLUDE_Wallh_INCLUDE__

#include <common/Vector.h>
#include <common/OptionsTransient.h>
#include <GLEXT/GLTexture.h>

class Wall
{
public:
	Wall();
	virtual ~Wall();

	void wallHit(Vector &position, OptionsTransient::WallSide side);
	void draw();
	void simulate(float time);

protected:
	bool createdTexture_;
	GLTexture texture_;
	float fadeTime_[4];

	void drawWall(Vector &cornerA, Vector &cornerB, 
				Vector &cornerC, Vector &cornerD,
				float fade);
};

#endif
