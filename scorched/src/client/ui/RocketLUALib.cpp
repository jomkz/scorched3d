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

#include <ui/RocketLUALib.h>
#include <lua/LUAScript.h>
#include <lua/LUAUtil.h>
#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <graph/MainCamera.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <coms/ComsTankChangeMessage.h>
#include <coms/ComsMessageSender.h>
#include <map>

#define LUA_LIB

#include <lua/lauxlib.h>
#include <lua/lualib.h>

static LUAScript *getScript(lua_State *L)
{
	lua_getglobal(L, "s3d_script");
	LUAScript *script = (LUAScript *) lua_touserdata(L, -1);
	lua_pop(L, 1);
	return script;
}

static int s3dui_joinGame(lua_State *L) 
{
	unsigned int current = 0, currentPlayerId_ = 0;
	std::map<unsigned int, Tank *> &tanks = 
		ScorchedClient::instance()->getTargetContainer().getTanks();
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		++itor)
	{
		Tank *tank = (*itor).second;
		if ((tank->getDestinationId() == 
			ScorchedClient::instance()->getTargetContainer().getCurrentDestinationId()) &&
			(tank->getPlayerId() != TargetID::SPEC_TANK_ID))
		{
			if (current == 0)
			{
				currentPlayerId_ = tank->getPlayerId();
				break;
			}
			else if (tank->getPlayerId() == current) 
			{
				current = 0;
			}
		}
	}

	// Add this player
	ComsTankChangeMessage message(currentPlayerId_,
		LANG_STRING("TestName"),
		Vector(),
		"none",
		"none",
		ScorchedClient::instance()->getTargetContainer().getCurrentDestinationId(),
		0,
		"Human",
		false);
	// Add avatar (if not one)
	// TODO
	/*
	Tank *tank = ScorchedClient::instance()->getTargetContainer().
		getTankById(currentPlayerId_);
	if (tank && 
		strcmp(tank->getAvatar().getName(), imageList_->getCurrentShortPath()) != 0)
	{
		if (tank->getAvatar().loadFromFile(imageList_->getCurrentLongPath()))
		{
			if (tank->getAvatar().getFile().getBufferUsed() <=
				(unsigned) ScorchedClient::instance()->getOptionsGame().getMaxAvatarSize())
			{
				message.setPlayerIconName(imageList_->getCurrentShortPath());
				message.getPlayerIcon().addDataToBuffer(
					tank->getAvatar().getFile().getBuffer(),
					tank->getAvatar().getFile().getBufferUsed());
			}
			else
			{
				Logger::log( "Warning: Avatar too large to send to server");
			}
		}
	}
	*/
	ComsMessageSender::sendToServer(message);

	return 0;
}

static int s3dui_stimulus(lua_State *L) 
{
	LUAScript *wrapper = getScript(L);
	const char *stimulusName = luaL_checkstring(L, 1);
	
	((ScorchedClient *) wrapper->getContext())->getClientState().setStateString(stimulusName);

	return 0;
}

static const luaL_Reg s3drocket[] = {
	{"stimulus", s3dui_stimulus},
	{"joinGame", s3dui_joinGame},
	{NULL, NULL}
};

LUALIB_API int luaopen_rocket (lua_State *L) {
	luaL_register(L, LUA_ROCKETLIBNAME, s3drocket);
	return 1;
}
