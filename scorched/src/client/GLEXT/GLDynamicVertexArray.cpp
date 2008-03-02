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

#include <GLEXT/GLDynamicVertexArray.h>
#include <GLEXT/GLInfo.h>
#include <graph/OptionsDisplay.h>

GLDynamicVertexArray *GLDynamicVertexArray::instance_ = 0;

GLDynamicVertexArray::GLDynamicVertexArray() : 
	capacity_(3000), used_(0), array_(0)
{
	current_ = array_ = new Data[capacity_];
}

GLDynamicVertexArray::~GLDynamicVertexArray()
{
	delete [] array_;
}

void GLDynamicVertexArray::reset()
{
	used_ = 0;
	current_ = array_;
}

void GLDynamicVertexArray::drawROAM()
{
	if (used_ < 3)
	{
		reset();
		return;
	}

	if (!GLStateExtension::hasVBO())
	{
		Data *start = array_;
		glBegin(GL_TRIANGLES);
		for (int i=0; i<used_; i++)
		{
			glTexCoord2f(start->t1x, start->t1y);
			if (GLStateExtension::hasMultiTex())
			{
				glMultiTexCoord2fARB(
					GL_TEXTURE1_ARB, start->t1x, start->t1y);
				if (GLStateExtension::getTextureUnits() > 2)
				{
					glMultiTexCoord2fARB(
						GL_TEXTURE2_ARB, start->t2x, start->t2y);
				}
			}

			glNormal3fv(&start->nx);
			glVertex3fv(&start->x); 
			start ++;
		}
		glEnd();
	}
	else
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		if (GLStateExtension::hasMultiTex())
		{
			glClientActiveTextureARB(GL_TEXTURE1_ARB);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			if (GLStateExtension::getTextureUnits() > 2)
			{
				glClientActiveTextureARB(GL_TEXTURE2_ARB);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			}
		}
		glClientActiveTextureARB(GL_TEXTURE0_ARB);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(Data), &array_[0].x);
		glNormalPointer(GL_FLOAT, sizeof(Data), &array_[0].nx);

		glTexCoordPointer(2, GL_FLOAT, sizeof(Data), &array_[0].t1x);
		if (GLStateExtension::hasMultiTex())
		{
			glClientActiveTextureARB(GL_TEXTURE1_ARB);
			glTexCoordPointer(2, GL_FLOAT, sizeof(Data), &array_[0].t1x);
			if (GLStateExtension::getTextureUnits() > 2)
			{
				glClientActiveTextureARB(GL_TEXTURE2_ARB);
				glTexCoordPointer(2, GL_FLOAT, sizeof(Data), &array_[0].t2x);
			}
		}

		glDrawArrays(GL_TRIANGLES, 0, used_);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		if (GLStateExtension::hasMultiTex())
		{
			glClientActiveTextureARB(GL_TEXTURE1_ARB);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			if (GLStateExtension::getTextureUnits() > 2)
			{
				glClientActiveTextureARB(GL_TEXTURE2_ARB);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			}
		}
		glClientActiveTextureARB(GL_TEXTURE0_ARB);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	GLInfo::addNoTriangles(used_ / 3);
	reset();
}
