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

#if !defined(AFX_ConsoleRULEFNIADAPTER_H__A8429FA2_3499_4F4A_95BC_9F94FC58C087__INCLUDED_)
#define AFX_ConsoleRULEFNIADAPTER_H__A8429FA2_3499_4F4A_95BC_9F94FC58C087__INCLUDED_

#include <common/OptionEntry.h>
#include <console/ConsoleRuleMethodIAdapter.h>
#include <string>

class ConsoleRuleOptionsAdapter
{
public:
	ConsoleRuleOptionsAdapter(Console &console, OptionEntry &entry);
	virtual ~ConsoleRuleOptionsAdapter();
protected:
	Console &console_;
	OptionEntry &entry_;
	ConsoleRuleMethodIAdapterEx<ConsoleRuleOptionsAdapter> *readRule_, *writeRule_;

	void readValue(std::vector<ConsoleRuleValue> &values, unsigned int userData);
	void writeValue(std::vector<ConsoleRuleValue> &values, unsigned int userData);
};

class ConsoleRuleOptionsAdapterHolder
{
public:
	ConsoleRuleOptionsAdapterHolder();
	virtual ~ConsoleRuleOptionsAdapterHolder();

	void addToConsole(Console &console, std::list<OptionEntry *> &options);

protected:
	std::list<ConsoleRuleOptionsAdapter *> adapters_;
};

#endif // !defined(AFX_ConsoleRULEFNIADAPTER_H__A8429FA2_3499_4F4A_95BC_9F94FC58C087__INCLUDED_)
