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

#include <common/Keyboard.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <GLEXT/GLState.h>
#include <console/Console.h>
#include <GLEXT/GLViewPort.h>
#include <GLW/GLWFont.h>
#include <GLW/GLWToolTip.h>

Console *Console::instance_ = 0;

Console *Console::instance()
{
	if (!instance_)
	{
		instance_ = new Console;
	}
	return instance_;
}

Console::Console() : 
	GameStateI("Console"),
	height_(0.0f), opening_(false), lines_(1000), 
	methods_(rules_, lines_), showCursor_(true)
{
	Logger::addLogger(this);
	font_ = GLWFont::instance()->getCourierFont();
}

Console::~Console()
{

}

void Console::logMessage(LoggerInfo &info)
{
	addLine(false, info.getMessage());
}

void Console::keyboardCheck(const unsigned state, float frameTime, 
							   char *buffer, unsigned int keyState,
							   KeyboardHistory::HistoryElement *history, int hisCount, 
							   bool &skipRest)
{
	// Use the history to ensure that no keystrokes are missed
	// regardless of the framerate.
	KEYBOARDKEY("CONSOLE", consoleKey);
	for (int i=0; i<hisCount; i++)
	{
		unsigned int dik = history[i].sdlKey;
		if (consoleKey->keyMatch(dik))
		{
			lines_.reset();
			if (currentLine_.empty()) opening_ = !opening_;
			else currentLine_ = "";
			skipRest = true;
		}
	}

	if (height_ > 0.0f)
	{
		// Use the history to ensure that no keystrokes are missed
		// regardless of the framerate.
		if (!skipRest)
		for (int i=0; i<hisCount; i++)
		{
			char c = history[i].representedKey;
			unsigned int dik = history[i].sdlKey;

			if (c >= ' ')
			{
				currentLine_ += c;
			}
			else 
			{
				switch (dik)
				{
				case SDLK_ESCAPE:
					if (currentLine_.empty()) opening_ = false;
					else currentLine_ = "";
					break;
				case SDLK_TAB:
					{
						std::list<ConsoleRule *> matches;
						const char *result = rules_.matchRule(currentLine_.c_str(), matches);
						if (result) currentLine_ = result;
						if (matches.size() > 1)
						{
							addLine(false, "-------------------");
							std::list<ConsoleRule *>::iterator itor;
							for (itor = matches.begin();
								itor != matches.end();
								itor++)
							{
								addLine(false, (*itor)->getName());
							}
						}
					}
					break;
				case SDLK_BACKSPACE:
				case SDLK_DELETE:
					currentLine_ = currentLine_.substr(0, currentLine_.length() - 1);
					break;
				case SDLK_RETURN:
					if (!currentLine_.empty())
					{
						parseLine(currentLine_.c_str());
						currentLine_.erase();
					}	
					break;
				case SDLK_UP:
					currentLine_ = lines_.getItem(-1);
					break;
				case SDLK_DOWN:
					currentLine_ = lines_.getItem(+1);
					break;
				case SDLK_PAGEUP:
					lines_.scroll(-5);
					break;
				case SDLK_PAGEDOWN:
					lines_.scroll(+5);
					break;	
				case SDLK_HOME:
					lines_.scroll(-((int)lines_.getMaxLines()));
					break;
				case SDLK_END:
					lines_.scroll(int(lines_.getMaxLines()));
					break;
				}
			}
		}
		skipRest = true;
	}
	else
	{
		std::list<KeyboardKey *> &keys = Keyboard::instance()->getCommandKeys();
		std::list<KeyboardKey *>::iterator keyItor = keys.begin();
		std::list<KeyboardKey *>::iterator keyEndItor = keys.end();
		for (; keyItor != keyEndItor; keyItor++)
		{
			KeyboardKey *key = (*keyItor);
			if (key->keyDown(buffer, keyState, false))
			{
				addLine(true, key->getName());
			}
		}
	}
}

