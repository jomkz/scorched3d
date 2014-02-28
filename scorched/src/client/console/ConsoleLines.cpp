////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2013
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
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
////////////////////////////////////////////////////////////////////////////////

#include <console/ConsoleLines.h>
#include <common/DefinesString.h>
#include <string>

unsigned ConsoleLine::nextLineNumber_ = 0;

ConsoleLine::ConsoleLine() :
	lineType_(eNone), lineNumber_(0)
{

}

ConsoleLine::~ConsoleLine()
{

}

void ConsoleLine::set(const LangString &line, LineType type)
{
	line_ = line;
	lineType_ = type;
	if (lineType_ == eCommand)
	{
		lineNumber_ = ++nextLineNumber_;
	}
	if (lineType_ == eCommandCont)
	{
		lineNumber_ = nextLineNumber_;
	}
	lineNumberStr_= LANG_STRING(S3D::formatStringBuffer("%4i", lineNumber_));
}

ConsoleLines::ConsoleLines(int maxLines) :
	maxLines_(maxLines), currentLine_(0)
{
}

ConsoleLines::~ConsoleLines()
{
}

void ConsoleLines::clear()
{
	currentLine_ = 0;
	while (!lines_.empty())
	{
		ConsoleLine *line = lines_.front();
		lines_.pop_front();
		delete line;
	}
}

void ConsoleLines::scroll(int lines)
{
	currentLine_ -= lines;
	if (currentLine_ < 0) currentLine_ = 0;
	else if (currentLine_ >= (int) lines_.size()) 
		currentLine_ = (int) lines_.size();
}

void ConsoleLines::addLine(const std::string &originalText, bool showPointer)
{
	LangString langStringText(LANG_STRING(originalText)), buffer;

	int section = 0;
	for (const unsigned int *a=langStringText.c_str(); 
		*a; 
		a++)
	{
		if (*a != '\n') buffer.push_back(*a);
		if (buffer.size() > 80 || (buffer.size() > 65 && *a ==' ') || *a=='\n')
		{
			addSmallLine(section++, buffer, showPointer);
			buffer.clear();
		}
	}
	if (!buffer.empty()) 
	{
		addSmallLine(section++, buffer, showPointer);
	}
}

void ConsoleLines::addSmallLine(int sectionNo, const LangString &text, bool showPointer)
{
	ConsoleLine::LineType type = ConsoleLine::eNone;
	if (showPointer)
	{
		if (sectionNo == 0) type = ConsoleLine::eCommand;
		else type = ConsoleLine::eCommandCont;
	}

	if (lines_.size() == maxLines_)
	{
		// We need to reuse the lines
		// reuse the first line
		ConsoleLine *line = lines_.back();
		lines_.pop_back();
		line->set(text, type);
		lines_.push_front(line);
	}
	else
	{
		// Add a new line
		ConsoleLine *line = new ConsoleLine;
		line->set(text, type);
		lines_.push_front(line);
	}

	// Set the lookat and cursor to point at the new line
	if (currentLine_ != 0)
	{
		currentLine_ ++;
		if (currentLine_ > (int) lines_.size()) 
			currentLine_ = (int) lines_.size();
	}
}
