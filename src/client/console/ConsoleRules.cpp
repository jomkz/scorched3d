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


// ConsoleRules.cpp: implementation of the ConsoleRules class.
//
//////////////////////////////////////////////////////////////////////

#include <console/ConsoleRules.h>
#include <common/Defines.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ConsoleRules::ConsoleRules()
{

}

ConsoleRules::~ConsoleRules()
{

}

bool ConsoleRules::addRule(ConsoleRule *rule)
{
	std::map<std::string, ConsoleRule *>::iterator itor =
		rules_.find(rule->getName());
	if (itor != rules_.end())
	{
		return false;
	}

	std::string addName = rule->getName();
	_strlwr((char *) addName.c_str());
	rules_[addName] = rule;

	return true;
}

ConsoleRule *ConsoleRules::removeRule(const char *rulename) 
{
	std::string removeName(rulename);
	_strlwr((char *) removeName.c_str());

	std::map<std::string, ConsoleRule *>::iterator itor =
		rules_.find(removeName);
	if (itor == rules_.end()) return 0;

	ConsoleRule *result = itor->second;
	rules_.erase(itor);
	return result;
}

const char *ConsoleRules::matchRule(const char *line, std::list<ConsoleRule *> &matches)
{
	std::map<std::string, ConsoleRule *>::iterator itor;
	for (itor = rules_.begin();
		itor != rules_.end();
		itor++)
	{
		if ((*itor).second->matchRule(line))
		{
			matches.push_back((*itor).second);
		}		
	}

	if (matches.empty()) return 0;
	if (matches.size() == 1) return matches.front()->getName();

	static char buffer[1024];
	snprintf(buffer, sizeof(buffer), "%s", line);

	int pos = (int) strlen(line);
	while (true)
	{
		std::list<ConsoleRule *>::iterator itor2 = matches.begin();
		char letter = (*itor2)->getName()[pos];
		
		for (itor2++;
			itor2 != matches.end();
			itor2++)
		{
			if ((*itor2)->getName()[pos] != letter) return buffer;
		}

		buffer[pos] = letter;
		buffer[pos+1] = '\0';
		pos++;
	}

	return buffer;
}

void ConsoleRules::addLine(const char *line, std::string &result, std::list<std::string> &resultList)
{
	std::list<ConsoleRuleSplit> split;
	if (!parseLine(line, split))
	{
		result = line;
		std::string failed;
		ConsoleRule::addRuleFail(failed, (int) strlen(line), 1);
		resultList.push_back(failed);
		resultList.push_back("Non terminated quote");
	}
	else
	{
		if (split.empty())
		{
			result = line;
			resultList.push_back("Input line is empty.");
		}
		else
		{
			ConsoleRuleSplit firstSplit = split.front();
			_strlwr((char *)firstSplit.rule.c_str());
			std::map<std::string, ConsoleRule *>::iterator itor =
				rules_.find(firstSplit.rule);
			if (itor == rules_.end())
			{
				result = line;
				std::string failed;
				ConsoleRule::addRuleFail(failed, firstSplit.position, (int) firstSplit.rule.length());
				resultList.push_back(failed);
				resultList.push_back(std::string("Unrecognised function ") + "\"" + firstSplit.rule + "\"");
			}
			else
			{
				(*itor).second->checkRule(line, split, result, resultList);
			}
		}
	}
}

bool ConsoleRules::parseLine(const char *line, std::list<ConsoleRuleSplit> &split)
{
	int pos = -1;
	bool inQuote = false;
	std::string currentEntry;
	for (int i=0; i<(int) strlen(line)+1; i++)
	{
		const char c = line[i];

		if (c == '\0')
		{
			if (inQuote) return false;
			parseAddLine(pos, currentEntry.c_str(), split);
			currentEntry = ""; pos = -1;

			return true;
		}
		else if (c == '=' && 
			((split.size() == 1 &&  currentEntry.empty()) || 
			 (split.size() == 0 && !currentEntry.empty())))
		{
			parseAddLine(pos, currentEntry.c_str(), split);
			currentEntry = ""; pos = -1;
		}
		else if ((c == ' ') && !inQuote)
		{
			parseAddLine(pos, currentEntry.c_str(), split);
			currentEntry = ""; pos = -1;
		}
		else if (c == '\"')
		{
			inQuote = !inQuote;
		}
		else
		{
			if (pos == -1) pos = i + 1;
			currentEntry += c;
		}
	}

	return !inQuote;
}

void ConsoleRules::parseAddLine(int position, const char *line, std::list<ConsoleRuleSplit> &split)
{
	int n = (int) strlen(line);
	if (n == 0) return;

	ConsoleRuleSplit newSplit;
	newSplit.rule = line;
	newSplit.position = position;

	if (strcmp(line, "on") == 0)
	{
		newSplit.type = ConsoleRuleTypeBoolean;
		newSplit.valueBool = true;
	}
	else if (strcmp(line, "off") == 0)
	{
		newSplit.type = ConsoleRuleTypeBoolean;
		newSplit.valueBool = false;
	}
	else
	{
		bool numbersOnly = true;
		for (int i=0; i<n; i++)
		{
			if ((line[i] < '0') || (line[i] > '9'))
			{
				if (line[i]!='.')
				{
					numbersOnly = false;
					break;
				}
			}
		}

		if (numbersOnly)
		{
			newSplit.type = ConsoleRuleTypeNumber;
			newSplit.valueNumber = (float) atof(line);
		}
		else
		{
			newSplit.type = ConsoleRuleTypeString;
		}
	}

	split.push_back(newSplit);
}

void ConsoleRules::dump(std::list<std::string> &resultList)
{
	std::list<std::string> sortList;

	std::map<std::string, ConsoleRule *>::iterator itor;
	for (itor = rules_.begin();
		itor != rules_.end();
		itor++)
	{
		sortList.push_back((*itor).first);
	}

	sortList.sort();
	std::list<std::string>::iterator itor2;
	for (itor2 = sortList.begin();
		itor2 != sortList.end();
		itor2++)
	{
		rules_[itor2->c_str()]->dump(resultList);
	}
}
