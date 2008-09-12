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

#if !defined(AFX_ConsoleImpl_H__516D85F7_420B_43EB_B0BE_563DCBE1B143__INCLUDED_)
#define AFX_ConsoleImpl_H__516D85F7_420B_43EB_B0BE_563DCBE1B143__INCLUDED_

#include <common/LoggerI.h>
#include <engine/GameStateI.h>
#include <GLEXT/GLFont2d.h>
#include <console/Console.h>
#include <console/ConsoleMethods.h>
#include <console/ConsoleLines.h>
#include <console/ConsoleRules.h>

class ConsoleImpl : public GameStateI, public LoggerI, public Console
{
public:
	ConsoleImpl();
	virtual ~ConsoleImpl();

	void init();

	virtual void addRule(ConsoleRule *rule) { rules_.addRule(rule); }
	virtual void removeRule(ConsoleRule *rule) { rules_.removeRule(rule); }

	virtual void addLine(bool parse, const std::string &line);
	virtual void clear() { lines_.clear(); }
	virtual void help();

	std::deque<ConsoleLine *> &getLines() { return lines_.getLines(); }

	// Inherited from GameStateI
	virtual void simulate(const unsigned state, float frameTime);
	virtual void draw(const unsigned state);
	virtual void keyboardCheck(const unsigned state, float frameTime, 
							   char *buffer, unsigned int keyState,
							   KeyboardHistory::HistoryElement *history, int hisCount, 
							   bool &skipRest);

	// Inherited from LoggerI
	virtual void logMessage(LoggerInfo &info);

protected:
	float height_;
	bool opening_;
	bool showCursor_;
	GLFont2d *font_;
	ConsoleLines lines_;
	ConsoleRules rules_;
	ConsoleMethods methods_;
	std::string currentLine_;
	int historyPosition_;
	std::deque<std::string> history_;

	void resetPositions();
	void drawBackdrop(float width, float top);
	void drawText(float width, float top);
};

#endif // !defined(AFX_ConsoleImpl_H__516D85F7_420B_43EB_B0BE_563DCBE1B143__INCLUDED_)
