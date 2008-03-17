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


// GLConsoleRuleFnIAdapter.h: interface for the GLConsoleRuleFnIAdapter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLCONSOLERULEFNIADAPTER_H__A8429FA2_3499_4F4A_95BC_9F94FC58C087__INCLUDED_)
#define AFX_GLCONSOLERULEFNIADAPTER_H__A8429FA2_3499_4F4A_95BC_9F94FC58C087__INCLUDED_

#include <common/OptionEntry.h>
#include <GLEXT/GLConsole.h>
#include <string>

class GLConsoleRuleFnIBooleanAdapter : 
	public GLConsoleRuleFnI
{
public:
	GLConsoleRuleFnIBooleanAdapter(const char *name, bool &param);
	virtual ~GLConsoleRuleFnIBooleanAdapter();

	// Inherited from GLConsoleRuleFnI
	virtual bool getBoolParam(const char *name);
	virtual void setBoolParam(const char *name, bool value);

protected:
	std::string name_;
	bool &param_;

};

class GLConsoleRuleFnINumberAdapter : 
	public GLConsoleRuleFnI
{
public:
	GLConsoleRuleFnINumberAdapter(const char *name, float &param);
	virtual ~GLConsoleRuleFnINumberAdapter();

	// Inherited from GLConsoleRuleFnI
	virtual float getNumberParam(const char *name);
	virtual void  setNumberParam(const char *name, float value);

protected:
	std::string name_;
	float &param_;

};

class GLConsoleRuleFnIOptionsAdapter :
	public GLConsoleRuleFnI
{
public:
	GLConsoleRuleFnIOptionsAdapter(OptionEntry &entry,
		GLConsoleRuleAccessType access);
	virtual ~GLConsoleRuleFnIOptionsAdapter();

	// Inherited from GLConsoleRuleFnI
	virtual bool getBoolParam(const char *name);
	virtual void setBoolParam(const char *name, bool value);
	virtual float getNumberParam(const char *name);
	virtual void  setNumberParam(const char *name, float value);
	virtual const char *getStringParam(const char *name);
	virtual void setStringParam(const char *name, const char *value);

protected:
	OptionEntry &entry_;

};

#endif // !defined(AFX_GLCONSOLERULEFNIADAPTER_H__A8429FA2_3499_4F4A_95BC_9F94FC58C087__INCLUDED_)
