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

#include <lua/LUAWrapper.h>
#include <lua/LUAFns.h>
#include <common/Logger.h>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

LUAWrapper::LUAWrapper() :
	context_(0)
{
}

LUAWrapper::~LUAWrapper()
{
}

bool LUAWrapper::runScript(const char *filename)
{
	lua_State *L = lua_open();
	
	// Load the available libraries
	luaopen_base(L); 
	luaopen_table(L); 
	//luaopen_io(L); 
	luaopen_string(L); 
	luaopen_s3d(L);
	//luaopen_math(L); 

	// Store the context globaly
	lua_pushlightuserdata(L, (void *) this);
	lua_setglobal(L, "wrapper");

	// Load the script
	bool result = true;
	int temp_int = luaL_loadfile(L, filename);
	if (temp_int == 0)
	{
		// Run the script
		temp_int = lua_pcall(L,0,0,0);
		if (temp_int != 0)
		{
			Logger::log(S3D::formatStringBuffer(
				"ERROR: LUA error : %s", lua_tostring(L, -1)));
			result = false;
		}
	}
	else
	{
		Logger::log(S3D::formatStringBuffer(
			"ERROR: Failed to load file \"%s\"", filename));
		result = false;
	}

	lua_close(L);
	return result;
}
