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


// GLConsoleRuleMethod.cpp: implementation of the GLConsoleRuleMethod class.
//
//////////////////////////////////////////////////////////////////////

#include <GLEXT/GLConsoleRuleMethod.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GLConsoleRuleMethodI::~GLConsoleRuleMethodI()
{

}

GLConsoleRuleMethod::GLConsoleRuleMethod(const char *name,
										 GLConsoleRuleMethodI *user) : 
	GLConsoleRule(name),
	user_(user)
{

}

GLConsoleRuleMethod::~GLConsoleRuleMethod()
{

}

void GLConsoleRuleMethod::checkRule(const char *line, 
				std::list<GLConsoleRuleSplit> split, 
				std::string &result, 
				std::list<std::string> &resultList)
{
	std::list<GLConsoleRuleSplit>::iterator iter = split.begin();
	result = line;

	user_->runMethod(name_.c_str(), split, result, resultList);
}

void GLConsoleRuleMethod::dump(std::list<std::string> &resultList)
{
	std::string result = "  " + name_;
	resultList.push_back(result);
}
