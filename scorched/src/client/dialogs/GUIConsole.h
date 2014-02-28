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

#if !defined(AFX_GUIConsole_H__516D85F7_420B_43EB_B0BE_563DCBE1B143__INCLUDED_)
#define AFX_GUIConsole_H__516D85F7_420B_43EB_B0BE_563DCBE1B143__INCLUDED_

#include <client/ClientUISync.h>
#include <scorched3dc/InputHandlerKeyboard.h>
#include <common/LoggerI.h>
#include <vector>

class GUIConsoleClientUISyncAction : public ClientUISyncAction 
{
public:
	GUIConsoleClientUISyncAction(const CEGUI::String &inMsg);
	virtual ~GUIConsoleClientUISyncAction();

	// ClientUISyncAction
	virtual void performUIAction();

protected:
	CEGUI::String inMsg_;
};

class GUIConsoleTabCompleteUISyncAction : public ClientUISyncAction 
{
public:
	GUIConsoleTabCompleteUISyncAction(const CEGUI::String &currentText);
	virtual ~GUIConsoleTabCompleteUISyncAction();

	// ClientUISyncAction
	virtual void performUIAction();

protected:
	CEGUI::String currentText_;
};

class GUIConsole : public LoggerI, public InputHandlerKeyboard
{
public:
	static GUIConsole *instance();

	void setVisible(bool visible);
	bool isVisible(); 
 
	void outputText(const CEGUI::String &inMsg, bool command); 
	void setText(const CEGUI::String &text);

	// LoggerI
	virtual void logMessage(LoggerInfo &info);

	// InputHandlerKeyboard
	virtual void keyPressed(const OIS::KeyEvent &arg);
	virtual void keyReleased(const OIS::KeyEvent &arg);

protected:
	void create();   
	void tabComplete();
	bool handle_TextSubmitted(const CEGUI::EventArgs &e);
	bool handle_keyDownSubmitted(const CEGUI::EventArgs &e);
 
	CEGUI::Window *consoleWindow_;
	CEGUI::Editbox* editBox_;
	CEGUI::Listbox *outputWindow_;
	int commandHistoryIndex_;
	std::vector<CEGUI::String> commandHistory_;

private:
	GUIConsole();
	virtual ~GUIConsole();

};

#endif // !defined(AFX_GUIConsole_H__516D85F7_420B_43EB_B0BE_563DCBE1B143__INCLUDED_)
