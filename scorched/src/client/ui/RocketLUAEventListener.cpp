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

#include <ui/RocketLUAEventListener.h>
#include <ui/RocketLUALib.h>
#include <client/ScorchedClient.h>
#include <common/Logger.h>
#include <lua/lua.h>
#include <lua/LUAScript.h>
#include <lua/LUAScriptFactory.h>
#include <stdio.h>

RocketLUAEventListener::RocketLUAEventListener() : script_(0)
{
}

RocketLUAEventListener::~RocketLUAEventListener()
{
	delete script_;
}

void RocketLUAEventListener::initialize(const std::string &script)
{
	// Load the script into memory
	script_ = ScorchedClient::instance()->getLUAScriptFactory().createScript();
	luaopen_rocket(script_->getLUAState());
	std::string errorResult;
	if (!script_->initializeFromString(script, errorResult))
	{
		Logger::log(S3D::formatStringBuffer(
			"RocketLUAEventListener::initialize %s", errorResult.c_str()));
		delete script_;
		script_ = 0;
	}
}

// Sends the event value through to Invader's event processing system.
void RocketLUAEventListener::ProcessEvent(Rocket::Core::Event& evt)
{
	if (!script_) return;

	// Turn the variables from the UI event into a LUA global variable called event
	// e.g. evt.mouse_x becomes event.mouse_x in LUA
	int i=0;
	Rocket::Core::String key;
	Rocket::Core::Variant *value;
	lua_newtable(script_->getLUAState());
	while (evt.GetParameters()->Iterate(i, key, value))
	{
		if (value->GetType() == Rocket::Core::Variant::INT)
		{
			lua_pushstring(script_->getLUAState(), key.CString());
			lua_pushnumber(script_->getLUAState(), value->Get<int>());
			lua_settable(script_->getLUAState(), -3);
		}
	}
	lua_setglobal(script_->getLUAState(), "event");

	// Call the actual script
	std::string errorResult;
	if (!script_->execute(errorResult))
	{
		Logger::log(S3D::formatStringBuffer(
			"RocketLUAEventListener::ProcessEvent %s", errorResult.c_str()));
	}
}

// Destroys the event.
void RocketLUAEventListener::OnDetach(Rocket::Core::Element* ROCKET_UNUSED(element))
{
	delete this;
}