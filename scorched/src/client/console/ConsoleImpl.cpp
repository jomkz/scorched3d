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
#include <dialogs/GUIConsole.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <limits.h>

GUIConsoleWriteAction::GUIConsoleWriteAction(const CEGUI::String &text, bool command) :
	text_(text), command_(command)
{
}

GUIConsoleWriteAction::~GUIConsoleWriteAction()
{
}

void GUIConsoleWriteAction::performUIAction()
{
	GUIConsole::instance()->outputText(text_, command_);
	delete this;
}

GUIConsoleUpdateTextAction::GUIConsoleUpdateTextAction(const CEGUI::String &text) :
	text_(text)
{
}

GUIConsoleUpdateTextAction::~GUIConsoleUpdateTextAction()
{
}

void GUIConsoleUpdateTextAction::performUIAction()
{
	GUIConsole::instance()->setText(text_);
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
	GUIConsoleWriteAction *writeAction = new GUIConsoleWriteAction(text, parse);
	ScorchedClient::instance()->getClientUISync().addActionFromClient(writeAction);
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

void ConsoleImpl::matchRule(const CEGUI::String &line)
{
	std::vector<ConsoleRule *> matches;
	std::string result = rules_.matchRule(line.c_str(), matches);

	if (result.length() > 0)
	{
		GUIConsoleUpdateTextAction *writeAction = new GUIConsoleUpdateTextAction(result);
		ScorchedClient::instance()->getClientUISync().addActionFromClient(writeAction);
	}

	addLine(false, "-------------------");
	std::vector<ConsoleRule *>::iterator itor;
	for (itor = matches.begin();
		itor != matches.end();
		++itor)
	{
		std::string text = (*itor)->toString();
		addLine(false, text.c_str());
	}
}
