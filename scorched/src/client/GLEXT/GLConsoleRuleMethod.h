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


// GLConsoleRuleMethod.h: interface for the GLConsoleRuleMethod class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLCONSOLERULEMETHOD_H__3B593346_8294_4DBC_8338_D2EA270FFDC8__INCLUDED_)
#define AFX_GLCONSOLERULEMETHOD_H__3B593346_8294_4DBC_8338_D2EA270FFDC8__INCLUDED_


#include <GLEXT/GLConsoleRule.h>

class GLConsoleRuleMethodI
{
public:
	virtual ~GLConsoleRuleMethodI();

	virtual void runMethod(const char *name, 
						   std::list<GLConsoleRuleSplit> split,
						   std::string &result,
						   std::list<std::string> &resultList) = 0;
};

class GLConsoleRuleMethod : public GLConsoleRule
{
public:
	GLConsoleRuleMethod(const char *name,
			GLConsoleRuleMethodI *user);
	virtual ~GLConsoleRuleMethod();

	void checkRule(const char *line, 
				std::list<GLConsoleRuleSplit> split, 
				std::string &result, 
				std::list<std::string> &resultList);
	void dump(std::list<std::string> &resultList);

protected:
	GLConsoleRuleMethodI *user_;
};

#endif // !defined(AFX_GLCONSOLERULEMETHOD_H__3B593346_8294_4DBC_8338_D2EA270FFDC8__INCLUDED_)
