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

#include <GLEXT/GLVertexArray.h>
#include <GLEXT/GLStateExtension.h>
#include <GLEXT/GLInfo.h>
#include <common/Defines.h>
#include <stdlib.h>

// ************ NOTE VBO TURNED OFF FOR NOW ************** //
GLVertexArray::GLVertexArray(GLenum prim, int noTris, 
	unsigned int type, GLTexture *texture) : 
	prim_(prim), noTris_(noTris), setup_(false),
	verticesVBO_(0), colorsVBO_(0), textureVBO_(0),
	texture_(texture), secondTexture_(false),
	useVBO_(true), listNo_(0)
{
	if (type & typeVertex)
	{
		vertices_ = (GLVertexArrayVertex *) 
			malloc(sizeof(GLVertexArrayVertex) * noTris);
	}
	else vertices_ = 0;
	if (type & typeColor)
	{
		colors_ = (GLVertexArrayColor *)
			malloc(sizeof(GLVertexArrayColor) * noTris);
	}
	else colors_ = 0;
	if (type & typeTexture)
	{
		texCoord_ = (GLVertexArrayTexCoord*) 
			malloc(sizeof(GLVertexArrayTexCoord) * noTris);
	}
	else texCoord_ = 0;
}

GLVertexArray::~GLVertexArray()
{
	if (colors_) free(colors_);
	if (vertices_) free(vertices_);
	if (texCoord_) free(texCoord_);
	if (verticesVBO_) glDeleteBuffersARB(1, &verticesVBO_);
	if (colorsVBO_) glDeleteBuffersARB(1, &colorsVBO_);
	if (textureVBO_) glDeleteBuffersARB(1, &textureVBO_);
	if (glIsList(listNo_)) glDeleteLists(listNo_, 1);
}

void GLVertexArray::setVertex(int offset, GLfloat x, GLfloat y, GLfloat z)
{
	DIALOG_ASSERT(vertices_);
	DIALOG_ASSERT(offset < noTris_);
	GLVertexArrayVertex newVertex = { x ,y ,z };
	vertices_[offset] = newVertex;
}

void GLVertexArray::setColor(int offset, GLfloat r, GLfloat g, GLfloat b)
{
	DIALOG_ASSERT(colors_);
	DIALOG_ASSERT(offset < noTris_);
	GLVertexArrayColor newColor = { r ,g ,b };
	colors_[offset] = newColor;
}

void GLVertexArray::setTexCoord(int offset, GLfloat a, GLfloat b)
{
	DIALOG_ASSERT(texCoord_);
	DIALOG_ASSERT(offset < noTris_);
	GLVertexArrayTexCoord newTexCoord = { a, b };
	texCoord_[offset] = newTexCoord;
}

void GLVertexArray::setup()
{
	// ************ NOTE VBO TURNED OFF FOR NOW ************** //
	useVBO_ = false;

	if (useVBO_ && GLStateExtension::hasVBO())
	{
		useVBO_ = true;
		if (vertices_)
		{
			glGenBuffersARB(1, &verticesVBO_);
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, verticesVBO_);
			glBufferDataARB(GL_ARRAY_BUFFER_ARB, 
				sizeof(GLVertexArrayVertex) * noTris_, 
				vertices_,
				GL_STATIC_DRAW_ARB);
			if (glGetError() == GL_OUT_OF_MEMORY) useVBO_ = false;
		}

		if (colors_)
		{
			glGenBuffersARB(1, &colorsVBO_);
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, colorsVBO_);
			glBufferDataARB(GL_ARRAY_BUFFER_ARB, 
				sizeof(GLVertexArrayColor) * noTris_, 
				colors_,
				GL_STATIC_DRAW_ARB);
			if (glGetError() == GL_OUT_OF_MEMORY) useVBO_ = false;
		}

		if (texCoord_)
		{
			glGenBuffersARB(1, &textureVBO_);
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, textureVBO_);
			glBufferDataARB(GL_ARRAY_BUFFER_ARB, 
				sizeof(GLVertexArrayTexCoord) * noTris_, 
				texCoord_,
				GL_STATIC_DRAW_ARB);
			if (glGetError() == GL_OUT_OF_MEMORY) useVBO_ = false;
		}
	}
	else
	{
		useVBO_ = false;
	}
}

int GLVertexArray::getNoTris()
{ 
	if (prim_ == GL_TRIANGLES) return noTris_ / 3; 
	if (prim_ == GL_TRIANGLE_STRIP) return noTris_ - 2;
	if (prim_ == GL_QUAD_STRIP) return noTris_ - 2;
	if (prim_ == GL_QUADS) return noTris_ / 4;

	DIALOG_ASSERT(0);
	return 0;
}

void GLVertexArray::draw()
{
	unsigned int requiredState = 0;
	if (texCoord_)
	{
		requiredState = GLState::TEXTURE_ON;
		if (texture_ && texture_->getTexFormat() == GL_RGBA)
		{
			requiredState |= GLState::BLEND_ON;
		}
	}
	GLState textureState(requiredState);
	if (texCoord_ && texture_) texture_->draw();

	if (listNo_ == 0)
	{
		makeList();
	}
	if (listNo_ != 0)
	{
		glCallList(listNo_);
	}

	GLInfo::addNoTriangles(getNoTris());
}

void GLVertexArray::makeList()
{
	if (!setup_) 
	{
		setup();
		setup_ = true;
	}

	if (!useVBO_)
	{
		if (vertices_)
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, 0, &vertices_[0].x);
		}
		if (colors_)
		{
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(3, GL_FLOAT, 0, &colors_[0].r);
		}
		if (texCoord_)
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			if (secondTexture_) glClientActiveTextureARB(GL_TEXTURE1_ARB); 
			glTexCoordPointer(2, GL_FLOAT, 0, &texCoord_[0].a);
		}
	}
	else
	{
		if (vertices_)
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, verticesVBO_);
			glVertexPointer(3, GL_FLOAT, 0, 0);
		}
		if (colors_)
		{
			glEnableClientState(GL_COLOR_ARRAY);
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, colorsVBO_);
			glColorPointer(3, GL_FLOAT, 0, 0);
		}
		if (texCoord_)
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			if (secondTexture_) glClientActiveTextureARB(GL_TEXTURE1_ARB); 
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, textureVBO_);
			glTexCoordPointer(2, GL_FLOAT, 0, 0);
		}
	}

	if (!useVBO_) glNewList(listNo_ = glGenLists(1), GL_COMPILE);
		glDrawArrays(prim_, 0, noTris_);
	if (!useVBO_) glEndList();

	if (vertices_) glDisableClientState(GL_VERTEX_ARRAY);
	if (colors_) glDisableClientState(GL_COLOR_ARRAY);
	if (texCoord_)
	{	
		if (secondTexture_) glClientActiveTextureARB(GL_TEXTURE0_ARB); 
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}
