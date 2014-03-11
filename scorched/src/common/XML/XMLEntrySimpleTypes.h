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

#if !defined(__INCLUDE_XMLEntryTypesh_INCLUDE__)
#define __INCLUDE_XMLEntryTypesh_INCLUDE__

#include <common/Vector.h>
#include <common/FixedVector.h>
#include <common/ARGParser.h>
#include <XML/XMLEntry.h>
#include <XML/XMLParser.h>

class XMLEntrySimpleType : 
	public XMLEntry,
	public ARGParserBoolI, 
	public ARGParserIntI,
	public ARGParserStringI
{
public:
	enum XMLEntrySimpleTypeCatagory
	{
		eSimpleNumberType,
		eSimpleStringType,
		eSimpleBooleanType
	};

	XMLEntrySimpleType(const char *description, unsigned int data);
	virtual ~XMLEntrySimpleType();

	virtual void getDescription(std::string &result);
	virtual void getExtraDescription(std::string &description) { }

	virtual unsigned int getData() { return data_; }
	virtual bool isRequired() { return data_ & eDataRequired; }
	virtual void getDefaultValueAsString(std::string &result) = 0;
	virtual void getValueAsString(std::string &result) = 0;
	virtual bool setValueFromString(const std::string &string) = 0;
	virtual void resetDefaultValue();
	virtual XMLEntrySimpleTypeCatagory getTypeCatagory() = 0;

	// XMLEntry
	virtual bool readXML(XMLNode *parentNode, void *xmlData);
	virtual void writeXML(XMLNode *parentNode);

	// ARGParserBoolI, ARGParserIntI, ARGParserStringI
	virtual bool setBoolArgument(const char *strValue, bool value);
	virtual bool setIntArgument(const char *strValue, int value);
	virtual bool setStringArgument(const char *value);

	// TemplateProvider
	virtual void getStringProperty(TemplateData &data, std::string &result);
	virtual void getListProperty(TemplateData &data, std::list<TemplateProvider *> &result);
	virtual TemplateProvider *getChild(TemplateData &data, const std::string &name);
protected:
	unsigned int data_;
	const char *description_;
};

class XMLEntrySimpleContainer : public XMLEntryContainer
{
public:
	XMLEntrySimpleContainer(const char *typeName, const char *description, bool required = true);
	virtual ~XMLEntrySimpleContainer();

	XMLEntrySimpleType *getEntryByName(const std::string &name);

	// Fns used to save or restore the state of the options
	bool writeToBuffer(NetBuffer &buffer, bool useProtected);
	bool readFromBuffer(NetBufferReader &reader, bool useProtected);
	void addToArgParser(ARGParser &parser);
};

class XMLEntryInt : public XMLEntrySimpleType
{
public:
	XMLEntryInt(const char *description);
	XMLEntryInt(const char *description,
		unsigned int data,
		int defaultValue);
	virtual ~XMLEntryInt();

	virtual void getTypeName(std::string &result) { result = "number"; }
	virtual void getValueAsString(std::string &result);
	virtual void getDefaultValueAsString(std::string &result);
	virtual bool setValueFromString(const std::string &string);
	virtual XMLEntrySimpleTypeCatagory getTypeCatagory() { return eSimpleNumberType; }

	virtual int getValue();
	virtual bool setValue(int value);
protected:
	int defaultValue_;
	int value_;
};

class XMLEntryBoundedInt : public XMLEntryInt
{
public:
	XMLEntryBoundedInt(const char *description,
		unsigned int data,
		int defaultValue,
		int minValue, int maxValue, int stepValue);
	virtual ~XMLEntryBoundedInt();

	virtual void getExtraDescription(std::string &description);
	virtual bool setValue(int value);

	int getMinValue() { return minValue_; }
	int getMaxValue() { return maxValue_; }
	int getStepValue() { return stepValue_; }

protected:
	int minValue_, maxValue_;
	int stepValue_;

};

class XMLEntryUnsignedInt : public XMLEntrySimpleType
{
public:
	XMLEntryUnsignedInt(const char *description);
	XMLEntryUnsignedInt(const char *description,
		unsigned int data,
		unsigned int defaultValue);
	virtual ~XMLEntryUnsignedInt();

	virtual void getTypeName(std::string &result) { result = "number"; }
	virtual void getValueAsString(std::string &result);
	virtual void getDefaultValueAsString(std::string &result);
	virtual bool setValueFromString(const std::string &string);
	virtual XMLEntrySimpleTypeCatagory getTypeCatagory() { return eSimpleNumberType; }

	virtual unsigned int getValue();
	virtual bool setValue(unsigned int value);
protected:
	unsigned int defaultValue_;
	unsigned int value_;
};


class XMLEntryEnum : public XMLEntryInt
{
public:
	struct EnumEntry
	{
		std::string description;
		int value;
	};

