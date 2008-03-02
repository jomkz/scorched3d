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
#include <XML/XMLFile.h>

bool Keyboard::dvorak_ = false;

Keyboard *Keyboard::instance_ = 0;

Keyboard *Keyboard::instance()
{
	if (!instance_)
	{
		instance_ = new Keyboard;
	}

	return instance_;
}

Keyboard::Keyboard() : keybHistCnt_(0)
{

}

Keyboard::~Keyboard()
{

}

bool Keyboard::init()
{
 	SDL_EnableUNICODE(1);
	SDL_EnableKeyRepeat(250, 100);
	return true;
}

unsigned int Keyboard::getKeyboardState()
{
	// Ingore capslock, numlock and scroll lock
	const unsigned int keymask = KMOD_LSHIFT | KMOD_RSHIFT | KMOD_LCTRL | KMOD_RCTRL |
		KMOD_LALT | KMOD_RALT | KMOD_LMETA | KMOD_RMETA;

	return SDL_GetModState() & keymask;
}

char *Keyboard::getkeyboardbuffer(unsigned int &bufCnt)
{
 	bufCnt = SDLK_LAST;
 	return (char *) SDL_GetKeyState(NULL);
}
 
KeyboardHistory::HistoryElement *Keyboard::getkeyboardhistory(unsigned int &histCnt)
{
 	histCnt = keybHistCnt_;
 	keybHistCnt_=0;

 	return keybHist_;
}
 
void Keyboard::processKeyboardEvent(SDL_Event &event)
{
	// Check we have enough space in the array
	if (keybHistCnt_ >= MAX_KEYBDHIST) return;

	// Check we are adding the correct key type
	if (event.type != SDL_KEYDOWN) return;

	// Check we don't have an international character
	if ( (event.key.keysym.unicode & 0xFF80) == 0 )
	{
		char ch = event.key.keysym.unicode & 0x7F;

		KeyboardHistory::HistoryElement newElement =
		{
			event.key.keysym.sym,
			ch
		};

		// Add characater and symbol to history
		keybHist_[keybHistCnt_++] = newElement;
	}
}

bool &Keyboard::getDvorak()
{
	return dvorak_;
}

void Keyboard::clear()
{
	std::map<std::string, KeyboardKey *>::iterator itor;
	for (itor = keyMap_.begin();
		itor != keyMap_.end();
		itor++)
	{
		delete (*itor).second;
	}
	keyMap_.clear();
	keyList_.clear();
	commandKeys_.clear();	
}

bool Keyboard::saveKeyFile()
{
	XMLNode keysNode("keys");
	std::list<std::string>::iterator itor;
	for (itor = keyList_.begin();
		itor != keyList_.end();
		itor++)
	{
		KeyboardKey *key = getKey((*itor).c_str());
		if (!key || !key->getChanged()) continue;

		XMLNode *keyNode = new XMLNode("keyentry");
		if (key->getNameIsCommand())
			keyNode->addChild(new XMLNode("command", key->getName()));
		else keyNode->addChild(new XMLNode("name", key->getName()));
		keyNode->addChild(new XMLNode("title", key->getTitle()));
		keyNode->addChild(new XMLNode("description", key->getDescription()));
		keyNode->addChild(new XMLNode("group", S3D::formatStringBuffer("%i", key->getGroup())));

		std::vector<KeyboardKey::KeyEntry> &keys = key->getKeys();
		std::vector<KeyboardKey::KeyEntry>::iterator subitor;
		for (subitor = keys.begin();
			subitor != keys.end();
			subitor++)
		{
			KeyboardKey::KeyEntry &subentry = (*subitor);
			const char *name = "";
			const char *state = "";
			KeyboardKey::translateKeyNameValue(subentry.key, name);
			KeyboardKey::translateKeyStateValue(subentry.state, state);
			XMLNode *subnode = new XMLNode("key", name);
			if (strcmp(state, "NONE") != 0) subnode->addParameter(
				new XMLNode("state", state, XMLNode::XMLParameterType));
			keyNode->addChild(subnode);
		}

		keysNode.addChild(keyNode);
	}

	std::string fileName = S3D::getSettingsFile("keys.xml");
	if (!keysNode.writeToFile(fileName)) return false;

	return true;
}

bool Keyboard::loadKeyFile(bool loadDefaults)
{
	// Empty the current keys
	clear();

	// Load the master list of keys
	if (!loadKeyFile(S3D::getDataFile("data/keys.xml"), true)) return false;

	// Replace with any custom keys
	std::string fileName = S3D::getSettingsFile("keys.xml");
	if (S3D::fileExists(fileName) && !loadDefaults)
	{
		if (!loadKeyFile(fileName, false)) return false;
	}

	// Build the command list
	std::map<std::string, KeyboardKey *>::iterator keyItor;
	for (keyItor = keyMap_.begin();
		keyItor != keyMap_.end();
		keyItor++)
	{
		KeyboardKey *key = keyItor->second;		

		// Add a command
		if (key->getNameIsCommand()) commandKeys_.push_back(key);

		// Find if this key is already in use
		std::map<std::string, KeyboardKey *>::iterator dupeItor;
		for (dupeItor = keyMap_.begin();
			dupeItor != keyMap_.end();
			dupeItor++)
		{
			KeyboardKey *dupe = dupeItor->second;
			if (dupe == key) continue;

			std::vector<KeyboardKey::KeyEntry> &dupeKeys = dupe->getKeys();
			std::vector<KeyboardKey::KeyEntry>::iterator itor;
			for (itor = dupeKeys.begin();
				itor != dupeKeys.end();
				itor++)
			{
				KeyboardKey::KeyEntry &entry = *itor;
				if (key->hasKey(entry.key, entry.state))
				{
					const char *keyName = "", *stateName = "";
					KeyboardKey::translateKeyNameValue(entry.key, keyName);
					KeyboardKey::translateKeyStateValue(entry.state, stateName);

					S3D::dialogMessage("Keyboard", S3D::formatStringBuffer(
								  "Key \"%s\" and state \"%s\" defined for \"%s\" was also defined for \"%s\"",
								  keyName, stateName, key->getName(), dupe->getName()));
					return false;					
				}
			}
		}
	}

	return true;
}

