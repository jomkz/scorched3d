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

#if !defined(AFX_GLCONSOLE_H__516D85F7_420B_43EB_B0BE_563DCBE1B143__INCLUDED_)
#define AFX_GLCONSOLE_H__516D85F7_420B_43EB_B0BE_563DCBE1B143__INCLUDED_

#include <common/LoggerI.h>
#include <engine/GameStateI.h>
#include <GLEXT/GLFont2d.h>
#include <GLEXT/GLConsoleMethods.h>
#include <GLEXT/GLConsoleRuleFn.h>
#include <GLEXT/GLConsoleRuleMethod.h>

class GLConsole : public GameStateI, public LoggerI
{
public:
	static GLConsole *instance();

	bool addFunction(const char *name, 
		GLConsoleRuleFnI *user,
		GLConsoleRuleType type, 
		GLConsoleRuleAccessType access);
	bool removeFunction(const char *name);

	bool addMethod(const char *name,
		GLConsoleRuleMethodI *user);
	bool removeMethod(const char *name);

	void addLine(bool parse, const std::string &line);

	std::deque<GLConsoleLine *> &getLines() { return lines_.getLines(); }

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
	static GLConsole *instance_;
	float height_;
	bool opening_;
	bool showCursor_;
	GLFont2d *font_;
	GLConsoleLines lines_;
	GLConsoleRules rules_;
	GLConsoleMethods methods_;
	std::string currentLine_;

	void drawBackdrop(float width, float top);
	void drawText(float width, float top);
	void parseLine(const char *line);

private:
	GLConsole();
	virtual ~GLConsole();

};

#endif // !defined(AFX_GLCONSOLE_H__516D85F7_420B_43EB_B0BE_563DCBE1B143__INCLUDED_)
