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

#include <common/Defines.h>
#include <console/Console.h>
#include <console/ConsoleRuleOptionsAdapter.h>
#include <client/ScorchedClient.h>

ConsoleRuleOptionsAdapter::ConsoleRuleOptionsAdapter(Console &console, const std::string &name, XMLEntrySimpleType &entry) :
	console_(console),
	name_(name),
	entry_(entry),
	readRule_(0), writeRule_(0)
{
	ConsoleRuleType type = ConsoleRuleTypeNone;
	switch (entry_.getTypeCatagory())
	{
		case XMLEntrySimpleType::eSimpleNumberType:
			type = ConsoleRuleTypeNumber;
		break;
		case XMLEntrySimpleType::eSimpleStringType:
			type = ConsoleRuleTypeString;
		break;
		case XMLEntrySimpleType::eSimpleBooleanType:
			type = ConsoleRuleTypeBoolean;
		break;
		default:
			DIALOG_ASSERT(0);
		break;
	}

	if (entry.getData() & XMLEntry::eDataRWAccess ||
		entry.getData() & XMLEntry::eDataROAccess) {
		std::string description;
		entry.getDescription(description);
		readRule_ = new ConsoleRuleMethodIAdapterEx<ConsoleRuleOptionsAdapter>(
			console, this,
			&ConsoleRuleOptionsAdapter::readValue, 
			"get", ConsoleUtil::formParams(ConsoleRuleParam(name)),
			description);
		console_.addRule(readRule_);
	}
	if (entry.getData() & XMLEntry::eDataRWAccess) 
	{
		std::string description;
		entry.getDescription(description);
		writeRule_ = new ConsoleRuleMethodIAdapterEx<ConsoleRuleOptionsAdapter>(
			console, this,
			&ConsoleRuleOptionsAdapter::writeValue, 
			"set", ConsoleUtil::formParams(ConsoleRuleParam(name), ConsoleRuleParam("value", type)),
			description);
		console_.addRule(writeRule_);
	}
}

ConsoleRuleOptionsAdapter::~ConsoleRuleOptionsAdapter()
{
	if (readRule_)
	{
		delete readRule_;
	}
	if (writeRule_)
	{
		delete writeRule_;
	}
}

void ConsoleRuleOptionsAdapter::readValue(std::vector<ConsoleRuleValue> &values, unsigned int userData)
{
	std::string currentValue;
	entry_.getValueAsString(currentValue);

	CEGUI::String result = CEGUI::String(name_) + " = " + currentValue;
	console_.addLine(false, result);
}

void ConsoleRuleOptionsAdapter::writeValue(std::vector<ConsoleRuleValue> &values, unsigned int userData)
{
	entry_.setValueFromString(values[2].valueString);
	std::string currentValue;
	entry_.getValueAsString(currentValue);

	CEGUI::String result = CEGUI::String(name_) + " = " + currentValue;
	console_.addLine(false, result);
}

ConsoleRuleOptionsAdapterHolder::ConsoleRuleOptionsAdapterHolder()
{

}

void ConsoleRuleOptionsAdapterHolder::addToConsole(Console &console, std::map<std::string, XMLEntry *> &options)
{
	std::map<std::string, XMLEntry *>::iterator itor;
	for (itor = options.begin();
		itor != options.end();
		++itor)
	{
		ConsoleRuleOptionsAdapter *adapter = new ConsoleRuleOptionsAdapter(console, itor->first, (XMLEntrySimpleType &) *itor->second);
		adapters_.push_back(adapter);
	}
}

ConsoleRuleOptionsAdapterHolder::~ConsoleRuleOptionsAdapterHolder()
{
	while (!adapters_.empty())
	{
		delete adapters_.back();
		adapters_.pop_back();
	}
}
