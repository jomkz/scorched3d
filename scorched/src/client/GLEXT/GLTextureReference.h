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

#if !defined(__INCLUDE_TextureReferenceh_INCLUDE__)
#define __INCLUDE_TextureReferenceh_INCLUDE__

#include <image/ImageID.h>
#include <GLEXT/GLTexture.h>

class GLTextureReference : public GLTextureBase
{
public:
	enum TextureState 
	{
		eMipMap = 1,
		eTextureClamped = 2
	};

	GLTextureReference();
	GLTextureReference(const ImageID &imageId, unsigned texState = eMipMap);
	~GLTextureReference();

	void setImageID(const ImageID &imageId, unsigned texState = eMipMap);

	virtual void draw(bool force = false); 
	
	ImageID &getImageID() { return imageId_; }
	GLTexture *getTexture();
	void reset();
protected:
	GLTexture *texture_;
	ImageID imageId_;
	unsigned texState_;
private:
	GLTextureReference(const GLTextureReference &other);
	GLTextureReference &operator=(const GLTextureReference &other);
};

#endif
