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

#include <lua/LUAFns.h>
#include <lua/LUAWrapper.h>
#include <common/OptionEntry.h>
#include <common/OptionsScorched.h>

#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

static LUAWrapper *getWrapper(lua_State *L)
{
	lua_getglobal(L, "wrapper");
	LUAWrapper *wrapper = (LUAWrapper *) lua_touserdata(L, -1);
	lua_pop(L, 1);
	return wrapper;
}

static int s3d_options_game (lua_State *L) 
{
	LUAWrapper *wrapper = getWrapper(L);

	OptionEntry *entry = OptionEntryHelper::getEntry(
		wrapper->getContext()->optionsGame->getMainOptions().getOptions(),
		"mod");
	if (entry) lua_pushstring(L, entry->getValueAsString());
	else lua_pushstring(L, "");

	return 1;
}

static const luaL_Reg s3dlib[] = {
	{"options_game",  s3d_options_game},
	{NULL, NULL}
};

LUALIB_API int luaopen_s3d (lua_State *L) {
	luaL_register(L, LUA_S3DLIBNAME, s3dlib);
	return 1;
}
