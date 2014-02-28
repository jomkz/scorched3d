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

#include <console/ConsoleRuleMethodIAdapter.h>
#include <console/ConsoleMethods.h>
#include <common/DefinesString.h>
#include <client/ScorchedClient.h>
#include <client/ClientState.h>

ConsoleMethods::ConsoleMethods()
{

}

ConsoleMethods::~ConsoleMethods()
{

}

void ConsoleMethods::init(Console &console)
{
	deleter_.addRule(
		new ConsoleRuleMethodIAdapter<ConsoleMethods>(
			console,
			this, &ConsoleMethods::exit, 
			"exit", "Exit the game"));
	deleter_.addRule(
		new ConsoleRuleMethodIAdapter<ConsoleMethods>(
			console,
			this, &ConsoleMethods::exit, 
			"quit", "Quit the game"));
	deleter_.addRule(
		new ConsoleRuleMethodIAdapter<ConsoleMethods>(
			console,
			this, &ConsoleMethods::help, 
			"help", "Show all of the available console commands"));
}

void ConsoleMethods::help()
{
	ScorchedClient::instance()->getConsole().help();
}

void ConsoleMethods::exit()
{
	::exit(0);
}
