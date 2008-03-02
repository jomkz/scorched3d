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


// GLConsoleRuleFn.cpp: implementation of the GLConsoleRuleFn class.
//
//////////////////////////////////////////////////////////////////////

#include <GLEXT/GLConsoleRuleFn.h>
#include <common/Defines.h>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GLConsoleRuleFnI::~GLConsoleRuleFnI()
{

}

GLConsoleRuleFn::GLConsoleRuleFn(const char *name, 
								GLConsoleRuleFnI *user, 
								GLConsoleRuleType type, 
								GLConsoleRuleAccessType access) :
	GLConsoleRule(name), user_(user), type_(type), access_(access)
{

}

GLConsoleRuleFn::~GLConsoleRuleFn()
{

}

void GLConsoleRuleFn::checkRule(const char *line, 
							  std::list<GLConsoleRuleSplit> split, 
							  std::string &result, 
							  std::list<std::string> &resultList)
{
	result = line;
	if (split.size() == 1)
	{
		if (access_ == GLConsoleRuleAccessTypeWrite)
		{
			resultList.push_back("Function is write only");
		}
		else
		{
			resultList.push_back(getValue());
		}
	}
	else if (split.size() == 2)
	{
		std::list<GLConsoleRuleSplit>::iterator iter = split.begin();
		iter++;
		GLConsoleRuleSplit &split = (*iter);

		if (access_ == GLConsoleRuleAccessTypeRead)
		{
			std::string failed;
			GLConsoleRule::addRuleFail(failed, split.position, (int) split.rule.length());
			resultList.push_back(failed);
			resultList.push_back("Function is read only");
		}
		else
		{
			if (split.type != type_)
			{
				std::string failed;
				GLConsoleRule::addRuleFail(failed, split.position, (int) split.rule.length());
				resultList.push_back(failed);
				resultList.push_back("Wrong argument type");
			}
			else
			{
				setValue(split);
				resultList.push_back(getValue());
			}
		}
	}
	else
	{
		std::string failed;
		std::list<GLConsoleRuleSplit>::iterator iter = split.begin();
		iter++; iter++;
		for (;iter!=split.end();iter++)
		{
			GLConsoleRuleSplit &split = (*iter);
			GLConsoleRule::addRuleFail(failed, split.position, (int) split.rule.length());
		}
		resultList.push_back(failed);
		resultList.push_back("Too many arguments to fn");
	}
}

void GLConsoleRuleFn::setValue(GLConsoleRuleSplit &split)
{
	switch (type_)
	{
	case GLConsoleRuleTypeBoolean:
		user_->setBoolParam(name_.c_str(), split.valueBool);
		break;
	case GLConsoleRuleTypeNumber:
		user_->setNumberParam(name_.c_str(), split.valueNumber);
		break;
	case GLConsoleRuleTypeString:
		user_->setStringParam(name_.c_str(), split.rule.c_str());
	}
}

const char *GLConsoleRuleFn::getValue()
{
	static std::string value;
	switch (type_)
	{
	case GLConsoleRuleTypeBoolean:
		value = (user_->getBoolParam(name_.c_str())?"on":"off");
		break;
	case GLConsoleRuleTypeNumber:
		static char buffer[10];
		snprintf(buffer, 10, "%.2f", user_->getNumberParam(name_.c_str()));
		value = buffer;
		break;
	case GLConsoleRuleTypeString:
		value = user_->getStringParam(name_.c_str());
		break;
	default:
		value = "None";
	}

	static std::string result;
	result = name_ + "=" + value;

	return result.c_str();
}

void GLConsoleRuleFn::dump(std::list<std::string> &resultList)
{
	std::string result = "  " + name_ + "=";
	switch (type_)
	{
	case GLConsoleRuleTypeBoolean:
		result += "<on|off>";
		break;
	case GLConsoleRuleTypeNumber:
		result += "<number>";
		break;
	case GLConsoleRuleTypeString:
		result += "<word>";
		break;
	}

	switch (access_)
	{
	case GLConsoleRuleAccessTypeRead:
		result += "  (Read only)";
		break;
	case GLConsoleRuleAccessTypeWrite:
		result += "  (Write only)";
		break;
	case GLConsoleRuleAccessTypeReadWrite:
		result += "  (Read and Write)";
		break;
	}

	result += " (";
	result += getValue();
	result += ")";

	resultList.push_back(result);
}
