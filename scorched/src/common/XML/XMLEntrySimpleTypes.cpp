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
#include <common/FileTemplate.h>
#include <XML/XMLFile.h>
#include <stdio.h>
#include <map>

XMLEntrySimpleType::XMLEntrySimpleType(const char *description, unsigned int data) :
	data_(data),
	description_(description)
{
}

XMLEntrySimpleType::~XMLEntrySimpleType()
{
}

void XMLEntrySimpleType::getDescription(std::string &result)
{
	result = description_;
	std::string extraDescription;
	getExtraDescription(extraDescription);
	if (!extraDescription.empty())
	{
		result.append(" (").append(extraDescription).append(")");
	}
}

bool XMLEntrySimpleType::readXML(XMLNode *node, void *xmlData)
{
	if (!setValueFromString(node->getContent()))
	{
		std::string typeName;
		getTypeName(typeName);
		return node->returnError(S3D::formatStringBuffer(
			"Failed to set %s XML entry with value \"%s\"",
			typeName.c_str(),
			node->getContent()));
	}
	return node->failChildren();
}

void XMLEntrySimpleType::writeXML(XMLNode *parentNode)
{
	std::string currentValue;
	getValueAsString(currentValue);
	parentNode->setContent(currentValue.c_str());
}

void XMLEntrySimpleType::resetDefaultValue()
{
	std::string defaultValue;
	getDefaultValueAsString(defaultValue);
	setValueFromString(defaultValue);
}

bool XMLEntrySimpleType::setBoolArgument(const char *strValue, bool value)
{
	return setValueFromString(strValue);
}

bool XMLEntrySimpleType::setIntArgument(const char *strValue, int value)
{
	return setValueFromString(strValue);
}

bool XMLEntrySimpleType::setStringArgument(const char *value)
{
	return setValueFromString(value);
}

XMLEntryDocumentInfo XMLEntrySimpleType::generateDocumentation(XMLEntryDocumentGenerator &generator)
{
	XMLEntryDocumentInfo info;
	return info;
}

XMLEntrySimpleContainer::XMLEntrySimpleContainer(const char *typeName, const char *description, bool required) :
	XMLEntryContainer(typeName, description, required)
{
}

XMLEntrySimpleContainer::~XMLEntrySimpleContainer()
{
}

XMLEntrySimpleType *XMLEntrySimpleContainer::getEntryByName(const std::string &name)
{
	std::map<std::string, XMLEntry *>::iterator itor = xmlEntryChildren_.find(name);
	if (itor == xmlEntryChildren_.end()) return 0;
	return (XMLEntrySimpleType*) itor->second;
}

bool XMLEntrySimpleContainer::writeToBuffer(NetBuffer &buffer, bool useProtected)
{
	// Write out all of the options
	// We do this as strings this make the message larger than it needs to be but
	// we always know how to read the options at the other end.
	// Even if the other end does not have all the options this end does.
	std::map<std::string, XMLEntry *>::iterator itor = xmlEntryChildren_.begin(),
		end = xmlEntryChildren_.end();
	for (;itor!=end;++itor)
	{
		XMLEntrySimpleType *entry = (XMLEntrySimpleType *) itor->second;
		// Only send data that is allowed to be sent
		if (!(entry->getData() & XMLEntry::eDataProtected) || useProtected)
		{
			// Optimization, only send options that have changed from thier default
			std::string currentValue, defaultValue;
			entry->getValueAsString(currentValue);
			entry->getDefaultValueAsString(defaultValue);
			if (currentValue != defaultValue)
			{
				buffer.addToBuffer(itor->first);
				buffer.addToBuffer(currentValue);
			}
		}
	}
	buffer.addToBuffer("-"); // Marks the end of the entries
	return true;
}

