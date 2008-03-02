////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
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

#include <GLEXT/GLConsoleMethods.h>
#include <GLEXT/GLConsoleFileReader.h>
#include <client/ScorchedClient.h>
#include <engine/MainLoop.h>

GLConsoleMethods::GLConsoleMethods(GLConsoleRules &rules,
								   GLConsoleLines &lines) :
	rules_(rules), lines_(lines)
{
	rules_.addRule(new GLConsoleRuleMethod("consoleload", this));
	rules_.addRule(new GLConsoleRuleMethod("consolesave", this));
	rules_.addRule(new GLConsoleRuleMethod("exit", this));
	rules_.addRule(new GLConsoleRuleMethod("quit", this));
	rules_.addRule(new GLConsoleRuleMethod("clear", this));
	rules_.addRule(new GLConsoleRuleMethod("help", this));
}

GLConsoleMethods::~GLConsoleMethods()
{

}

void GLConsoleMethods::runMethod(const char *name,
								 std::list<GLConsoleRuleSplit> split,
								 std::string &result,
								 std::list<std::string> &resultList)
{
	if (!strcmp(name, "clear"))
	{
		lines_.clear();
	}
	else if (!strcmp(name, "help"))
	{
		rules_.dump(resultList);
	}
	else if (!strcmp(name, "exit") || !strcmp(name, "quit"))
	{
		ScorchedClient::instance()->getMainLoop().exitLoop();
	}
	else if (!strcmp(name, "consoleload"))
	{
		split.pop_front();
		if (split.empty())
		{
			resultList.push_back("consoleload <filename>");
			resultList.push_back("  Loads <filename> into the console.");
		}
		else
		{
			std::string errorString;
			GLConsoleRuleSplit option = split.front();
			if (!GLConsoleFileReader::loadFileIntoConsole(option.rule.c_str(), errorString))
			{
				resultList.push_back("  Load failed.");
				resultList.push_back(errorString);
			}
			result += " ";
			result += option.rule.c_str();
		}
	}
	else if (!strcmp(name, "consolesave"))
	{
		split.pop_front();
		if (split.empty())
		{
			resultList.push_back("consolesave <filename>");
			resultList.push_back("  Saves the console into <filename>.");
		}
		else
		{
			GLConsoleRuleSplit option = split.front();
			GLConsoleFileReader::saveConsoleIntoFile(option.rule.c_str());
			result += " ";
			result += option.rule.c_str();
		}
	}
}
