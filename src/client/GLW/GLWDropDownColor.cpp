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

#include <GLW/GLWDropDownColor.h>
#include <image/ImageFactory.h>
#include <client/ScorchedClient.h>

REGISTER_CLASS_SOURCE(GLWDropDownColor);

GLWDropDownColor::GLWDropDownColor(float x, float y, float w) :
	GLWDropDown(x, y, w), createdTexture_(false)
{
}

GLWDropDownColor::~GLWDropDownColor()
{
}

void GLWDropDownColor::addColor(Vector &color)
{
	if (!createdTexture_)
	{
		createdTexture_ = true;
		ImageHandle map = ImageFactory::loadImageHandle(
			S3D::getDataFile("data/windows/white.bmp"));
		colorTexture_.create(map);
	}

	GLWSelectorEntry entry(LANG_STRING(""), 0, false, &colorTexture_, 0);
	entry.getColor() = color;
	entry.getTextureWidth() = 32;
	addEntry(entry);
}

Vector &GLWDropDownColor::getCurrentColor()
{
	if (!getCurrentEntry()) return Vector::getNullVector();
	return getCurrentEntry()->getColor();
}

void GLWDropDownColor::setCurrentColor(Vector &color)
{
	int position = 0;
	std::list<GLWSelectorEntry>::iterator itor;
	for (itor = texts_.begin();
		itor != texts_.end();
		itor++)
	{
		GLWSelectorEntry &entry = *itor;
		if (color == entry.getColor())
		{
			current_ = &entry;
			break;
		}

		position++;
	}

	if (handler_)
	{
		handler_->select(id_, position, *current_);
	}
}

void GLWDropDownColor::draw()
{
	GLWDropDown::draw();

	if (getCurrentEntry())
	{
		GLState state(GLState::TEXTURE_OFF);

		Vector color = getCurrentColor();
		glColor3f(color[0], color[1], color[2]);

		float x = x_ + 12.0f;
		float y = y_ + 6.0f;
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(x, y);
			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(x + w_ - 44.0f, y);
			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(x + w_ - 44.0f, y + 14.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(x, y + 14.0f);
		glEnd();
	}
}
