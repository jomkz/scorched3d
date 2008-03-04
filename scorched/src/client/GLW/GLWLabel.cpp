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

#include <GLW/GLWLabel.h>
#include <GLW/GLWFont.h>

REGISTER_CLASS_SOURCE(GLWLabel);

GLWLabel::GLWLabel(float x, float y, const std::string &labelText, 
	float size, unsigned int flags) : 
	GLWidget(x, y, 0.0f, 20.0f), size_(size),
	color_(GLWFont::widgetFontColor), flags_(flags)
{
	setText(labelText);
	tooltipTransparent_ = true;
}

GLWLabel::~GLWLabel()
{

}

void GLWLabel::setColor(const Vector &color)
{
	color_ = color;
}

void GLWLabel::setSize(float size)
{
	size_ = size;
	w_ = 0.0f;
}

void GLWLabel::setText(const std::string &text)
{ 
	labelText_ = text.c_str(); 
	if (flags_ & eMultiLine)
	{
		labelTexts_.clear();
		char *token = strtok((char *) text.c_str(), "\n");
		while(token != 0)
		{
			labelTexts_.push_back(token);
			token = strtok(0, "\n");
		}
	}

	w_ = 0.0f;
}

void GLWLabel::calcWidth()
{
	if (w_ == 0.0f)
	{
		if (flags_ & eMultiLine)
		{
			for (int i=0; i<(int) labelTexts_.size(); i++)
			{
				const char *text = labelTexts_[i].c_str();
				w_ = MAX(w_, GLWFont::instance()->getGameFont()->getWidth(
					size_, (char *) text));
			}
			h_ = float(labelTexts_.size()) * 20.0f / 14.0f * size_;
		}
		else
		{
			w_ = GLWFont::instance()->getGameFont()->getWidth(
				size_, (char *) labelText_.c_str());
			h_ = 20.0f / 14.0f * size_;
		}
	}
}

void GLWLabel::draw()
{
	GLWidget::draw();

	glColor3f(1.0f, 0.0f, 0.0f);
	calcWidth();

	if (flags_ & eMultiLine)
	{
		for (int i=0; i<(int) labelTexts_.size(); i++)
		{
			const char *text = labelTexts_[labelTexts_.size() - (i + 1)].c_str();

			GLWFont::instance()->getGameFont()->draw(
				color_, size_,
				x_, y_ + 6.0f + float(i) * 20.0f / 14.0f * size_, 0.0f, (char *) text);
		}
	}
	else
	{
		GLWFont::instance()->getGameFont()->draw(
			color_, size_,
			x_, y_ + 6.0f, 0.0f, (char *) labelText_.c_str());
	}
}
