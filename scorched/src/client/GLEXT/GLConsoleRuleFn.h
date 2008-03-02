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

#if !defined(AFX_GLCONSOLERULEFN_H__B0DEC68E_5D12_410F_AEA2_3BF1AF6B3319__INCLUDED_)
#define AFX_GLCONSOLERULEFN_H__B0DEC68E_5D12_410F_AEA2_3BF1AF6B3319__INCLUDED_

#include <GLEXT/GLConsoleRule.h>
#include <common/DefinesAssert.h>

enum GLConsoleRuleAccessType
{
	GLConsoleRuleAccessTypeRead = 1,
	GLConsoleRuleAccessTypeWrite = 2,
	GLConsoleRuleAccessTypeReadWrite = 3
};

class GLConsoleRuleFnI
{
public:
	virtual ~GLConsoleRuleFnI();

	virtual bool getBoolParam(const char *name) { DIALOG_ASSERT(0); return true; }
	virtual void setBoolParam(const char *name, bool value) { DIALOG_ASSERT(0); }

	virtual float getNumberParam(const char *name) { DIALOG_ASSERT(0); return 0.0f; }
	virtual void  setNumberParam(const char *name, float value) { DIALOG_ASSERT(0); }

	virtual const char *getStringParam(const char *name) { DIALOG_ASSERT(0); return 0; }
	virtual void setStringParam(const char *name, const char *value) { DIALOG_ASSERT(0); }
};

class GLConsoleRuleFn : public GLConsoleRule
{
public:
	GLConsoleRuleFn(const char *name, 
		GLConsoleRuleFnI *user, 
		GLConsoleRuleType type, 
		GLConsoleRuleAccessType access = GLConsoleRuleAccessTypeReadWrite);
	virtual ~GLConsoleRuleFn();

	void checkRule(const char *line, 
					std::list<GLConsoleRuleSplit> split, 
					std::string &result, 
					std::list<std::string> &resultList);
	void dump(std::list<std::string> &resultList);

protected:
	GLConsoleRuleFnI *user_;
	GLConsoleRuleType type_;
	GLConsoleRuleAccessType access_;

	void setValue(GLConsoleRuleSplit &split);
	const char *getValue();
};

#endif // !defined(AFX_GLCONSOLERULEFN_H__B0DEC68E_5D12_410F_AEA2_3BF1AF6B3319__INCLUDED_)