bool Keyboard::loadKeyFile(const std::string &fileName, bool masterFile)
{
	// Load key definition file
	XMLFile file;
    if (!file.readFile(fileName))
	{
		S3D::dialogMessage("Keyboard", S3D::formatStringBuffer(
					  "Failed to parse \"%s\"\n%s", 
					  fileName.c_str(),
					  file.getParserError()));
		return false;
	}

	// Check file exists
	if (!file.getRootNode())
	{
		S3D::dialogMessage("Keyboard", S3D::formatStringBuffer(
					  "Failed to find key definition file \"%s\"",
					  fileName.c_str()));
		return false;		
	}

	// Itterate all of the keys in the file
    std::list<XMLNode *>::iterator childrenItor;
	std::list<XMLNode *> &children = file.getRootNode()->getChildren();
    for (childrenItor = children.begin();
		 childrenItor != children.end();
		 childrenItor++)
    {
		// Parse the tank entry
        XMLNode *currentNode = (*childrenItor);
		if (strcmp(currentNode->getName(), "keyentry"))
		{
			S3D::dialogMessage("Keyboard",
						  "Failed to find keyentry node");
			return false;
		}

		// Get the name of the key
		const char *keyName = 0;
		bool command = false;
		XMLNode *nameNode;
		XMLNode *commandNode;
		if (currentNode->getNamedChild("name", nameNode, false))
		{
			keyName = nameNode->getContent();
			command = false;
		}
		else if (currentNode->getNamedChild("command", commandNode, false))
		{
			keyName = commandNode->getContent();
			command = true;
		}
		else
		{
			S3D::dialogMessage("Keyboard",
						  "Failed to find name node");
			return false;
		}

		if (masterFile)
		{
			// We have already seen this key entry, 
			// and we are loading the master key file.
			std::map<std::string, KeyboardKey *>::iterator findItor =
				keyMap_.find(keyName);
			if (findItor != keyMap_.end())
			{
				// Delete it to make space for the new entry.
				delete findItor->second;
				keyMap_.erase(findItor);
			}
		}
		else
		{
			// We have already seen this key entry, 
			// and we are loading custom keys.
			// Only load custom key entries for keys that we have already
			// seen in the master file
			std::map<std::string, KeyboardKey *>::iterator findItor =
				keyMap_.find(keyName);
			if (findItor != keyMap_.end())
			{
				// Delete it to make space for the new entry.
				delete findItor->second;
				keyMap_.erase(findItor);		
			}
			else
			{
				// This is not in the master file
				// Ignore it
				continue;
			}
		}
		
		// Get the description for the key
		XMLNode *descNode;
		if (!currentNode->getNamedChild("description", descNode)) return false;
		const char *keyDesc = descNode->getContent();

		// Get the group
		int group = 0;
		if (!currentNode->getNamedChild("group", group)) return false;

		// Get the title
		XMLNode *titleNode;
		if (!currentNode->getNamedChild("title", titleNode)) return false;
		const char *keyTitle = titleNode->getContent();

		// Add all the key names
		XMLNode *currentKey = 0;
		std::list<std::string> keyNames, keyStateNames;
		while (currentNode->getNamedChild("key", currentKey, false))
		{
			const char *state = "NONE";
			XMLNode *stateNode;
			if (currentKey->getNamedParameter("state", stateNode, false)) 
			state = stateNode->getContent();

			// Add a key and state
			keyNames.push_back(currentKey->getContent());
			keyStateNames.push_back(state);
		}

		// Create the key
		KeyboardKey *newKey = new KeyboardKey(keyName, keyTitle, keyDesc, group, command);
		if (!newKey->addKeys(keyNames, keyStateNames)) return false;
		if (masterFile) newKey->setChanged(false);

		// Add to the list of pre-defined keys
		keyMap_[keyName] = newKey;
		if (masterFile) keyList_.push_back(keyName);
		if (!currentNode->failChildren()) return false;
	}

	return true;
}

KeyboardKey *Keyboard::getKey(const char *name)
{
	static KeyboardKey defaultKey("None", "None", "None", 0, false);

	std::map<std::string, KeyboardKey *>::iterator itor =
		keyMap_.find(name);
	if (itor != keyMap_.end()) return (*itor).second;

	S3D::dialogMessage("Keyboard", S3D::formatStringBuffer(
				  "Failed to find key for key name \"%s\"",
				  name));	
	return &defaultKey;
}

