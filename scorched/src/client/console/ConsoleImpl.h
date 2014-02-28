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

#if !defined(AFX_ConsoleImpl_H__516D85F7_420B_43EB_B0BE_563DCBE1B143__INCLUDED_)
#define AFX_ConsoleImpl_H__516D85F7_420B_43EB_B0BE_563DCBE1B143__INCLUDED_

#include <console/Console.h>
#include <console/ConsoleMethods.h>
#include <console/ConsoleRules.h>
#include <client/ClientUISync.h>

class GUIConsoleWriteAction : public ClientUISyncAction 
{
public:
	GUIConsoleWriteAction(const CEGUI::String &text, bool command);
	virtual ~GUIConsoleWriteAction();
	
	// ClientUISyncAction
	virtual void performUIAction();

private:
	bool command_;
	CEGUI::String text_;
};

class GUIConsoleUpdateTextAction : public ClientUISyncAction 
{
public:
	GUIConsoleUpdateTextAction(const CEGUI::String &text);
	virtual ~GUIConsoleUpdateTextAction();
	
	// ClientUISyncAction
	virtual void performUIAction();

private:
	CEGUI::String text_;
};

class ConsoleImpl : public Console
{
public:
	ConsoleImpl();
	virtual ~ConsoleImpl();

	virtual void addRule(ConsoleRule *rule) { rules_.addRule(rule); }
	virtual void removeRule(ConsoleRule *rule) { rules_.removeRule(rule); }

	virtual void addLine(bool parse, const CEGUI::String &line);
	virtual void help();

	virtual void matchRule(const CEGUI::String &line);

protected:
	ConsoleRules rules_;
	ConsoleMethods methods_;
};

#endif // !defined(AFX_ConsoleImpl_H__516D85F7_420B_43EB_B0BE_563DCBE1B143__INCLUDED_)
