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

#include <XML/XMLEntrySimpleTypes.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <XML/XMLFile.h>
#include <stdio.h>
#include <map>

XMLEntrySimpleType::XMLEntrySimpleType(const char *name, const char *description, unsigned int data) :
	data_(data),
	name_(name),
	description_(description)
{
}

XMLEntrySimpleType::~XMLEntrySimpleType()
{
}

bool XMLEntrySimpleType::readXML(XMLNode *parentNode)
{
	bool required = data_ & eDataRequired;
	XMLNode *foundNode = 0;
	if (parentNode->getNamedChild(name_, foundNode, required)) 
	{
		if (!setValueFromString(foundNode->getContent()))
		{
			return foundNode->returnError(S3D::formatStringBuffer(
				"Failed to set XML etry \"%s\" with \"%s\"",
				name_, foundNode->getContent()));
		}
	}
	else if (required)
	{
		return false;
	}
	return true;
}

void XMLEntrySimpleType::writeXML(XMLNode *parentNode)
{
	if (data_ & eDataDepricated) return;

	bool required = data_ & eDataRequired;
	std::string name = getName(),
		description = getDescription(),
		defaultValue = getDefaultValueAsString(), 
		rangeDescription = getRangeDescription();
	if (rangeDescription.length() > 0) rangeDescription = " (" + rangeDescription + ")";

	// Add the comments for this node
	parentNode->addChild(new XMLNode("", 
		S3D::formatStringBuffer("%s%s.  %s%s", 
		description.c_str(), rangeDescription.c_str(),
		required?"Required.  ":"Optional, default value ",
		required?"":defaultValue.c_str()), 
		XMLNode::XMLCommentType));

	if (getValueAsString() == getDefaultValueAsString()) return;

	// Add the actual node
	XMLNode *newNode = new XMLNode(name_, getValueAsString());
	parentNode->addChild(newNode);
}

XMLEntrySimpleGroup::XMLEntrySimpleGroup(const char *name, const char *description) :
	XMLEntryGroup(name, description)
{
}

XMLEntrySimpleGroup::~XMLEntrySimpleGroup()
{
}

XMLEntrySimpleType *XMLEntrySimpleGroup::getEntryByName(const std::string &name)
{
	std::list<XMLEntry *>::iterator itor = xmlEntryChildren_.begin(),
		end = xmlEntryChildren_.end();
	for (;itor!=end;++itor)
	{
		XMLEntrySimpleType *entry = (XMLEntrySimpleType *) *itor;
		if (name == entry->getName())
		{
			return entry;
		}
	}
	return 0;
}

bool XMLEntrySimpleGroup::writeToBuffer(NetBuffer &buffer, bool useProtected)
{
	// Write out all of the options
	// We do this as strings this make the message larger than it needs to be but
	// we always know how to read the options at the other end.
	// Even if the other end does not have all the options this end does.
	std::list<XMLEntry *>::iterator itor = xmlEntryChildren_.begin(),
		end = xmlEntryChildren_.end();
	for (;itor!=end;++itor)
	{
		XMLEntrySimpleType *entry = (XMLEntrySimpleType *) *itor;
		// Only send data that is allowed to be sent
		if (!(entry->getData() & XMLEntry::eDataProtected) || useProtected)
		{
			// Optimization, only send options that have changed from thier default
			if (entry->getValueAsString() != entry->getDefaultValueAsString())
			{
				buffer.addToBuffer(entry->getName());
				buffer.addToBuffer(entry->getValueAsString());
			}
		}
	}
	buffer.addToBuffer("-"); // Marks the end of the entries
	return true;
}

