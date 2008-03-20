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


// ConsoleRule.h: interface for the ConsoleRule class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ConsoleRULE_H__7A8C972C_069D_44F7_A604_A8A8D1B4773A__INCLUDED_)
#define AFX_ConsoleRULE_H__7A8C972C_069D_44F7_A604_A8A8D1B4773A__INCLUDED_

#include <string>
#include <list>
#include <map>

enum ConsoleRuleType
{
	ConsoleRuleTypeBoolean,
	ConsoleRuleTypeNumber,
	ConsoleRuleTypeString
};

struct ConsoleRuleSplit
{
	std::string rule;
	int position;
	bool valueBool;
	float valueNumber;

	ConsoleRuleType type;
};

class ConsoleRule
{
public:
	ConsoleRule(const char *name);
	virtual ~ConsoleRule();

	virtual void checkRule(const char *line, 
					std::list<ConsoleRuleSplit> split, 
					std::string &result, 
					std::list<std::string> &resultList) = 0;
	virtual void dump(std::list<std::string> &resultList) = 0;
	bool matchRule(const char *line);

	const char *getName() { return name_.c_str(); }
	static void addRuleFail(std::string &failLine, int position, int length);

protected:
	std::string name_;

};

#endif // !defined(AFX_ConsoleRULE_H__7A8C972C_069D_44F7_A604_A8A8D1B4773A__INCLUDED_)
