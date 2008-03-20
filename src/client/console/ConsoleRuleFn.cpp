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


// ConsoleRuleFn.cpp: implementation of the ConsoleRuleFn class.
//
//////////////////////////////////////////////////////////////////////

#include <console/ConsoleRuleFn.h>
#include <common/Defines.h>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ConsoleRuleFnI::~ConsoleRuleFnI()
{

}

ConsoleRuleFn::ConsoleRuleFn(const char *name, 
								ConsoleRuleFnI *user, 
								ConsoleRuleType type, 
								ConsoleRuleAccessType access) :
	ConsoleRule(name), user_(user), type_(type), access_(access)
{

}

ConsoleRuleFn::~ConsoleRuleFn()
{

}

void ConsoleRuleFn::checkRule(const char *line, 
							  std::list<ConsoleRuleSplit> split, 
							  std::string &result, 
							  std::list<std::string> &resultList)
{
	result = line;
	if (split.size() == 1)
	{
		if (access_ == ConsoleRuleAccessTypeWrite)
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
		std::list<ConsoleRuleSplit>::iterator iter = split.begin();
		iter++;
		ConsoleRuleSplit &split = (*iter);

		if (access_ == ConsoleRuleAccessTypeRead)
		{
			std::string failed;
			ConsoleRule::addRuleFail(failed, split.position, (int) split.rule.length());
			resultList.push_back(failed);
			resultList.push_back("Function is read only");
		}
		else
		{
			if (split.type != type_)
			{
				std::string failed;
				ConsoleRule::addRuleFail(failed, split.position, (int) split.rule.length());
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
		std::list<ConsoleRuleSplit>::iterator iter = split.begin();
		iter++; iter++;
		for (;iter!=split.end();iter++)
		{
			ConsoleRuleSplit &split = (*iter);
			ConsoleRule::addRuleFail(failed, split.position, (int) split.rule.length());
		}
		resultList.push_back(failed);
		resultList.push_back("Too many arguments to fn");
	}
}

void ConsoleRuleFn::setValue(ConsoleRuleSplit &split)
{
	switch (type_)
	{
	case ConsoleRuleTypeBoolean:
		user_->setBoolParam(name_.c_str(), split.valueBool);
		break;
	case ConsoleRuleTypeNumber:
		user_->setNumberParam(name_.c_str(), split.valueNumber);
		break;
	case ConsoleRuleTypeString:
		user_->setStringParam(name_.c_str(), split.rule.c_str());
	}
}

const char *ConsoleRuleFn::getValue()
{
	static std::string value;
	switch (type_)
	{
	case ConsoleRuleTypeBoolean:
		value = (user_->getBoolParam(name_.c_str())?"on":"off");
		break;
	case ConsoleRuleTypeNumber:
		static char buffer[10];
		snprintf(buffer, 10, "%.2f", user_->getNumberParam(name_.c_str()));
		value = buffer;
		break;
	case ConsoleRuleTypeString:
		value = user_->getStringParam(name_.c_str());
		break;
	default:
		value = "None";
	}

	static std::string result;
	result = name_ + "=" + value;

	return result.c_str();
}

void ConsoleRuleFn::dump(std::list<std::string> &resultList)
{
	std::string result = "  " + name_ + "=";
	switch (type_)
	{
	case ConsoleRuleTypeBoolean:
		result += "<on|off>";
		break;
	case ConsoleRuleTypeNumber:
		result += "<number>";
		break;
	case ConsoleRuleTypeString:
		result += "<word>";
		break;
	}

	switch (access_)
	{
	case ConsoleRuleAccessTypeRead:
		result += "  (Read only)";
		break;
	case ConsoleRuleAccessTypeWrite:
		result += "  (Write only)";
		break;
	case ConsoleRuleAccessTypeReadWrite:
		result += "  (Read and Write)";
		break;
	}

	result += " (";
	result += getValue();
	result += ")";

	resultList.push_back(result);
}
