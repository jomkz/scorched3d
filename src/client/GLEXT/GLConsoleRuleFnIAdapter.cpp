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


// GLConsoleRuleFnIAdapter.cpp: implementation of the GLConsoleRuleFnIAdapter class.
//
//////////////////////////////////////////////////////////////////////

#include <common/Defines.h>
#include <GLEXT/GLConsoleRuleFnIAdapter.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GLConsoleRuleFnIBooleanAdapter::GLConsoleRuleFnIBooleanAdapter(const char *name, bool &param)
	: name_(name), param_(param)
{
	GLConsole::instance()->addFunction(
		name, 
		this,
		GLConsoleRuleTypeBoolean, 
		GLConsoleRuleAccessTypeReadWrite);
}

GLConsoleRuleFnIBooleanAdapter::~GLConsoleRuleFnIBooleanAdapter()
{

}

bool GLConsoleRuleFnIBooleanAdapter::getBoolParam(const char *name)
{
	DIALOG_ASSERT(name_ == name);
	return param_;
}

void GLConsoleRuleFnIBooleanAdapter::setBoolParam(const char *name, bool value)
{
	DIALOG_ASSERT(name_ == name);
	param_ = value;
}

GLConsoleRuleFnIOptionsAdapter::GLConsoleRuleFnIOptionsAdapter(
	OptionEntry &entry,
	GLConsoleRuleAccessType access) :
	entry_(entry)
{
	GLConsoleRuleType type = GLConsoleRuleTypeBoolean;
	switch (entry.getEntryType())
	{
		case OptionEntry::OptionEntryEnumType:
		case OptionEntry::OptionEntryIntType:
		case OptionEntry::OptionEntryBoundedIntType:
		case OptionEntry::OptionEntryFloatType:
			type = GLConsoleRuleTypeNumber;
		break;
		case OptionEntry::OptionEntryVectorType:
		case OptionEntry::OptionEntryStringEnumType:
		case OptionEntry::OptionEntryStringType:
		case OptionEntry::OptionEntryTextType:
			type = GLConsoleRuleTypeString;
		break;
		case OptionEntry::OptionEntryBoolType:
			type = GLConsoleRuleTypeBoolean;
		break;
		default:
			DIALOG_ASSERT(0);
		break;
	}

	GLConsole::instance()->addFunction(
		entry.getName(), 
		this,
		type, 
		access);
}

GLConsoleRuleFnIOptionsAdapter::~GLConsoleRuleFnIOptionsAdapter()
{
	GLConsole::instance()->removeFunction(entry_.getName());
}

bool GLConsoleRuleFnIOptionsAdapter::getBoolParam(const char *name)
{
	return ((OptionEntryBool &) entry_).getValue();
}

void GLConsoleRuleFnIOptionsAdapter::setBoolParam(const char *name, bool value)
{
	((OptionEntryBool &) entry_).setValue(value);
}

float GLConsoleRuleFnIOptionsAdapter::getNumberParam(const char *name)
{
	return (float) (((OptionEntryInt &) entry_).getValue());
}

void  GLConsoleRuleFnIOptionsAdapter::setNumberParam(const char *name, float value)
{
	((OptionEntryInt &) entry_).setValue(int(value));
}

const char *GLConsoleRuleFnIOptionsAdapter::getStringParam(const char *name)
{
	return ((OptionEntryString &) entry_).getValue();
}

void GLConsoleRuleFnIOptionsAdapter::setStringParam(const char *name, const char *value)
{
	((OptionEntryString &) entry_).setValue(value);
}
