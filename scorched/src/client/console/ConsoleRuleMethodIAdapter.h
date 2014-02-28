////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2013
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
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
////////////////////////////////////////////////////////////////////////////////

#if !defined(__INCLUDE_ConsoleRuleMethodIAdapterh_INCLUDE__)
#define __INCLUDE_ConsoleRuleMethodIAdapterh_INCLUDE__

#include <console/Console.h>

// Q. Hmm what happens when you have a long winded naming scheme?
// A. name -> inf

// Usage pattern :-
// 	new ConsoleRuleMethodIAdapter<InfoMap>(
//		this, showHeightBands, "InfoHeightBands");
// Calls InfoMap::showHeightBands when InfoHeightBands is selected 
// in the console
template<class T>
class ConsoleRuleMethodIAdapter : public ConsoleRule
{
public:
	ConsoleRuleMethodIAdapter(Console &console, T *inst, 
		void (T::*call)(), 
		const char *name) : 
		ConsoleRule(name, std::vector<ConsoleRuleParam>()), 
		console_(console), inst_(inst), call_(call)
	{
		console_.addRule(this);
	};

	ConsoleRuleMethodIAdapter(Console &console, T *inst, 
		void (T::*call)(), 
		const char *name, const std::vector<ConsoleRuleParam> &params) : 
		ConsoleRule(name, params),
		console_(console), inst_(inst), call_(call)
	{
		console_.addRule(this);
	};
	virtual ~ConsoleRuleMethodIAdapter()
	{
		console_.removeRule(this);
	};

	virtual void runRule(
		Console *console,
		const char *wholeLine,
		std::vector<ConsoleRuleValue> &values)
	{
		(inst_->*call_)();
	};

protected:
	Console &console_;
	std::string name_;
	T *inst_;
	void (T::*call_)();
};

// Same as above but passed params/userdata to method
template<class T>
class ConsoleRuleMethodIAdapterEx : public ConsoleRule
{
public:
	ConsoleRuleMethodIAdapterEx(Console &console, T *inst, 
		void (T::*call)(std::vector<ConsoleRuleValue>&, unsigned int), 
		const char *name, const std::vector<ConsoleRuleParam> &params,
		unsigned int userData = 0) :
		ConsoleRule(name, params, userData), 
		console_(console), inst_(inst), call_(call)
	{
		console_.addRule(this);
	};
	virtual ~ConsoleRuleMethodIAdapterEx()
	{
		console_.removeRule(this);
	};

	virtual void runRule(
		Console *console,
		const char *wholeLine,
		std::vector<ConsoleRuleValue> &values)
	{
		(inst_->*call_)(values, userData_);
	};

protected:
	Console &console_;
	std::string name_;
	T *inst_;
	void (T::*call_)(std::vector<ConsoleRuleValue>&, unsigned int);
};

#endif

