////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011
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

#if !defined(__INCLUDE_LUAScript_INCLUDE__)
#define __INCLUDE_LUAScript_INCLUDE__

#include <string>
#include <common/FixedVector.h>

#include "lua.h"

class Weapon;
class ScorchedContext;
class LUAScript
{
public:
	LUAScript(ScorchedContext *context);
	~LUAScript();

	bool executeFile(const std::string &filename, std::string &error);
	bool initializeFromString(const std::string &script, std::string &error);
	bool execute(std::string &error);

	bool functionExists(const std::string &functionName);
	bool startFunction(const std::string &functionName);
	bool endFunction(int argCount);

	bool addStringParameter(const std::string &str);
	bool addNumberParameter(fixed number);
	bool addBoolParameter(bool boolean);
	bool addVectorParameter(const FixedVector &vector);

	bool setGlobal(const std::string &name, int value);
	bool setGlobal(const std::string &name, fixed value);

	ScorchedContext *getContext() { return context_; }

	void setWeapon(Weapon *weapon) { weapon_ = weapon; }
	Weapon *getWeapon() { return weapon_; }

	lua_State *getLUAState() { return L_; }

protected:
	Weapon *weapon_;
	ScorchedContext *context_;
	int savedChunkReference_;
	lua_State *L_;
};

#endif // __INCLUDE_LUAScript_INCLUDE__
