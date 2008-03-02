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

#if !defined(AFX_GLTEXTURE_H__9DA92C48_B9A4_4517_AB9F_55141CCDA817__INCLUDED_)
#define AFX_GLTEXTURE_H__9DA92C48_B9A4_4517_AB9F_55141CCDA817__INCLUDED_

#include <GLEXT/GLState.h>
#include <image/Image.h>
#include <GLEXT/GLTextureBase.h>

class GLTexture : public GLTextureBase
{
public:
	GLTexture();
	virtual ~GLTexture();

	virtual void draw(bool force = false);

	bool create(Image &bitmap, 
				bool mipMap = true);
	bool create(const void * data, 
				GLint width, 
				GLint height, 
				GLint components, 
				GLint alignment = 4,
				GLenum format = GL_RGB, 
				bool mipMap = true);
	bool create(GLint width, 
				GLint height, 
				GLenum format = GL_RGB);
	bool createBufferTexture(GLint width, 
				GLint height,
				bool depthTex);
	bool replace(Image &bitmap,
						bool mipMap = true);

	bool textureValid();
	GLenum getTexType() { return texType_; }
	GLenum getTexFormat() { return texFormat_; }
	GLuint getTexName() { return texNum_; }
	int getWidth() { return width_; }
	int getHeight() { return height_; }

	static unsigned int getTextureSpace() { return textureSpace_; }
	static bool validateSize(int size);

protected:
	static unsigned int textureSpace_;
	unsigned int usedSpace_;
	int width_, height_;
	GLuint texNum_;
	GLenum texType_;
	GLenum texFormat_;

	bool createTexture(const void * data, 
						GLint width, 
						GLint height, 
						GLint components, 
						GLint alignment,
						GLenum format, 
						bool mipMap);
	bool createObject();
};

#endif // !defined(AFX_GLTEXTURE_H__9DA92C48_B9A4_4517_AB9F_55141CCDA817__INCLUDED_)
