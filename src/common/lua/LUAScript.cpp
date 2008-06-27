////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
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

#include <lua/LUAScript.h>
#include <common/Logger.h>

#include "lauxlib.h"
#include "lualib.h"
#include "LUAFns.h"

LUAScript::LUAScript(LUAWrapper *wrapper) :
	L_(0)
{
	L_ = lua_open();
	
	// Load the available libraries
	luaopen_base(L_); 
	luaopen_table(L_); 
	//luaopen_io(L_); 
	luaopen_string(L_); 
	luaopen_s3d(L_);
	//luaopen_math(L_); 

	// Store the context globaly
	lua_pushlightuserdata(L_, (void *) wrapper);
	lua_setglobal(L_, "wrapper");
}

LUAScript::~LUAScript()
{
	if (L_) 
	{
		lua_close(L_);
		L_ = 0;
	}
}

bool LUAScript::loadFromFile(const std::string &filename)
{
	// Load the script
	bool result = true;
	int temp_int = luaL_dofile(L_, filename.c_str());
	if (temp_int != 0)
	{
		Logger::log(S3D::formatStringBuffer(
			"ERROR: LUA error : %s", lua_tostring(L_, -1)));
		result = false;
	}
	
	return result;
}

bool LUAScript::setGlobal(const std::string &name, fixed value)
{
	lua_pushnumber(L_, value.getInternal());
	lua_setglobal(L_, name.c_str());
	return true;
}

bool LUAScript::startFunction(const std::string &functionName)
{
	lua_getglobal(L_, functionName.c_str());
	return true;
}

bool LUAScript::endFunction(int argCount)
{
	int temp_int = lua_pcall(L_, argCount, 0, 0);
	if (temp_int != 0)
	{
		Logger::log(S3D::formatStringBuffer(
			"ERROR: LUA error : %s", lua_tostring(L_, -1)));
		return false;
	}
	return true;
}

bool LUAScript::addNumberParameter(fixed number)
{
	lua_pushnumber(L_, number.getInternal());
	return true;
}

bool LUAScript::addVectorParameter(const FixedVector &v)
{
	FixedVector &vec = (FixedVector &) v;

	lua_newtable(L_);
	lua_pushstring(L_, "x");
	lua_pushnumber(L_, vec[0].getInternal());
	lua_settable(L_, -3);

	lua_pushstring(L_, "y");
	lua_pushnumber(L_, vec[1].getInternal());
	lua_settable(L_, -3);

	lua_pushstring(L_, "z");
	lua_pushnumber(L_, vec[2].getInternal());
	lua_settable(L_, -3);

	return true;
}
