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

#if !defined(__INCLUDE_GLConsoleRuleMethodIAdapterh_INCLUDE__)
#define __INCLUDE_GLConsoleRuleMethodIAdapterh_INCLUDE__

#include <GLEXT/GLConsole.h>

// Q. Hmm what happens when you have a long winded naming scheme?
// A. name -> inf

// Usage pattern :-
// 	new GLConsoleRuleMethodIAdapter<InfoMap>(
//		this, showHeightBands, "InfoHeightBands");
// Calls InfoMap::showHeightBands when InfoHeightBands is selected 
// in the console
template<class T>
class GLConsoleRuleMethodIAdapter : public GLConsoleRuleMethodI
{
public:
	GLConsoleRuleMethodIAdapter(T *inst, void (T::*call)(), const char *name) 
		: inst_(inst), call_(call), name_(name)
	{
		GLConsole::instance()->addMethod(name_.c_str(), this);
	};
	virtual ~GLConsoleRuleMethodIAdapter()
	{
		GLConsole::instance()->removeMethod(name_.c_str());
	};

	virtual void runMethod(const char *name, 
						   std::list<GLConsoleRuleSplit> split,
						   std::string &result,
						   std::list<std::string> &resultList)
	{
		(inst_->*call_)();
	};

protected:
	std::string name_;
	T *inst_;
	void (T::*call_)();
};

// Same as above but passed params to method
template<class T>
class GLConsoleRuleMethodIAdapterEx : public GLConsoleRuleMethodI
{
public:
	GLConsoleRuleMethodIAdapterEx(T *inst, void (T::*call)(std::list<GLConsoleRuleSplit>), const char *name) 
		: inst_(inst), call_(call), name_(name)
	{
		GLConsole::instance()->addMethod(name_.c_str(), this);
	};
	virtual ~GLConsoleRuleMethodIAdapterEx()
	{
		GLConsole::instance()->removeMethod(name_.c_str());
	};

	virtual void runMethod(const char *name, 
						   std::list<GLConsoleRuleSplit> split,
						   std::string &result,
						   std::list<std::string> &resultList)
	{
		(inst_->*call_)(split);
	};

protected:
	std::string name_;
	T *inst_;
	void (T::*call_)(std::list<GLConsoleRuleSplit>);
};

// Same as above but passed params and result to method
template<class T>
class GLConsoleRuleMethodIAdapterEx2 : public GLConsoleRuleMethodI
{
public:
	GLConsoleRuleMethodIAdapterEx2(T *inst, void (T::*call)(std::list<GLConsoleRuleSplit>, 
		std::list<std::string>&), const char *name)
		: inst_(inst), call_(call), name_(name)
	{
		GLConsole::instance()->addMethod(name_.c_str(), this);
	};
	virtual ~GLConsoleRuleMethodIAdapterEx2()
	{
		GLConsole::instance()->removeMethod(name_.c_str());
	};
	virtual void runMethod(const char *name,  
		std::list<GLConsoleRuleSplit> split,
		std::string &result,
		std::list<std::string> &resultList)
	{
		(inst_->*call_)(split, resultList);
	};
	
protected:
	std::string name_;
	T *inst_;
	void (T::*call_)(std::list<GLConsoleRuleSplit>, std::list<std::string>&);
};                         

#endif

