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


#if !defined(__INCLUDE_OptionEntryh_INCLUDE__)
#define __INCLUDE_OptionEntryh_INCLUDE__

#include <net/NetBuffer.h>
#include <common/Vector.h>
#include <common/FixedVector.h>
#include <common/ARGParser.h>
#include <XML/XMLParser.h>

class OptionEntry
{
public:
	enum StandardData
	{
		DataNoAccess = 0,
		DataRWAccess = 1,
		DataRAccess = 2,
		DataNoRestore = 4,
		DataDebugOnly = 8,

		DataDepricated = 128,
		DataProtected = 256
	};

	enum EntryType
	{
		OptionEntryIntType,
		OptionEntryStringType,
		OptionEntryTextType,
		OptionEntryBoolType,
		OptionEntryFloatType,
		OptionEntryFixedType,
		OptionEntryVectorType,
		OptionEntryFixedVectorType,
		OptionEntryEnumType,
		OptionEntryStringEnumType,
		OptionEntryBoundedIntType
	};

	OptionEntry(std::list<OptionEntry *> &group,
				const std::string &name,
				const std::string &description,
				unsigned int data);
	virtual ~OptionEntry();

	std::string getName() { return name_; }
	virtual std::string getDescription() { return description_.c_str(); }
	virtual std::string getRangeDescription() { return ""; }
	virtual unsigned getData() { return data_; }

	virtual EntryType getEntryType() = 0;
	virtual std::string getDefaultValueAsString() = 0;
	virtual std::string getValueAsString() = 0;
	virtual bool isDefaultValue() = 0;
	virtual bool isChangedValue() { return changedValue_; }
	virtual bool setValueFromString(const std::string &string) = 0;
	virtual bool addToArgParser(ARGParser &parser) = 0;
	virtual void setNotChanged() { changedValue_ = false; }

	virtual std::string getComsBufferValue() 
		{ return getValueAsString(); }
	virtual bool setComsBufferValue(const std::string &string) 
		{ return setValueFromString(string); }

protected:
	bool changedValue_;
	unsigned int data_;
	std::string name_;
	std::string description_;
};

class OptionEntryHelper
{
public:
	static bool writeToBuffer(std::list<OptionEntry *> &options,
							  NetBuffer &buffer,
							  bool useprotected);
	static bool readFromBuffer(std::list<OptionEntry *> &options,
							   NetBufferReader &reader,
							   bool useprotected);

	static bool writeToXML(std::list<OptionEntry *> &options,
								XMLNode *node, bool allOptions);
	static bool readFromXML(std::list<OptionEntry *> &options,
								XMLNode *node);

	static bool writeToFile(std::list<OptionEntry *> &options,
		const std::string &fileName, bool allOptions);
	static bool readFromFile(std::list<OptionEntry *> &options,
		const std::string &fileName);

	static bool addToArgParser(std::list<OptionEntry *> &options,
								ARGParser &parser);

	static OptionEntry *getEntry(std::list<OptionEntry *> &options,
		const std::string &name);
};

class OptionEntryInt : public OptionEntry, public ARGParserIntI
{
public:
	OptionEntryInt(std::list<OptionEntry *> &group,
				   const std::string &name, 
				   const std::string &description,
				   unsigned int data,
				   int defaultValue);
	virtual ~OptionEntryInt();

	virtual EntryType getEntryType() { return OptionEntryIntType; }
	virtual std::string getValueAsString();
	virtual std::string getDefaultValueAsString();
	virtual bool setValueFromString(const std::string &string);

	virtual int getValue();
	virtual bool setValue(int value);

	virtual bool addToArgParser(ARGParser &parser);
	virtual bool setIntArgument(int value);
	virtual bool isDefaultValue() { return value_ == defaultValue_; }

	operator int () { return value_; }

protected:
	int defaultValue_;
	int value_;

};

class OptionEntryBoundedInt : public OptionEntryInt
{
public:
	OptionEntryBoundedInt(std::list<OptionEntry *> &group,
						  const std::string &name, 
						  const std::string &description,
						  unsigned int data,
						  int defaultValue,
						  int minValue, int maxValue, int stepValue);
	virtual ~OptionEntryBoundedInt();

	virtual std::string getRangeDescription();
	virtual EntryType getEntryType() { return OptionEntryBoundedIntType; }
	virtual bool setValue(int value);

	int getMinValue() { return minValue_; }
	int getMaxValue() { return maxValue_; }
	int getStepValue() { return stepValue_; }

protected:
	int minValue_, maxValue_;
	int stepValue_;

};

class OptionEntryEnum : public OptionEntryInt
{
public:
	struct EnumEntry
	{
		std::string description;
		int value;
	};

	OptionEntryEnum(std::list<OptionEntry *> &group,
						  const std::string &name, 
						  const std::string &description,
						  unsigned int data,
						  int value,
						  OptionEntryEnum::EnumEntry enums[]);
	virtual ~OptionEntryEnum();

	virtual EntryType getEntryType() { return OptionEntryEnumType; }
	virtual bool setValue(int value);
	virtual std::string getRangeDescription();

	virtual std::string getDefaultValueAsString();
	virtual std::string getValueAsString();
	virtual bool setValueFromString(const std::string &string);

	virtual std::string getComsBufferValue() 
		{ return OptionEntryInt::getValueAsString(); }
	virtual bool setComsBufferValue(const std::string &string) 
		{ return OptionEntryInt::setValueFromString(string); }

	OptionEntryEnum::EnumEntry *getEnums() { return enums_; }

protected:
	EnumEntry *enums_;

};