void Console::simulate(const unsigned state, float frameTime)
{
	const GLfloat dropSpeed = 1200.0f;
	if (opening_)
	{
		height_ += frameTime * dropSpeed;
	}
	else
	{
		height_ -= frameTime * dropSpeed;
		if (height_ < 0.0f) height_ = 0.0f;
	}

	static float ctime = 0;
	ctime += frameTime;
	if (ctime > 0.5f)
	{
		ctime = 0.0f;
		showCursor_ = !showCursor_;
	}
}

void Console::draw(const unsigned state)
{
	if (height_ <= 0.0f) return;

	GLState currentState(GLState::DEPTH_OFF | GLState::TEXTURE_OFF | GLState::BLEND_ON);

	GLfloat width = (GLfloat) GLViewPort::getWidth();
	GLfloat top = (GLfloat) GLViewPort::getHeight();

	drawBackdrop(width, top);
	drawText(width, top);
}

void Console::drawBackdrop(float width, float top)
{
	if (height_ > top * .85f) height_ = top * .85f;

	GLWToolTip::instance()->clearToolTip(
		0.0f, top - height_ + 10.0f,
		width, height_);

	glColor4f(0.0f, 0.0f, 0.0f, 0.9f);
	glBegin(GL_QUADS);
		glVertex2f(0.0f, top - height_ + 10.0f);
		glVertex2f(width, top - height_ + 10.0f);
		glVertex2f(width, top);
		glVertex2f(0.0f, top);

		glVertex2f(10.0f, top - height_);
		glVertex2f(width - 10.0f, top - height_);
		glVertex2f(width, top - height_ + 10.0f);
		glVertex2f(0.0f, top - height_ + 10.0f);
	glEnd();

	glColor4f(0.1f, 0.1f, 0.1f, 1.0f);
	glBegin(GL_LINE_STRIP);
		glVertex2f(0.0f, top - height_ + 10.0f);
		glVertex2f(10.0f, top - height_);
		glVertex2f(width - 10.0f, top - height_);
		glVertex2f(width, top - height_ + 10.0f);
	glEnd();
}

void Console::drawText(float width, float top)
{
	static Vector color(1.0f, 1.0f, 1.0f);
	font_->draw(color, 14,
		10.0f, top - (height_ - 14.0f), 0.0f, 
		S3D::formatStringBuffer("> %s%c", 
		currentLine_.c_str(),
		(showCursor_?'_':' ')));

	lines_.drawLines(font_, top - (height_ - 14.0f), top, width);
}

void Console::parseLine(const char *line)
{
	std::list<std::string> linesToAdd;
	std::string result;
	rules_.addLine(line, result, linesToAdd);

	lines_.addLine(result.c_str(), true);
	std::list<std::string>::iterator iter;
	for (iter = linesToAdd.begin();
		iter != linesToAdd.end();
		iter++)
	{
		lines_.addLine(iter->c_str(), false);
	}
}

void Console::addLine(bool parse, const std::string &text)
{
	if (parse)
	{
		parseLine(text.c_str());
	}
	else
	{
		lines_.addLine(text.c_str(), false);
	}
}

bool Console::addFunction(const char *name, 
							ConsoleRuleFnI *user, 
							ConsoleRuleType type, 
							ConsoleRuleAccessType access)
{
	ConsoleRuleFn *rule = new ConsoleRuleFn(name, user, type, access);
	if (!rules_.addRule(rule))
	{
		delete rule;
		return false;
	}
	return true;
}

bool Console::removeFunction(const char *name)
{
	ConsoleRule *rule = rules_.removeRule(name);
	delete rule;

	return (rule!=0);
}

bool Console::addMethod(const char *name,
						  ConsoleRuleMethodI *user)
{
	ConsoleRuleMethod *rule = new ConsoleRuleMethod(name, user);
	if (!rules_.addRule(rule))
	{
		delete rule;
		return false;
	}
	return true;
}

bool Console::removeMethod(const char *name)
{
	ConsoleRule *rule = rules_.removeRule(name);
	delete rule;

	return (rule!=0);
}
