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

#if !defined(__INCLUDE_XMLEntryh_INCLUDE__)
#define __INCLUDE_XMLEntryh_INCLUDE__

#include <XML/XMLParser.h>

class XMLEntry
{
public:
	XMLEntry();
	virtual ~XMLEntry();

	virtual bool readXML(XMLNode *parentNode) = 0;
	virtual void writeXML(XMLNode *parentNode) = 0;
protected:
};

class XMLEntryFactory
{
public:
	virtual XMLEntry *createXMLEntry() = 0;
};

class XMLEntryTypeFactory
{
public:
	virtual XMLEntry *createXMLEntry(const std::string &type) = 0;
};

class XMLEntryTypeChoice : public XMLEntry
{
public:
	XMLEntryTypeChoice(const std::string &name, const std::string &description);
	virtual ~XMLEntryTypeChoice();

	void setXMLEntryFactory(XMLEntryTypeFactory *factory) { factory_ = factory; }
	XMLEntry *getValue() { return value_; }

	// XMLEntry
	virtual bool readXML(XMLNode *parentNode);
	virtual void writeXML(XMLNode *parentNode);

protected:
	std::string type_;
	XMLEntry *value_;
	XMLEntryTypeFactory *factory_;
	std::string name_, description_;
};

class XMLEntryContainer : public XMLEntry
{
public:
	XMLEntryContainer();
	virtual ~XMLEntryContainer();

	void addChildXMLEntry(XMLEntry *entry1);
	void addChildXMLEntry(XMLEntry *entry1, XMLEntry *entry2);
	void addChildXMLEntry(XMLEntry *entry1, XMLEntry *entry2, XMLEntry *entry3);
	void addChildXMLEntry(XMLEntry *entry1, XMLEntry *entry2, XMLEntry *entry3, XMLEntry *entry4);
	void addChildXMLEntry(XMLEntry *entry1, XMLEntry *entry2, XMLEntry *entry3, XMLEntry *entry4, XMLEntry *entry5);
	void addChildXMLEntry(XMLEntry *entry1, XMLEntry *entry2, XMLEntry *entry3, XMLEntry *entry4, XMLEntry *entry5,
		XMLEntry *entry6);
	void addChildXMLEntry(XMLEntry *entry1, XMLEntry *entry2, XMLEntry *entry3, XMLEntry *entry4, XMLEntry *entry5,
		XMLEntry *entry6, XMLEntry *entry7);
	void addChildXMLEntry(XMLEntry *entry1, XMLEntry *entry2, XMLEntry *entry3, XMLEntry *entry4, XMLEntry *entry5,
		XMLEntry *entry6, XMLEntry *entry7, XMLEntry *entry8);
	void addChildXMLEntry(XMLEntry *entry1, XMLEntry *entry2, XMLEntry *entry3, XMLEntry *entry4, XMLEntry *entry5,
		XMLEntry *entry6, XMLEntry *entry7, XMLEntry *entry8, XMLEntry *entry9);
	void addChildXMLEntry(XMLEntry *entry1, XMLEntry *entry2, XMLEntry *entry3, XMLEntry *entry4, XMLEntry *entry5,
		XMLEntry *entry6, XMLEntry *entry7, XMLEntry *entry8, XMLEntry *entry9, XMLEntry *entry10);
	XMLEntryContainer *addChildXMLEntryContainer(XMLEntryContainer *container);

	// XMLEntry
	virtual bool readXML(XMLNode *parentNode);
	virtual void writeXML(XMLNode *parentNode);

private:
	std::list<XMLEntry *> xmlEntryChildren_;
};

class XMLEntryNamedContainer : public XMLEntryContainer
{
public:
	XMLEntryNamedContainer(const std::string &name, const std::string &description);
	virtual ~XMLEntryNamedContainer();

protected:
	std::string xmlEntryName_, xmlEntryDescription_;
};

class XMLEntryGroup : public XMLEntryNamedContainer
{
public:
	XMLEntryGroup(const std::string &name, const std::string &description);
	virtual ~XMLEntryGroup();

	// XMLEntry
	virtual bool readXML(XMLNode *parentNode);
	virtual void writeXML(XMLNode *parentNode);
};

class XMLEntryList : public XMLEntry
{
public:
	XMLEntryList();
	virtual ~XMLEntryList();
	
	std::list<XMLEntry *> getChildren() { return xmlEntryChildren_; }

	// XMLEntry
	virtual bool readXML(XMLNode *parentNode);
	virtual void writeXML(XMLNode *parentNode);
protected:
	std::list<XMLEntry *> xmlEntryChildren_;
};

#endif
