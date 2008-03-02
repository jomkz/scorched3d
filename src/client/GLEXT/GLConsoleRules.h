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


// GLConsoleRules.h: interface for the GLConsoleRules class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLCONSOLERULES_H__F75EC197_F0BD_459C_BC62_929D3A97FB5A__INCLUDED_)
#define AFX_GLCONSOLERULES_H__F75EC197_F0BD_459C_BC62_929D3A97FB5A__INCLUDED_

#include <GLEXT/GLConsoleRule.h>

class GLConsoleRules  
{
public:
	GLConsoleRules();
	virtual ~GLConsoleRules();

	void addLine(const char *line, std::string &result, std::list<std::string> &resultList);
	bool addRule(GLConsoleRule *rule);
	GLConsoleRule *removeRule(const char *rule);

	const char *matchRule(const char *line,
						std::list<GLConsoleRule *> &matches);
	void dump(std::list<std::string> &resultList);

protected:
	std::map<std::string, GLConsoleRule *> rules_;

	bool parseLine(const char *line, std::list<GLConsoleRuleSplit> &split);
	void parseAddLine(int position, const char *line, std::list<GLConsoleRuleSplit> &split);

};

#endif // !defined(AFX_GLCONSOLERULES_H__F75EC197_F0BD_459C_BC62_929D3A97FB5A__INCLUDED_)