class OptionEntryBool : public OptionEntry, public ARGParserBoolI
{
public:
	OptionEntryBool(std::list<OptionEntry *> &group,
					const std::string &name, 
					const std::string &description,
					unsigned int data,
					bool defaultValue);
	virtual ~OptionEntryBool();

	virtual EntryType getEntryType() { return OptionEntryBoolType; }
	virtual std::string getValueAsString();
	virtual std::string getDefaultValueAsString();
	virtual bool setValueFromString(const std::string &string);

	virtual bool setValue(bool value);
	virtual bool getValue();

	virtual bool addToArgParser(ARGParser &parser);
	virtual bool setBoolArgument(bool value);
	virtual bool isDefaultValue() { return value_ == defaultValue_; }

	operator bool () { return value_; }

protected:
	bool defaultValue_;
	bool value_;
};

class OptionEntryString : public OptionEntry, public ARGParserStringI
{
public:
	OptionEntryString(std::list<OptionEntry *> &group,
					  const std::string &name,
					  const std::string &description,
					  unsigned int data,
					  const std::string &defaultValue,
					  bool multiline = false);
	virtual ~OptionEntryString();

	virtual EntryType getEntryType() { return (multiline_?OptionEntryTextType:OptionEntryStringType); }
	virtual std::string getValueAsString();
	virtual std::string getDefaultValueAsString();
	virtual bool setValueFromString(const std::string &string);

	virtual std::string getValue();
	virtual bool setValue(const std::string &value);

	virtual bool addToArgParser(ARGParser &parser);
	virtual bool setStringArgument(const char* value);

	operator const char *() { return value_.c_str(); }
	virtual bool isDefaultValue() { return value_ == defaultValue_; }

protected:
	std::string value_;
	std::string defaultValue_;
	bool multiline_;

};

class OptionEntryStringEnum : public OptionEntryString
{
public:
	struct EnumEntry
	{
		std::string value;
	};

	OptionEntryStringEnum(std::list<OptionEntry *> &group,
						  const std::string &name, 
						  const std::string &description,
						  unsigned int data,
						  const std::string &value,
						  OptionEntryStringEnum::EnumEntry enums[]);
	virtual ~OptionEntryStringEnum();

	virtual EntryType getEntryType() { return OptionEntryStringEnumType; }
	virtual bool setValue(const std::string &value);
	virtual std::string getRangeDescription();

	virtual bool setValueFromString(const std::string &string);

	OptionEntryStringEnum::EnumEntry *getEnums() { return enums_; }

protected:
	EnumEntry *enums_;

};

class OptionEntryFloat : public OptionEntry
{
public:
	OptionEntryFloat(std::list<OptionEntry *> &group,
				   const std::string &name, 
				   const std::string &description,
				   unsigned int data,
				   float defaultValue,
				   bool truncate = false);
	virtual ~OptionEntryFloat();

	virtual EntryType getEntryType() { return OptionEntryFloatType; }
	virtual std::string getValueAsString();
	virtual std::string getDefaultValueAsString();
	virtual bool setValueFromString(const std::string &string);

	virtual float getValue();
	virtual bool setValue(float value);

	virtual bool addToArgParser(ARGParser &parser);
	virtual bool isDefaultValue() { return value_ == defaultValue_; }

	operator float () { return value_; }

protected:
	bool truncate_;
	float defaultValue_;
	float value_;

};

class OptionEntryVector : public OptionEntry
{
public:
	OptionEntryVector(std::list<OptionEntry *> &group,
				   const std::string &name, 
				   const std::string &description,
				   unsigned int data,
				   Vector defaultValue,
				   bool truncate = false);
	virtual ~OptionEntryVector();

	virtual EntryType getEntryType() { return OptionEntryVectorType; }
	virtual std::string getValueAsString();
	virtual std::string getDefaultValueAsString();
	virtual bool setValueFromString(const std::string &string);

	virtual Vector &getValue();
	virtual bool setValue(Vector value);

	virtual bool addToArgParser(ARGParser &parser);
	virtual bool isDefaultValue() { return value_ == defaultValue_; }

	operator Vector() { return value_; }

protected:
	bool truncate_;
	Vector defaultValue_;
	Vector value_;

};

class OptionEntryFixed : public OptionEntry
{
public:
	OptionEntryFixed(std::list<OptionEntry *> &group,
				   const std::string &name, 
				   const std::string &description,
				   unsigned int data,
				   fixed defaultValue);
	virtual ~OptionEntryFixed();

	virtual EntryType getEntryType() { return OptionEntryFixedType; }
	virtual std::string getValueAsString();
	virtual std::string getDefaultValueAsString();
	virtual bool setValueFromString(const std::string &string);

	virtual fixed getValue();
	virtual bool setValue(fixed value);

	virtual bool addToArgParser(ARGParser &parser);
	virtual bool isDefaultValue() { return value_ == defaultValue_; }

	operator fixed () { return value_; }

protected:
	fixed defaultValue_;
	fixed value_;

};

class OptionEntryFixedVector : public OptionEntry
{
public:
	OptionEntryFixedVector(std::list<OptionEntry *> &group,
				   const std::string &name, 
				   const std::string &description,
				   unsigned int data,
				   FixedVector defaultValue);
	virtual ~OptionEntryFixedVector();

	virtual EntryType getEntryType() { return OptionEntryFixedVectorType; }
	virtual std::string getValueAsString();
	virtual std::string getDefaultValueAsString();
	virtual bool setValueFromString(const std::string &string);

	virtual FixedVector &getValue();
	virtual bool setValue(FixedVector value);

	virtual bool addToArgParser(ARGParser &parser);
	virtual bool isDefaultValue() { return value_ == defaultValue_; }

	operator FixedVector() { return value_; }

protected:
	FixedVector defaultValue_;
	FixedVector value_;

};

#endif
