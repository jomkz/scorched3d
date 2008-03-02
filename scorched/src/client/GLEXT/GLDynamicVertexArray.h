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

#if !defined(__INCLUDE_GLDynamicVertexArrayh_INCLUDE__)
#define __INCLUDE_GLDynamicVertexArrayh_INCLUDE__

#include <GLEXT/GLState.h>
#include <GLEXT/GLStateExtension.h>
#include <common/DefinesAssert.h>

class GLDynamicVertexArray
{
public:
	static GLDynamicVertexArray *instance()
	{
		if (!instance_) instance_ = new GLDynamicVertexArray;
		return instance_;
	}

	struct Data
	{
		float x, y, z;
		float nx, ny, nz;
		float t1x, t1y;
		float t2x, t2y;
	};

	int getSpace() { return capacity_ - used_; }
	int getUsed() { return used_; }

	Data *getData()
	{
		Data *c = current_;
		current_++;
		used_++;
		return c;
	}

	void drawROAM();
	void reset();

protected:
	static GLDynamicVertexArray *instance_;
	Data *array_;
	int used_;
	Data *current_;
	int capacity_;

private:
	GLDynamicVertexArray();
	virtual ~GLDynamicVertexArray();
};

#endif // __INCLUDE_GLDynamicVertexArrayh_INCLUDE__
