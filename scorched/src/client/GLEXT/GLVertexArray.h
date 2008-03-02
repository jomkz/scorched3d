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

#if !defined(AFX_GLVERTEXARRAY_H__281612E4_C081_45C1_A049_B92631DBA524__INCLUDED_)
#define AFX_GLVERTEXARRAY_H__281612E4_C081_45C1_A049_B92631DBA524__INCLUDED_

#include <GLEXT/GLVertexSet.h>
#include <GLEXT/GLState.h>
#include <GLEXT/GLTexture.h>

class GLVertexArray : public GLVertexSet
{
public:
	struct GLVertexArrayVertex 
	{
		GLfloat	x;
		GLfloat	y;
		GLfloat	z;
	};
	struct GLVertexArrayColor
	{
		GLfloat r;
		GLfloat g;
		GLfloat b;
	};
	struct GLVertexArrayTexCoord
	{
		GLfloat a;
		GLfloat b;
	};

	enum GLVertexArrayType
	{
		typeVertex = 1,
		typeColor = 2,
		typeTexture = 4
	};

	GLVertexArray(GLenum prim, int noTris, 
		unsigned int type = GLVertexArray::typeVertex | GLVertexArray::typeColor, 
		GLTexture *texture = 0);
	virtual ~GLVertexArray();

	virtual void draw();
	virtual int getNoTris();
	void setSecondTexture() { secondTexture_ = true; }

	void setTexCoord(int offset, GLfloat a, GLfloat b);
	void setVertex(int offset, GLfloat x, GLfloat y, GLfloat z);
	void setColor(int offset, GLfloat r, GLfloat g, GLfloat b);

protected:
	int noTris_;
	bool setup_;
	bool secondTexture_;
	bool useVBO_;
	GLuint verticesVBO_;
	GLuint colorsVBO_;
	GLuint textureVBO_;
	unsigned int listNo_;
	GLTexture *texture_;
	GLenum prim_;
	GLVertexArrayTexCoord *texCoord_;
	GLVertexArrayVertex *vertices_;
	GLVertexArrayColor *colors_;

	virtual void setup();
	virtual void makeList();

private:
	GLVertexArray(const GLVertexArray &other);
	GLVertexArray &operator=(GLVertexArray &other);

};

#endif // !defined(AFX_GLVERTEXARRAY_H__281612E4_C081_45C1_A049_B92631DBA524__INCLUDED_)
