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

#if !defined(AFX_ConsoleLINES_H__2DA65C94_3E52_43C6_B75B_D0CEDBF6A9DE__INCLUDED_)
#define AFX_ConsoleLINES_H__2DA65C94_3E52_43C6_B75B_D0CEDBF6A9DE__INCLUDED_

#include <GLEXT/GLFont2d.h>
#include <deque>
#include <string>

class ConsoleLine
{
public:
	enum LineType
	{
		eNone,
		eCommand,
		eCommandCont
	};

	ConsoleLine();
	virtual ~ConsoleLine();

	void set(const char *line, LineType type);
	void drawLine(float x, float y, GLFont2d *font);

	LineType getLineType() { return lineType_; }
	const char *getLine() { return line_.c_str(); }

protected:
	unsigned int lineNumber_;
	std::string line_;
	std::string lineNumberStr_;
	static unsigned nextLineNumber_;
	LineType lineType_;

};

class ConsoleLines  
{
public:
	ConsoleLines(int maxLines);
	virtual ~ConsoleLines();

	void addLine(const char *line, bool showPointer);
	void drawLines(GLFont2d *font, float startHeight, float totalHeight, float totalWidth);

	void clear();

	void reset() { currentLine_ = 0; }
	void scroll(int lines);
	const char *getItem(int lines);

	int getMaxLines() { return maxLines_; }
	std::deque<ConsoleLine *> &getLines() { return lines_; }

protected:
	std::deque<ConsoleLine *> lines_;
	int maxLines_;
	int currentLine_;

	void addSmallLine(int section, const char *line, bool showPointer);

};

#endif // !defined(AFX_ConsoleLINES_H__2DA65C94_3E52_43C6_B75B_D0CEDBF6A9DE__INCLUDED_)
