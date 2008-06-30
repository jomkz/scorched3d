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

#include <lua/LUAUtil.h>
#include <common/Logger.h>

void LUAUtil::addVectorToStack(lua_State *L, const FixedVector &vector)
{
	FixedVector &vec = (FixedVector &) vector;

	lua_newtable(L);
	lua_pushstring(L, "x");
	lua_pushnumber(L, vec[0].getInternal());
	lua_settable(L, -3);

	lua_pushstring(L, "y");
	lua_pushnumber(L, vec[1].getInternal());
	lua_settable(L, -3);

	lua_pushstring(L, "z");
	lua_pushnumber(L, vec[2].getInternal());
	lua_settable(L, -3);
}

FixedVector LUAUtil::getVectorFromStack(lua_State *L, int position)
{
	FixedVector result;

	luaL_checktype(L, position, LUA_TTABLE);

	lua_pushstring(L, "x");
	lua_gettable(L, position);
	result[0] = fixed(true, luaL_checknumber(L, -1));
	lua_pop(L, 1);

	lua_pushstring(L, "y");
	lua_gettable(L, position);
	result[1] = fixed(true, luaL_checknumber(L, -1));
	lua_pop(L, 1);

	lua_pushstring(L, "z");
	lua_gettable(L, position);
	result[2] = fixed(true, luaL_checknumber(L, -1));
	lua_pop(L, 1);

	return result;
}

fixed LUAUtil::getNumberFromTable(lua_State *L, int tablePosition, const char *name)
{
	lua_pushstring(L, name);
	lua_gettable(L, tablePosition);
	fixed result = fixed(true, luaL_checknumber(L, -1));
	lua_pop(L, 1);

	return result;
}
