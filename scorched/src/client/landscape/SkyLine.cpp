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

#include <GLEXT/GLState.h>
#include <landscape/SkyLine.h>
#include <common/Defines.h>
#include <math.h>

SkyLine::SkyLine() :
	listNo_(0)
{
}

SkyLine::~SkyLine()
{
	clear();
}

void SkyLine::clear()
{
	if (listNo_) glDeleteLists(listNo_, 1);
	listNo_ = 0;
}

void SkyLine::draw(float radius, float radius2, float height)
{
	if (listNo_)
	{
		glCallList(listNo_);
	}
	else
	{
		glNewList(listNo_ = glGenLists(1), GL_COMPILE_AND_EXECUTE);
			actualDraw(radius, radius2, height);
		glEndList();
	}
}

void SkyLine::actualDraw(float radius, float radius2, float height)
{
	GLState state(GLState::TEXTURE_ON | GLState::BLEND_ON | GLState::ALPHATEST_ON);

	glColor3f(1.0f, 1.0f, 1.0);
	glBegin(GL_QUAD_STRIP);
	for (float a=0.0f; a<=360.0f; a+=360.0f / 12)
	{
		float x = sinf(a / 180.0f * PI) * radius;
		float y = cosf(a / 180.0f * PI) * radius2;

		glTexCoord2f(a / 360.0f, 1.0f);
		glVertex3f(x, y, height);

		glTexCoord2f(a / 360.0f, 0.0f);
		glVertex3f(x, y, 0.0f);
	}
	glEnd();
}