bool XMLEntrySimpleContainer::readFromBuffer(NetBufferReader &reader, bool useProtected)
{
	// Create a map from string name to existing options
	// So we can find the named option when reading the buffer
	std::map<std::string, XMLEntrySimpleType *> entryMap;
	std::map<std::string, XMLEntry *>::iterator itor = xmlEntryChildren_.begin(),
		end = xmlEntryChildren_.end();
	for (;itor!=end;++itor)
	{
		XMLEntrySimpleType *entry = (XMLEntrySimpleType *) itor->second;
		if (!(entry->getData() & XMLEntry::eDataProtected) || useProtected)
		{
			// Create map
			std::string name = itor->first;
			entryMap[name] = entry;

			std::string currentValue, defaultValue;
			entry->getValueAsString(currentValue);
			entry->getDefaultValueAsString(defaultValue);

			// Reset to the default value as only the non-default values are sent
			if (currentValue != defaultValue)
			{
				entry->setValueFromString(defaultValue);
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
			if (!entry->setValueFromString(value)) return false;
		}
	}

	return true;
}

void XMLEntrySimpleContainer::addToArgParser(ARGParser &parser)
{
	std::map<std::string, XMLEntrySimpleType *> entryMap;
	std::map<std::string, XMLEntry *>::iterator itor = xmlEntryChildren_.begin(),
		end = xmlEntryChildren_.end();
	for (;itor!=end;++itor)
	{
		XMLEntrySimpleType *entry = (XMLEntrySimpleType *) itor->second;

		std::string name = std::string("-") +  itor->first;
		std::string defaultValue, description;
		entry->getDefaultValueAsString(defaultValue);
		entry->getDescription(description);
		std::string fullDescription = S3D::formatStringBuffer("%s (Default : %s)",
			description.c_str(), defaultValue.c_str());

		switch (entry->getTypeCatagory())
		{
		case XMLEntrySimpleType::eSimpleNumberType:
			parser.addEntry(name, (ARGParserIntI *) entry, fullDescription);
			break;
		case XMLEntrySimpleType::eSimpleStringType:
			parser.addEntry(name , (ARGParserStringI *) entry, fullDescription);
			break;
		case XMLEntrySimpleType::eSimpleBooleanType:
			parser.addEntry(name , (ARGParserBoolI *) entry, fullDescription);
			break;
		}
	}
}

XMLEntryInt::XMLEntryInt(const char *description) :
	XMLEntrySimpleType(description, XMLEntrySimpleType::eDataRequired), 
	value_(0), defaultValue_(0)
{
}

XMLEntryInt::XMLEntryInt(const char *description, unsigned int data, int value) :
	XMLEntrySimpleType(description, data), 
	value_(value), defaultValue_(value)
{
	
}

XMLEntryInt::~XMLEntryInt()
{

}

void XMLEntryInt::getValueAsString(std::string &result)
{
	result = S3D::formatStringBuffer("%i", value_);
}

void XMLEntryInt::getDefaultValueAsString(std::string &result)
{
	result = S3D::formatStringBuffer("%i", defaultValue_);
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

XMLEntryBoundedInt::XMLEntryBoundedInt(const char *description,
	unsigned int data,
	int value,
	int minValue, int maxValue, int stepValue) :
	XMLEntryInt(description, data, value), 
	minValue_(minValue), maxValue_(maxValue), stepValue_(stepValue)
{
}

XMLEntryBoundedInt::~XMLEntryBoundedInt()
{
}

void XMLEntryBoundedInt::getExtraDescription(std::string &result)
{
	result = S3D::formatStringBuffer("Max = %i, Min = %i", getMaxValue(), getMinValue());
}

bool XMLEntryBoundedInt::setValue(int value)
{
	if (value < minValue_ || value > maxValue_) return false;
	return XMLEntryInt::setValue(value);
}

XMLEntryUnsignedInt::XMLEntryUnsignedInt(const char *description) :
	XMLEntrySimpleType(description, XMLEntrySimpleType::eDataRequired), 
	value_(0), defaultValue_(0)
{
}

XMLEntryUnsignedInt::XMLEntryUnsignedInt(const char *description, unsigned int data, unsigned int value) :
	XMLEntrySimpleType(description, data), 
	value_(value), defaultValue_(value)
{
	
}

XMLEntryUnsignedInt::~XMLEntryUnsignedInt()
{

}

void XMLEntryUnsignedInt::getValueAsString(std::string &result)
{
	result = S3D::formatStringBuffer("%u", value_);
}

void XMLEntryUnsignedInt::getDefaultValueAsString(std::string &result)
{
	result = S3D::formatStringBuffer("%u", defaultValue_);
}

bool XMLEntryUnsignedInt::setValueFromString(const std::string &string)
{
	int val;
	if (sscanf(string.c_str(), "%u", &val) != 1) return false;
	return setValue(val);
}

unsigned int XMLEntryUnsignedInt::getValue() 
{
	return value_;
}

bool XMLEntryUnsignedInt::setValue(unsigned int value)
{
	value_ = value;
	return true;
}

XMLEntryEnum::XMLEntryEnum(const char *description,
	XMLEntryEnum::EnumEntry enums[]) :
	XMLEntryInt(description), 
	enums_(enums)
{	
}

XMLEntryEnum::XMLEntryEnum(const char *description,
	unsigned int data,
	int value,
	XMLEntryEnum::EnumEntry enums[]) :
	XMLEntryInt(description, data, value), 
	enums_(enums)
{	
}

XMLEntryEnum::~XMLEntryEnum()
{
}

void XMLEntryEnum::getExtraDescription(std::string &result)
{
	result = "possible values = [";
	for (EnumEntry *current = enums_; !current->description.empty(); current++)
	{
		result += S3D::formatStringBuffer(" \"%s\" ",
			current->description.c_str(), current->value);
	}
	result += "]";
}

bool XMLEntryEnum::setValue(int value)
{
	for (EnumEntry *current = enums_; !current->description.empty(); current++)
	{
		if (current->value == value)
		{
			return XMLEntryInt::setValue(value);
		}
	}
	return false;
}

void XMLEntryEnum::getDefaultValueAsString(std::string &result)
{
	for (EnumEntry *current = enums_; !current->description.empty(); current++)
	{
		if (current->value == defaultValue_)
		{
			result = current->description;
			return;
		}
	}
	result = "-";
}

void XMLEntryEnum::getValueAsString(std::string &result)
{
	for (EnumEntry *current = enums_; !current->description.empty(); current++)
	{
		if (current->value == value_)
		{
			result = current->description;
			return;
		}
	}
	result = "-";
}

bool XMLEntryEnum::setValueFromString(const std::string &string)
{
	for (EnumEntry *current = enums_; !current->description.empty(); current++)
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
		std::string defaultValue;
		getDefaultValueAsString(defaultValue);
		return setValueFromString(defaultValue);
	}
	return setValue(val);
}

XMLEntryBool::XMLEntryBool(const char *description) :
	XMLEntrySimpleType(description, XMLEntrySimpleType::eDataRequired),
	value_(false), defaultValue_(false)
{
}

XMLEntryBool::XMLEntryBool(const char *description, 
	unsigned int data,
	bool value) :
	XMLEntrySimpleType(description, data), 
	value_(value), defaultValue_(value)
{
	
}

XMLEntryBool::~XMLEntryBool()
{
	
}

void XMLEntryBool::getDefaultValueAsString(std::string &result)
{
	result = (defaultValue_?"true":"false");
}

void XMLEntryBool::getValueAsString(std::string &result)
{
	result = (value_?"true":"false");
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

XMLEntryString::XMLEntryString(const char *description) :
	XMLEntrySimpleType(description, XMLEntrySimpleType::eDataRequired),
	value_(""), defaultValue_(""), multiline_(false)
{

}

XMLEntryString::XMLEntryString(const char *description, 
	unsigned int data,
	const std::string &value,
	bool multiline) :
	XMLEntrySimpleType(description, data),
	value_(value), defaultValue_(value), multiline_(multiline)
{
	
}

XMLEntryString::~XMLEntryString()
{

}

void XMLEntryString::getValueAsString(std::string &result)
{
	result = value_;
}

void XMLEntryString::getDefaultValueAsString(std::string &result)
{
	result = defaultValue_;
}

bool XMLEntryString::setValueFromString(const std::string &string)
{
	return setValue(string);
}

std::string &XMLEntryString::getValue() 
{
	return value_;
}

bool XMLEntryString::setValue(const std::string &value)
{
	value_ = value;
	return true;
}

XMLEntryFile::XMLEntryFile(const char *description) :
	XMLEntryString(description)
{
}

XMLEntryFile::XMLEntryFile(const char *description,
	unsigned int data,
	const std::string &defaultValue) :
	XMLEntryString(description, data, defaultValue)
{
}

XMLEntryFile::~XMLEntryFile()
{
}

bool XMLEntryFile::setValueFromString(const std::string &string)
{
	if (!S3D::checkDataFile(string)) return false;
	return XMLEntryString::setValueFromString(string);
}

XMLEntryStringEnum::XMLEntryStringEnum(const char *description,
	unsigned int data,
	const std::string &value,
	XMLEntryStringEnum::EnumEntry enums[]) :
	XMLEntryString(description, data, value), 
	enums_(enums)
{	
}

XMLEntryStringEnum::~XMLEntryStringEnum()
{
}

void XMLEntryStringEnum::getExtraDescription(std::string &result)
{
	result = "possible values = [";
	for (EnumEntry *current = enums_; !current->value.empty(); current++)
	{
		result += S3D::formatStringBuffer(" \"%s\" ",
			current->value.c_str());
	}
	result += "]";
}

bool XMLEntryStringEnum::setValue(const std::string &value)
{
	for (EnumEntry *current = enums_; !current->value.empty(); current++)
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

XMLEntryFixed::XMLEntryFixed(const char *description) :
	XMLEntrySimpleType(description, XMLEntrySimpleType::eDataRequired),
	value_(0), defaultValue_(0)
{
}

XMLEntryFixed::XMLEntryFixed(const char *description,
	unsigned int data,
	fixed value) :
	XMLEntrySimpleType(description, data), 
	value_(value), defaultValue_(value)
{
	
}

XMLEntryFixed::~XMLEntryFixed()
{

}

void XMLEntryFixed::getValueAsString(std::string &result)
{
	result = value_.asString();
}

void XMLEntryFixed::getDefaultValueAsString(std::string &result)
{
	result = defaultValue_.asString();
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

XMLEntryFixedVector::XMLEntryFixedVector(const char *description) :
	XMLEntrySimpleType(description, XMLEntrySimpleType::eDataRequired)
{

}

XMLEntryFixedVector::XMLEntryFixedVector(const char *description,
	unsigned int data,
	FixedVector value) :
	XMLEntrySimpleType(description, data), 
	value_(value), defaultValue_(value)
{
	
}

XMLEntryFixedVector::~XMLEntryFixedVector()
{

}

void XMLEntryFixedVector::getValueAsString(std::string &result)
{
	std::string a = value_[0].asString();
	std::string b = value_[1].asString();
	std::string c = value_[2].asString();

	result = a + " " + b + " " + c;
}

void XMLEntryFixedVector::getDefaultValueAsString(std::string &result)
{
	std::string a = defaultValue_[0].asString();
	std::string b = defaultValue_[1].asString();
	std::string c = defaultValue_[2].asString();

	result = a + " " + b + " " + c;
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
	if (i!=3) return false;
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