	XMLEntryEnum(const char *description,
		XMLEntryEnum::EnumEntry enums[]);
	XMLEntryEnum(const char *description,
		unsigned int data,
		int value,
		XMLEntryEnum::EnumEntry enums[]);
	virtual ~XMLEntryEnum();

	virtual void getTypeName(std::string &result) { result = "string"; }
	virtual bool setValue(int value);
	virtual void getExtraDescription(std::string &description);

	virtual void getDefaultValueAsString(std::string &result);
	virtual void getValueAsString(std::string &result);
	virtual bool setValueFromString(const std::string &string);

	XMLEntryEnum::EnumEntry *getEnums() { return enums_; }

protected:
	EnumEntry *enums_;

};

class XMLEntryBool : public XMLEntrySimpleType
{
public:
	XMLEntryBool(const char *description);
	XMLEntryBool(const char *description,
		unsigned int data,
		bool defaultValue);
	virtual ~XMLEntryBool();

	virtual void getTypeName(std::string &result) { result = "boolean"; }
	virtual void getValueAsString(std::string &result);
	virtual void getDefaultValueAsString(std::string &result);
	virtual bool setValueFromString(const std::string &string);
	virtual XMLEntrySimpleTypeCatagory getTypeCatagory() { return eSimpleBooleanType; }

	virtual bool setValue(bool value);
	virtual bool getValue();

protected:
	bool defaultValue_;
	bool value_;
};

class XMLEntryString : public XMLEntrySimpleType
{
public:
	XMLEntryString(const char *description);
	XMLEntryString(const char *description,
		unsigned int data,
		const std::string &defaultValue,
		bool multiline = false);
	virtual ~XMLEntryString();

	virtual void getTypeName(std::string &result) { result = "string"; }
	virtual void getValueAsString(std::string &result);
	virtual void getDefaultValueAsString(std::string &result);
	virtual bool setValueFromString(const std::string &string);
	virtual XMLEntrySimpleTypeCatagory getTypeCatagory() { return eSimpleStringType; }

	virtual std::string &getValue();
	virtual bool setValue(const std::string &value);

protected:
	std::string value_;
	std::string defaultValue_;
	bool multiline_;
};

class XMLEntryFile : public XMLEntryString
{
public:
	XMLEntryFile(const char *description);
	XMLEntryFile(const char *description,
		unsigned int data,
		const std::string &defaultValue);
	virtual ~XMLEntryFile();

	virtual bool setValueFromString(const std::string &string);
protected:
	bool afile;
};

class XMLEntryStringEnum : public XMLEntryString
{
public:
	struct EnumEntry
	{
		std::string value;
	};

	XMLEntryStringEnum(const char *description,
		XMLEntryStringEnum::EnumEntry enums[]);
	XMLEntryStringEnum(const char *description,
		unsigned int data,
		const std::string &value,
		XMLEntryStringEnum::EnumEntry enums[]);
	virtual ~XMLEntryStringEnum();

	virtual bool setValue(const std::string &value);
	virtual void getExtraDescription(std::string &result);

	virtual bool setValueFromString(const std::string &string);

	XMLEntryStringEnum::EnumEntry *getEnums() { return enums_; }

protected:
	EnumEntry *enums_;
};

class XMLEntryFixed : public XMLEntrySimpleType
{
public:
	XMLEntryFixed(const char *description);
	XMLEntryFixed(const char *description,
		unsigned int data,
		fixed defaultValue);
	virtual ~XMLEntryFixed();

	virtual void getTypeName(std::string &result) { result = "number"; }
	virtual void getValueAsString(std::string &result);
	virtual void getDefaultValueAsString(std::string &result);
	virtual bool setValueFromString(const std::string &string);
	virtual XMLEntrySimpleTypeCatagory getTypeCatagory() { return eSimpleNumberType; }

	virtual fixed getValue();
	virtual bool setValue(fixed value);

protected:
	fixed defaultValue_;
	fixed value_;
};

class XMLEntryFixedVector : public XMLEntrySimpleType
{
public:
	XMLEntryFixedVector(const char *description);
	XMLEntryFixedVector(const char *description,
		unsigned int data,
		FixedVector defaultValue);
	virtual ~XMLEntryFixedVector();

	virtual void getTypeName(std::string &result) { result = "vector"; }
	virtual void getValueAsString(std::string &result);
	virtual void getDefaultValueAsString(std::string &result);
	virtual bool setValueFromString(const std::string &string);
	virtual XMLEntrySimpleTypeCatagory getTypeCatagory() { return eSimpleStringType; }

	virtual FixedVector &getValue();
	virtual bool setValue(FixedVector value);

protected:
	FixedVector defaultValue_;
	FixedVector value_;
};

#endif // __INCLUDE_XMLEntryTypesh_INCLUDE__
