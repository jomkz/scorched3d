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

#if !defined(__INCLUDE_GLWIconh_INCLUDE__)
#define __INCLUDE_GLWIconh_INCLUDE__

#include <GLEXT/GLTextureReference.h>
#include <GLW/GLWidget.h>

class GLWIcon : public GLWidget
{
public:
	GLWIcon(float x = 0.0f, float y = 0.0f, 
		float w = 0.0f, float h = 0.0f);
	GLWIcon(float x, float y, 
		float w, float h, 
		const ImageID &imageId);
	virtual ~GLWIcon();

	virtual void draw();
	virtual bool initFromXML(XMLNode *node);

	void setTextureImage(const ImageID &imageId);

	REGISTER_CLASS_HEADER(GLWIcon);
protected:
	GLTextureReference texture_;
};

#endif