bool XMLEntrySimpleGroup::readFromBuffer(NetBufferReader &reader, bool useProtected)
{
	// Create a map from string name to existing options
	// So we can find the named option when reading the buffer
	std::map<std::string, XMLEntrySimpleType *> entryMap;
	std::list<XMLEntry *>::iterator itor = xmlEntryChildren_.begin(),
		end = xmlEntryChildren_.end();
	for (;itor!=end;++itor)
	{
		XMLEntrySimpleType *entry = (XMLEntrySimpleType *) *itor;
		if (!(entry->getData() & XMLEntry::eDataProtected) || useProtected)
		{
			// Create map
			std::string name = entry->getName();
			entryMap[name] = entry;

			// Reset to the default value as only the non-default values are sent
			if (entry->getValueAsString() != entry->getDefaultValueAsString())
			{
				entry->setValueFromString(entry->getDefaultValueAsString());
			}
		}
	}

	// Read the strings from the other end
	for (;;)
	{
		std::string name, value;
		if (!reader.getFromBuffer(name)) return false;
		if (strcmp(name.c_str(), "-") == 0) break; // The end symbol
		if (!reader.getFromBuffer(value)) return false;
		
		std::map<std::string, XMLEntrySimpleType *>::iterator finditor =
			entryMap.find(name);
		if (finditor == entryMap.end())
		{
			Logger::log(S3D::formatStringBuffer("Warning:Does not support server option \"%s\"",
				name.c_str()));
		}
		else
		{
			XMLEntrySimpleType *entry = (*finditor).second;
			if (!entry->setValueFromString(value.c_str())) return false;
		}
	}

	return true;
}

bool XMLEntrySimpleGroup::writeToFile(const std::string &filePath)
{
	XMLNode tmpNode("tmp");
	writeXML(&tmpNode);
	if (tmpNode.getChildren().empty()) return false;
	XMLNode *rootNode = tmpNode.getChildren().front();
	if (!rootNode->writeToFile(filePath.c_str())) return false;
	return true;
}

bool XMLEntrySimpleGroup::readFromFile(const std::string &filePath)
{
	// Parse the XML file
	XMLFile file;
	if (!file.readFile(filePath.c_str()))
	{
		S3D::dialogMessage("Scorched3D Options", S3D::formatStringBuffer(
			"ERROR: Failed to parse file \"%s\"\n"
			"%s",
			filePath.c_str(),
			file.getParserError()));
		return false;
	}

	// return true for an empty file
	if (!file.getRootNode()) return true;

	// Read the options from the XML node
	if (!readXML(file.getRootNode())) return false;
	return true;
}

XMLEntryInt::XMLEntryInt(const char *name, 
	const char *description) :
	XMLEntrySimpleType(name, description, XMLEntrySimpleType::eDataRequired), 
	value_(0), defaultValue_(0)
{
}

XMLEntryInt::XMLEntryInt(const char *name, const char *description,
	unsigned int data, int value) :
	XMLEntrySimpleType(name, description, data), 
	value_(value), defaultValue_(value)
{
	
}

XMLEntryInt::~XMLEntryInt()
{

}

std::string XMLEntryInt::getValueAsString()
{
	return S3D::formatStringBuffer("%i", value_);
}

std::string XMLEntryInt::getDefaultValueAsString()
{
	return S3D::formatStringBuffer("%i", defaultValue_);
}

bool XMLEntryInt::setValueFromString(const std::string &string)
{
	int val;
	if (sscanf(string.c_str(), "%i", &val) != 1) return false;
	return setValue(val);
}

int XMLEntryInt::getValue() 
{
	return value_;
}

bool XMLEntryInt::setValue(int value)
{
	value_ = value;
	return true;
}

XMLEntryBoundedInt::XMLEntryBoundedInt(const char *name,
	const char *description,
	unsigned int data,
	int value,
	int minValue, int maxValue, int stepValue) :
	XMLEntryInt(name, description, data, value), 
	minValue_(minValue), maxValue_(maxValue), stepValue_(stepValue)
{
}

XMLEntryBoundedInt::~XMLEntryBoundedInt()
{
}

std::string XMLEntryBoundedInt::getRangeDescription()
{
	return S3D::formatStringBuffer("Max = %i, Min = %i", getMaxValue(), getMinValue());
}

