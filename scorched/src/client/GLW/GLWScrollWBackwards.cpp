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

#include <GLW/GLWScrollWBackwards.h>
#include <common/Defines.h>

REGISTER_CLASS_SOURCE(GLWScrollWBackwards);

GLWScrollWBackwards::GLWScrollWBackwards(float x, float y, float h, int min, int max, int see) :
	GLWScrollW(x, y, h, min, max, see)
{
}

GLWScrollWBackwards::~GLWScrollWBackwards()
{
}

void GLWScrollWBackwards::setCurrent(int c)
{
	if (getMax() <= getSee()) 
	{
		GLWScrollW::setCurrent(0);
	}
	else
	{
		int left = getMax() - getSee();
		GLWScrollW::setCurrent(MAX(0, left - c));
	}
}

int GLWScrollWBackwards::getCurrent()
{
	if (getMax() <= getSee()) 
	{
		return 0;
	}
	return getMax() - getSee() - GLWScrollW::getCurrent();
}

void GLWScrollWBackwards::mouseWheel(float x, float y, float z, bool &skipRest)
{
	GLWScrollW::mouseWheel(x, y, z, skipRest);
}
