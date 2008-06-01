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


// ConsoleRuleFnIAdapter.cpp: implementation of the ConsoleRuleFnIAdapter class.
//
//////////////////////////////////////////////////////////////////////

#include <common/Defines.h>
#include <console/ConsoleRuleFnIAdapter.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ConsoleRuleFnIBooleanAdapter::ConsoleRuleFnIBooleanAdapter(const char *name, bool &param)
	: name_(name), param_(param)
{
	Console::instance()->addFunction(
		name, 
		this,
		ConsoleRuleTypeBoolean, 
		ConsoleRuleAccessTypeReadWrite);
}

ConsoleRuleFnIBooleanAdapter::~ConsoleRuleFnIBooleanAdapter()
{

}

bool ConsoleRuleFnIBooleanAdapter::getBoolParam(const char *name)
{
	DIALOG_ASSERT(name_ == name);
	return param_;
}

void ConsoleRuleFnIBooleanAdapter::setBoolParam(const char *name, bool value)
{
	DIALOG_ASSERT(name_ == name);
	param_ = value;
}

ConsoleRuleFnINumberAdapter::ConsoleRuleFnINumberAdapter(const char *name, float &param)
	: name_(name), param_(param)
{
	Console::instance()->addFunction(
		name, 
		this,
		ConsoleRuleTypeNumber, 
		ConsoleRuleAccessTypeReadWrite);
}

ConsoleRuleFnINumberAdapter::~ConsoleRuleFnINumberAdapter()
{

}

float ConsoleRuleFnINumberAdapter::getNumberParam(const char *name)
{
	DIALOG_ASSERT(name_ == name);
	return param_;
}

void ConsoleRuleFnINumberAdapter::setNumberParam(const char *name, float value)
{
	DIALOG_ASSERT(name_ == name);
	param_ = value;
}

ConsoleRuleFnIOptionsAdapter::ConsoleRuleFnIOptionsAdapter(
	OptionEntry &entry,
	ConsoleRuleAccessType access) :
	entry_(entry)
{
	ConsoleRuleType type = ConsoleRuleTypeBoolean;
	switch (entry.getEntryType())
	{
		case OptionEntry::OptionEntryEnumType:
		case OptionEntry::OptionEntryIntType:
		case OptionEntry::OptionEntryBoundedIntType:
		case OptionEntry::OptionEntryFloatType:
			type = ConsoleRuleTypeNumber;
		break;
		case OptionEntry::OptionEntryVectorType:
		case OptionEntry::OptionEntryStringEnumType:
		case OptionEntry::OptionEntryStringType:
		case OptionEntry::OptionEntryTextType:
			type = ConsoleRuleTypeString;
		break;
		case OptionEntry::OptionEntryBoolType:
			type = ConsoleRuleTypeBoolean;
		break;
		default:
			DIALOG_ASSERT(0);
		break;
	}

	Console::instance()->addFunction(
		entry.getName(), 
		this,
		type, 
		access);
}

ConsoleRuleFnIOptionsAdapter::~ConsoleRuleFnIOptionsAdapter()
{
	Console::instance()->removeFunction(entry_.getName());
}

bool ConsoleRuleFnIOptionsAdapter::getBoolParam(const char *name)
{
	return ((OptionEntryBool &) entry_).getValue();
}

void ConsoleRuleFnIOptionsAdapter::setBoolParam(const char *name, bool value)
{
	((OptionEntryBool &) entry_).setValue(value);
}

float ConsoleRuleFnIOptionsAdapter::getNumberParam(const char *name)
{
	if (entry_.getEntryType() == OptionEntry::OptionEntryIntType)
	{
		return (float) (((OptionEntryInt &) entry_).getValue());
	}
	else if (entry_.getEntryType() == OptionEntry::OptionEntryFloatType)
	{
		return ((OptionEntryFloat &) entry_).getValue();
	}
	return -99.99f;
}

void  ConsoleRuleFnIOptionsAdapter::setNumberParam(const char *name, float value)
{
	if (entry_.getEntryType() == OptionEntry::OptionEntryIntType)
	{
		((OptionEntryInt &) entry_).setValue(int(value));
	}
	else if (entry_.getEntryType() == OptionEntry::OptionEntryFloatType)
	{
		((OptionEntryFloat &) entry_).setValue(value);
	}
}

const char *ConsoleRuleFnIOptionsAdapter::getStringParam(const char *name)
{
	return ((OptionEntryString &) entry_).getValue();
}

void ConsoleRuleFnIOptionsAdapter::setStringParam(const char *name, const char *value)
{
	((OptionEntryString &) entry_).setValue(value);
}
