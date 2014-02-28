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

class XMLEntrySimpleType : public XMLEntry
{
public:
	enum XMLEntrySimpleTypeCatagory
	{
		eSimpleNumberType,
		eSimpleStringType,
		eSimpleBooleanType
	};

	XMLEntrySimpleType(const char *name, const char *description, unsigned int data);
	virtual ~XMLEntrySimpleType();

	std::string getName() { return name_; }
	virtual std::string getDescription() { return description_; }
	virtual std::string getRangeDescription() { return ""; }

	virtual unsigned int getData() { return data_; }

	virtual std::string getDefaultValueAsString() = 0;
	virtual std::string getValueAsString() = 0;
	virtual bool setValueFromString(const std::string &string) = 0;
	virtual XMLEntrySimpleTypeCatagory getTypeCatagory() = 0;

	// XMLEntry
	virtual bool readXML(XMLNode *parentNode);
	virtual void writeXML(XMLNode *parentNode);
protected:
	unsigned int data_;
	const char *name_;
	const char *description_;
};

class XMLEntrySimpleGroup : public XMLEntryGroup
{
public:
	XMLEntrySimpleGroup(const char *name, const char *description);
	virtual ~XMLEntrySimpleGroup();

	XMLEntrySimpleType *getEntryByName(const std::string &name);

	// Fns used to save or restore the state of the options
	bool writeToFile(const std::string &filePath);
	bool readFromFile(const std::string &filePath);
	bool writeToBuffer(NetBuffer &buffer, bool useProtected);
	bool readFromBuffer(NetBufferReader &reader, bool useProtected);
};

class XMLEntryInt : public XMLEntrySimpleType
{
public:
	XMLEntryInt(const char *name, 
		const char *description);
	XMLEntryInt(const char *name, 
		const char *description,
		unsigned int data,
		int defaultValue);
	virtual ~XMLEntryInt();

	virtual std::string getValueAsString();
	virtual std::string getDefaultValueAsString();
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
	XMLEntryBoundedInt(const char *name, 
		const char *description,
		unsigned int data,
		int defaultValue,
		int minValue, int maxValue, int stepValue);
	virtual ~XMLEntryBoundedInt();

	virtual std::string getRangeDescription();
	virtual bool setValue(int value);

	int getMinValue() { return minValue_; }
	int getMaxValue() { return maxValue_; }
	int getStepValue() { return stepValue_; }

protected:
	int minValue_, maxValue_;
	int stepValue_;

};

class XMLEntryEnum : public XMLEntryInt
{
public:
	struct EnumEntry
	{
		std::string description;
		int value;
	};

	XMLEntryEnum(const char *name, 
		const char *description,
		unsigned int data,
		int value,
		XMLEntryEnum::EnumEntry enums[]);
	virtual ~XMLEntryEnum();

	virtual bool setValue(int value);
	virtual std::string getRangeDescription();

	virtual std::string getDefaultValueAsString();
	virtual std::string getValueAsString();
	virtual bool setValueFromString(const std::string &string);

	XMLEntryEnum::EnumEntry *getEnums() { return enums_; }

protected:
	EnumEntry *enums_;

};

class XMLEntryBool : public XMLEntrySimpleType
{
public:
	XMLEntryBool(const char *name, 
		const char *description);
	XMLEntryBool(const char *name, 
		const char *description,
		unsigned int data,
		bool defaultValue);
	virtual ~XMLEntryBool();

	virtual std::string getValueAsString();
	virtual std::string getDefaultValueAsString();
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
	XMLEntryString(const char *name,
		const char *description);
	XMLEntryString(const char *name,
		const char *description,
		unsigned int data,
		const std::string &defaultValue,
		bool multiline = false);
	virtual ~XMLEntryString();

	virtual std::string getValueAsString();
	virtual std::string getDefaultValueAsString();
	virtual bool setValueFromString(const std::string &string);
	virtual XMLEntrySimpleTypeCatagory getTypeCatagory() { return eSimpleStringType; }

	virtual std::string getValue();
	virtual bool setValue(const std::string &value);

protected:
	std::string value_;
	std::string defaultValue_;
	bool multiline_;
};

class XMLEntryStringEnum : public XMLEntryString
{
public:
	struct EnumEntry
	{
		std::string value;
	};

	XMLEntryStringEnum(const char *name, 
		const char *description,
		unsigned int data,
		const std::string &value,
		XMLEntryStringEnum::EnumEntry enums[]);
	virtual ~XMLEntryStringEnum();

	virtual bool setValue(const std::string &value);
	virtual std::string getRangeDescription();

	virtual bool setValueFromString(const std::string &string);

	XMLEntryStringEnum::EnumEntry *getEnums() { return enums_; }

protected:
	EnumEntry *enums_;
};

class XMLEntryFixed : public XMLEntrySimpleType
{
public:
	XMLEntryFixed(const char *name, 
		const char *description);
	XMLEntryFixed(const char *name, 
		const char *description,
		unsigned int data,
		fixed defaultValue);
	virtual ~XMLEntryFixed();

	virtual std::string getValueAsString();
	virtual std::string getDefaultValueAsString();
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
	XMLEntryFixedVector(const char *name, 
		const char *description);
	XMLEntryFixedVector(const char *name, 
		const char *description,
		unsigned int data,
		FixedVector defaultValue);
	virtual ~XMLEntryFixedVector();

	virtual std::string getValueAsString();
	virtual std::string getDefaultValueAsString();
	virtual bool setValueFromString(const std::string &string);
	virtual XMLEntrySimpleTypeCatagory getTypeCatagory() { return eSimpleStringType; }

	virtual FixedVector &getValue();
	virtual bool setValue(FixedVector value);

protected:
	FixedVector defaultValue_;
	FixedVector value_;
};

#endif // __INCLUDE_XMLEntryTypesh_INCLUDE__