bool XMLEntryBoundedInt::setValue(int value)
{
	if (value < minValue_ || value > maxValue_) return false;
	return XMLEntryInt::setValue(value);
}

XMLEntryEnum::XMLEntryEnum(const char *name,
	const char *description,
	unsigned int data,
	int value,
	XMLEntryEnum::EnumEntry enums[]) :
	XMLEntryInt(name, description, data, value), 
	enums_(enums)
{	
}

XMLEntryEnum::~XMLEntryEnum()
{
}

std::string XMLEntryEnum::getRangeDescription()
{
	std::string result;
	result = "possible values = [";
	for (EnumEntry *current = enums_; current->description[0]; current++)
	{
		result += S3D::formatStringBuffer(" \"%s\" ",
			current->description.c_str(), current->value);
	}
	result += "]";
	return result;
}

bool XMLEntryEnum::setValue(int value)
{
	for (EnumEntry *current = enums_; current->description[0]; current++)
	{
		if (current->value == value)
		{
			return XMLEntryInt::setValue(value);
		}
	}
	return false;
}

std::string XMLEntryEnum::getDefaultValueAsString()
{
	for (EnumEntry *current = enums_; current->description[0]; current++)
	{
		if (current->value == defaultValue_)
		{
			return current->description;
		}
	}
	return "-";
}

std::string XMLEntryEnum::getValueAsString()
{
	for (EnumEntry *current = enums_; current->description[0]; current++)
	{
		if (current->value == value_)
		{
			return current->description;
		}
	}
	return "-";
}

bool XMLEntryEnum::setValueFromString(const std::string &string)
{
	for (EnumEntry *current = enums_; current->description[0]; current++)
	{
		if (current->description == string)
		{
			setValue(current->value);
			return true;
		}
	}

	int val;
	if (sscanf(string.c_str(), "%i", &val) != 1)
	{
		return setValueFromString(getDefaultValueAsString());
	}
	return setValue(val);
}

XMLEntryBool::XMLEntryBool(const char *name, 
	const char *description) :
	XMLEntrySimpleType(name, description, XMLEntrySimpleType::eDataRequired),
	value_(false), defaultValue_(false)
{
}

XMLEntryBool::XMLEntryBool(const char *name, 
	const char *description, 
	unsigned int data,
	bool value) :
	XMLEntrySimpleType(name, description, data), 
	value_(value), defaultValue_(value)
{
	
}

XMLEntryBool::~XMLEntryBool()
{
	
}

std::string XMLEntryBool::getDefaultValueAsString()
{
	return (defaultValue_?"true":"false");
}

std::string XMLEntryBool::getValueAsString()
{
	return (value_?"true":"false");
}

bool XMLEntryBool::setValueFromString(const std::string &string)
{
	if (stricmp("true", string.c_str()) == 0)
	{
		setValue(true);
		return true;
	}
	if (stricmp("false", string.c_str()) == 0)
	{
		setValue(false);
		return true;
	}
	return false;
}

bool XMLEntryBool::setValue(bool value)
{
	value_ = (value?1:0);
	return true;
}

bool XMLEntryBool::getValue()
{
	return ((value_==0)?false:true);
}

XMLEntryString::XMLEntryString(const char *name,
	const char *description) :
	XMLEntrySimpleType(name, description, XMLEntrySimpleType::eDataRequired),
	value_(""), defaultValue_(""), multiline_(false)
{

}

XMLEntryString::XMLEntryString(const char *name, 
	const char *description, 
	unsigned int data,
	const std::string &value,
	bool multiline) :
	XMLEntrySimpleType(name, description, data),
	value_(value), defaultValue_(value), multiline_(multiline)
{
	
}

XMLEntryString::~XMLEntryString()
{

}

std::string XMLEntryString::getValueAsString()
{
	return value_;
}

std::string XMLEntryString::getDefaultValueAsString()
{
	return defaultValue_;
}

bool XMLEntryString::setValueFromString(const std::string &string)
{
	return setValue(string);
}

