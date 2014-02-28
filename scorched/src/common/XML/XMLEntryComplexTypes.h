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

#if !defined(__INCLUDE_XMLEntryComplexTypesh_INCLUDE__)
#define __INCLUDE_XMLEntryComplexTypesh_INCLUDE__

#include <XML/XMLEntrySimpleTypes.h>
#include <common/NumberParser.h>

class XMLEntryModelSpec : public XMLEntryContainer
{
public:
	XMLEntryModelSpec(const char *typeName, const char *description);
	virtual ~XMLEntryModelSpec();
};

class XMLEntryModelSpecDefinition : public XMLEntryModelSpec
{
public:
	XMLEntryModelSpecDefinition();
	virtual ~XMLEntryModelSpecDefinition();

	XMLEntryString meshName;
	XMLEntryFixed scale;
	XMLEntryFixed rotation;
	XMLEntryFixed brightness;
};

class XMLEntryModelSpecReference : public XMLEntryModelSpec
{
public:
	XMLEntryModelSpecReference();
	virtual ~XMLEntryModelSpecReference();

	XMLEntryString modelName;
};

class XMLEntryModel : public XMLEntryTypeChoice<XMLEntryModelSpec>
{
public:
	XMLEntryModel();
	virtual ~XMLEntryModel();

	virtual XMLEntryModelSpec *createXMLEntry(const std::string &type, void *xmlData);
	virtual void getAllTypes(std::set<std::string> &allTypes);
};

class XMLEntryParticleID : public XMLEntryContainer
{
public:
	XMLEntryParticleID(bool required = true);
	virtual ~XMLEntryParticleID();

	XMLEntryString particleName;
};

class XMLEntrySoundID : public XMLEntryContainer
{
public:
	XMLEntrySoundID(bool required = true);
	virtual ~XMLEntrySoundID();

	XMLEntryString soundFile;
	XMLEntryFixed gain;
	XMLEntryFixed rollOff;
	XMLEntryFixed referenceDistance;
};

class XMLEntryNumberParser : public XMLEntry
{
public:
	XMLEntryNumberParser(const char *parserName, const char *description);
	XMLEntryNumberParser(const char *parserName, const char *description,
		unsigned int data,
		const std::string &defaultValue);
	virtual ~XMLEntryNumberParser();

	virtual fixed getValue(ScorchedContext &context);
	void setValue(const std::string &value);

	// XMLEntry
	virtual bool readXML(XMLNode *node, void *xmlData);
	virtual void writeXML(XMLNode *node);
	virtual unsigned int getData() { return data_; }

	virtual void getTypeName(std::string &result);
	virtual void getDescription(std::string &result);

protected:
	const char *description_;
	unsigned int data_;
	NumberParser value_;
};

#endif // __INCLUDE_XMLEntryComplexTypesh_INCLUDE__
