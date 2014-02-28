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

#include <console/ConsoleImpl.h>
#include <client/ScorchedClient.h>
#include <scorched3dc/GUIConsole.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <limits.h>

GUIConsoleWriteAction::GUIConsoleWriteAction(const CEGUI::String &text) :
	text_(text)
{
}

GUIConsoleWriteAction::~GUIConsoleWriteAction()
{
}

void GUIConsoleWriteAction::performUIAction()
{
	GUIConsole::instance()->outputText(text_);
	delete this;
}

ConsoleImpl::ConsoleImpl()
{
	methods_.init(*this);
}

ConsoleImpl::~ConsoleImpl()
{

}

void ConsoleImpl::addLine(bool parse, const CEGUI::String &text)
{
	GUIConsoleWriteAction *writeAction = new GUIConsoleWriteAction(text);
	ScorchedClient::instance()->getClientUISync().addClientUISyncAction(writeAction);
	if (parse)
	{
		rules_.addLine(this, text.c_str());
	}
}

void ConsoleImpl::help()
{
	std::vector<std::string>::iterator itor;
	std::vector<std::string> result;
	rules_.dump(result);

	for (itor = result.begin();
		itor != result.end();
		++itor)
	{
		addLine(false, *itor);
	}
}