std::string XMLEntryString::getValue() 
{
	return value_;
}

bool XMLEntryString::setValue(const std::string &value)
{
	value_ = value;
	return true;
}

XMLEntryStringEnum::XMLEntryStringEnum(const char *name,
	const char *description,
	unsigned int data,
	const std::string &value,
	XMLEntryStringEnum::EnumEntry enums[]) :
	XMLEntryString(name, description, data, value), 
	enums_(enums)
{	
}

XMLEntryStringEnum::~XMLEntryStringEnum()
{
}

std::string XMLEntryStringEnum::getRangeDescription()
{
	std::string result;
	result = "possible values = [";
	for (EnumEntry *current = enums_; current->value[0]; current++)
	{
		result += S3D::formatStringBuffer(" \"%s\" ",
			current->value.c_str());
	}
	result += "]";
	return result;
}

bool XMLEntryStringEnum::setValue(const std::string &value)
{
	for (EnumEntry *current = enums_; current->value[0]; current++)
	{
		if (current->value == value)
		{
			return XMLEntryString::setValue(value);
		}
	}
	return false;
}

bool XMLEntryStringEnum::setValueFromString(const std::string &string)
{
	return setValue(string);
}

XMLEntryFixed::XMLEntryFixed(const char *name, 
		const char *description) :
	XMLEntrySimpleType(name, description, XMLEntrySimpleType::eDataRequired),
	value_(0), defaultValue_(0)
{
}

XMLEntryFixed::XMLEntryFixed(const char *name,
	const char *description,
	unsigned int data,
	fixed value) :
	XMLEntrySimpleType(name, description, data), 
	value_(value), defaultValue_(value)
{
	
}

XMLEntryFixed::~XMLEntryFixed()
{

}

std::string XMLEntryFixed::getValueAsString()
{
	return value_.asString();
}

std::string XMLEntryFixed::getDefaultValueAsString()
{
	return defaultValue_.asString();
}

bool XMLEntryFixed::setValueFromString(const std::string &string)
{
	return setValue(fixed(string.c_str()));
}

fixed XMLEntryFixed::getValue() 
{
	return value_;
}

bool XMLEntryFixed::setValue(fixed value)
{
	value_ = value;
	return true;
}

XMLEntryFixedVector::XMLEntryFixedVector(const char *name, 
	const char *description) :
	XMLEntrySimpleType(name, description, XMLEntrySimpleType::eDataRequired)
{

}

XMLEntryFixedVector::XMLEntryFixedVector(const char *name,
	const char *description,
	unsigned int data,
	FixedVector value) :
	XMLEntrySimpleType(name, description, data), 
	value_(value), defaultValue_(value)
{
	
}

XMLEntryFixedVector::~XMLEntryFixedVector()
{

}

std::string XMLEntryFixedVector::getValueAsString()
{
	std::string a = value_[0].asString();
	std::string b = value_[1].asString();
	std::string c = value_[2].asString();

	char value[256];
	snprintf(value, 256, "%s %s %s", 
		a.c_str(), b.c_str(), c.c_str());
	return value;
}

std::string XMLEntryFixedVector::getDefaultValueAsString()
{
	std::string a = defaultValue_[0].asString();
	std::string b = defaultValue_[1].asString();
	std::string c = defaultValue_[2].asString();

	char value[256];
	snprintf(value, 256, "%s %s %s", 
		a.c_str(), b.c_str(), c.c_str());
	return value;
}

bool XMLEntryFixedVector::setValueFromString(const std::string &string)
{
	FixedVector value;

	int i=0;
	char *token = strtok((char *) string.c_str(), " ");
	while(token != 0)
	{
		value[i++] = fixed(token);		
		token = strtok(0, " ");
	}

	return setValue(value);
}

FixedVector &XMLEntryFixedVector::getValue() 
{
	return value_;
}

bool XMLEntryFixedVector::setValue(FixedVector value)
{
	value_ = value;
	return true;
}
