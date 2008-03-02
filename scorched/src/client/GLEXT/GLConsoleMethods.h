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


// GLConsoleMethods.h: interface for the GLConsoleMethods class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLCONSOLEMETHODS_H__D5E893D4_E73A_49CB_B0E8_8C945C692CC7__INCLUDED_)
#define AFX_GLCONSOLEMETHODS_H__D5E893D4_E73A_49CB_B0E8_8C945C692CC7__INCLUDED_


#include <GLEXT/GLConsoleRuleMethod.h>
#include <GLEXT/GLConsoleRules.h>
#include <GLEXT/GLConsoleLines.h>

class GLConsoleMethods : public GLConsoleRuleMethodI
{
public:
	GLConsoleMethods(GLConsoleRules &rules,
					GLConsoleLines &lines);
	virtual ~GLConsoleMethods();

	// Inherited from GLConsoleMethodI
	virtual void runMethod(const char *name, 
		std::list<GLConsoleRuleSplit> split,
		std::string &result,
		std::list<std::string> &resultList);

protected:
	GLConsoleRules &rules_;
	GLConsoleLines &lines_;

};

#endif // !defined(AFX_GLCONSOLEMETHODS_H__D5E893D4_E73A_49CB_B0E8_8C945C692CC7__INCLUDED_)
