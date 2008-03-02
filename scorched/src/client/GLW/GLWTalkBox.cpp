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

#include <GLW/GLWTalkBox.h>
#include <GLW/GLWFont.h>
#include <GLW/GLWPanel.h>
#include <GLW/GLWWindowManager.h>
#include <GLEXT/GLState.h>
#include <GLEXT/GLConsole.h>
#include <XML/XMLParser.h>
#include <common/Defines.h>

REGISTER_CLASS_SOURCE(GLWTalkBox);

GLWTalkBox::GLWTalkBox() :
	ctime_(0.0f), cursor_(false), maxTextLen_(0),
	parentSized_(false), mode_(eSay), defaultMode_(eSay)
{
}

GLWTalkBox::~GLWTalkBox()
{
}

void GLWTalkBox::simulate(float frameTime)
{
	ctime_ += frameTime;
	if (ctime_ > 0.5f)
	{
		ctime_ = 0.0f;
		cursor_ = !cursor_;
	}
}

void GLWTalkBox::draw()
{
	if (parent_ && parentSized_)
	{
		w_ = parent_->getW();
		h_ = parent_->getH();
	}

	GLWidget::draw();
	glColor4f(0.4f, 0.6f, 0.8f, 0.6f);

	{
		GLState currentStateBlend(GLState::BLEND_ON);
		glBegin(GL_TRIANGLE_FAN);
			glVertex2f(x_ + 20.0f, y_ + 5.0f);
			glVertex2f(x_ + 20.0f, y_);
			drawRoundBox(x_, y_, w_, h_, 10.0f);
			glVertex2f(x_ + 20.0f, y_);
		glEnd();
	}

	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINE_LOOP);
		drawRoundBox(x_, y_, w_, h_, 10.0f);
	glEnd();

	Vector white(0.9f, 0.9f, 1.0f);
	GLWFont::instance()->getGameFont()->drawWidthRhs(
		w_ - 10.0f,
		white, 12,
		x_ + 5.0f, y_ + 5.0f, 0.0f, 
		S3D::formatStringBuffer("%s: %s%s", 
		((mode_ == eSay)?"Say":"Team Say"), text_.c_str(), cursor_?" ":"_"));
}

void GLWTalkBox::keyDown(char *buffer, unsigned int keyState, 
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest)
{
	for (int i=0; i<hisCount; i++)
	{
		char c = history[i].representedKey;
		unsigned int dik = history[i].sdlKey;
		if (dik == SDLK_BACKSPACE || dik == SDLK_DELETE)
		{
			if (!text_.empty())
			{
				text_ = text_.substr(0, text_.length() - 1);
			}
		}
		else if (dik == SDLK_ESCAPE)
		{
			GLWWindowManager::instance()->hideWindow(parent_->getId());
		}
		else if (dik == SDLK_RETURN)
		{
			if (!text_.empty())
			{
				char *text = (char *) text_.c_str();
				while (char *c = strchr(text, '"'))
				{
					*c = '\'';
				}

				GLConsole::instance()->addLine(true, 
					S3D::formatStringBuffer("%s \"%s\"",
					((mode_ == eTeamSay)?"Teamsay":"Say"),
					text));
				text_ = "";
			}
			GLWWindowManager::instance()->hideWindow(parent_->getId());
		}
		else if (c >= ' ')
		{
			if ((maxTextLen_==0) || ((int) text_.size() < maxTextLen_))
			{
				text_ += c;

				if (text_[0] == '\\' || text_[0] == '/')
				{
					if (0 == stricmp(&text_[1], "say ") ||
						0 == stricmp(&text_[1], "s "))
					{
						text_ = "";
						mode_ = eSay;
					}
					else if (0 == stricmp(&text_[1], "team ") ||
						0 == stricmp(&text_[1], "t "))
					{
						text_ = "";
						mode_ = eTeamSay;
					}
				}
			}
		}
	}
	skipRest = true;
}

void GLWTalkBox::display()
{
	mode_ = defaultMode_;
	text_ = "";
}

bool GLWTalkBox::initFromXML(XMLNode *node)
{
	if (!GLWidget::initFromXML(node)) return false;

	if (!node->getNamedChild("parentsized", parentSized_)) return false;

	std::string mode;
	if (!node->getNamedChild("mode", mode)) return false;
	defaultMode_ = ((0 == strcmp(mode.c_str(), "say"))?eSay:eTeamSay);

	return true;
}
