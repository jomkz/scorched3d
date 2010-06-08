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

#include <GLW/GLWIcon.h>
#include <GLEXT/GLState.h>
#include <GLEXT/GLTextureStore.h>
#include <XML/XMLParser.h>
#include <common/Defines.h>

REGISTER_CLASS_SOURCE(GLWIcon);

GLWIcon::GLWIcon(float x, float y, float w, float h, GLTexture *texture) : 
	GLWidget(x, y, w, h),
	texture_(texture)
{
	tooltipTransparent_ = true;
}

GLWIcon::~GLWIcon()
{
}

void GLWIcon::draw()
{
	if (texture_)
	{
		GLState state(GLState::TEXTURE_ON | GLState::BLEND_ON);
		glColor3f(1.0f, 1.0f, 1.0f);
		texture_->draw();

		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(x_, y_);
			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(x_ + w_, y_);
			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(x_ + w_, y_ + h_);
			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(x_, y_ + h_);
		glEnd();
	}

	GLWidget::draw();
}

bool GLWIcon::initFromXML(XMLNode *node)
{
	if (!GLWidget::initFromXML(node)) return false;

	XMLNode *bitmapNode, *bitmapANode, *invertNode;
	if (!node->getNamedChild("bitmap", bitmapNode)) return false;
	if (!node->getNamedChild("bitmapa", bitmapANode)) return false;
	if (!node->getNamedChild("invert", invertNode)) return false;

	bool invert = (0 == strcmp(invertNode->getContent(), "true"));
	if (bitmapNode && bitmapANode)
	{
		std::string bitmapName = 
			S3D::getModFile(bitmapNode->getContent());
		std::string bitmapAName = 
			S3D::getModFile(bitmapANode->getContent());

		texture_ = GLTextureStore::instance()->loadTexture(
			ImageID(S3D::eModLocation, 
			bitmapNode->getContent(),
			bitmapANode->getContent(), 
			invert));
		if (!texture_) return false;

		texture_->draw();
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	}
	return true;
}
