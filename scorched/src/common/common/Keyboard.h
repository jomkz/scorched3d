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

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <common/KeyboardHistory.h>
#include <common/KeyboardKey.h>
#include <string>
#include <list>
#include <map>

#define MAX_KEYBDHIST 1024
#define KEYBOARDKEY(name, key) static KeyboardKey * key = Keyboard::instance()->getKey( name );

class Keyboard
{
public:
	static Keyboard *instance();

	bool loadKeyFile(bool loadDefaults = false);
	bool saveKeyFile();
	bool init();

	char *getkeyboardbuffer(unsigned int &bufCnt);
	unsigned int getKeyboardState();
	KeyboardHistory::HistoryElement *getkeyboardhistory(unsigned int &histCnt); 

	void processKeyboardEvent(SDL_Event &event);
	static bool &getDvorak();

	KeyboardKey *getKey(const char *name);
	std::list<std::string> &getKeyList() { return keyList_; }
	std::list<KeyboardKey *> &getCommandKeys() { return commandKeys_; }
	std::map<std::string, KeyboardKey *> &getKeyMap() { return keyMap_; }
			                       
protected:
	static Keyboard *instance_;
	static bool dvorak_; // TODO // FIX ME NOT WORKING
	KeyboardHistory::HistoryElement keybHist_[MAX_KEYBDHIST];
	int keybHistCnt_;
	void clear();
	bool loadKeyFile(const std::string &fileName, bool masterFile);

	std::map<std::string, KeyboardKey *> keyMap_;
	std::list<KeyboardKey *> commandKeys_;
	std::list<std::string> keyList_;
                       
private:
	Keyboard();
	virtual ~Keyboard();

};

#endif /* _KEYBOARD_H_ */
