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
#include <set>

class XMLEntryDocumentInfo
{
public:
	XMLEntryDocumentInfo();
	~XMLEntryDocumentInfo();
private:
};

class FileTemplateVariables;
class XMLEntryDocumentGenerator
{
public:
	XMLEntryDocumentGenerator(const std::string &documentLocation);
	~XMLEntryDocumentGenerator();

	void addType(const std::string &typeName, const std::string &fileName, FileTemplateVariables *variables);
	bool hasType(const std::string &typeName);
private:
	std::string documentLocation_;
	std::set<std::string> types_;
};

class XMLEntry
{
public:
	enum XMLEntryData
	{
		eDataRequired =  1,
		eDataDepricated = 2,
		eDataProtected = 4,
		eDataRWAccess = 8,
		eDataROAccess = 16,
		eDataNoRestore = 32,
		eDataDebugOnly = 64
	};

	XMLEntry();
	virtual ~XMLEntry();

	virtual bool readXML(XMLNode *node) = 0;
	virtual void writeXML(XMLNode *node) = 0;
	virtual unsigned int getData() = 0;

	virtual void getTypeName(std::string &result) = 0;
	virtual void getDescription(std::string &result) = 0;

	virtual XMLEntryDocumentInfo generateDocumentation(XMLEntryDocumentGenerator &generator) { XMLEntryDocumentInfo info; return info; }
protected:
};

template <class T>
class XMLEntryTypeChoice : public XMLEntry
{
public:
	XMLEntryTypeChoice(const char *typeName, const char *description) :
		XMLEntry(),
		xmlTypeName_(typeName), xmlDescription_(description), 
		value_(0)
	{

	}
	virtual ~XMLEntryTypeChoice()
	{
		delete value_;
		value_ = 0;
	}

	std::string &getChoiceType() { return type_; }
	T *getValue() { return value_; }
	virtual T *createXMLEntry(const std::string &type) = 0;

	// XMLEntry
	virtual unsigned int getData() { return eDataRequired; }
	virtual void getTypeName(std::string &result) { result = xmlTypeName_; }
	virtual void getDescription(std::string &result) { result = xmlDescription_; }

	virtual bool readXML(XMLNode *node)
	{
		if (!node->getNamedParameter("type", type_)) 
		{
			return node->returnError(S3D::formatStringBuffer(
				"Failed to find a required type attribute"));;
		}
		value_ = createXMLEntry(type_);
		if (!value_)
		{
			return node->returnError(S3D::formatStringBuffer(
				"Failed to create the type : \"%s\"", type_.c_str()));
		}
		if (!value_->readXML(node)) return false;

		return true;
	}
	virtual void writeXML(XMLNode *node)
	{
		node->addParameter(new XMLNode("type", type_, XMLNode::XMLParameterType));
		value_->writeXML(node);
	}

protected:
	std::string type_;
	T *value_;
	const char *xmlTypeName_, *xmlDescription_;
};

class XMLEntryContainer : public XMLEntry
{
public:
	XMLEntryContainer(const char *typeName, const char *description);
	virtual ~XMLEntryContainer();

	void addChildXMLEntry(const char *name1, XMLEntry *entry1);
	void addChildXMLEntry(const char *name1, XMLEntry *entry1, const char *name2, XMLEntry *entry2);
	void addChildXMLEntry(const char *name1, XMLEntry *entry1, const char *name2, XMLEntry *entry2, 
		const char *name3, XMLEntry *entry3);
	void addChildXMLEntry(const char *name1, XMLEntry *entry1, const char *name2, XMLEntry *entry2, 
		const char *name3, XMLEntry *entry3, const char *name4, XMLEntry *entry4);
	void addChildXMLEntry(const char *name1, XMLEntry *entry1, const char *name2, XMLEntry *entry2, 
		const char *name3, XMLEntry *entry3, const char *name4, XMLEntry *entry4, const char *name5, XMLEntry *entry5);
	void addChildXMLEntry(const char *name1, XMLEntry *entry1, const char *name2, XMLEntry *entry2, 
		const char *name3, XMLEntry *entry3, const char *name4, XMLEntry *entry4, const char *name5, XMLEntry *entry5, 
		const char *name6, XMLEntry *entry6);
	void addChildXMLEntry(const char *name1, XMLEntry *entry1, const char *name2, XMLEntry *entry2, 
		const char *name3, XMLEntry *entry3, const char *name4, XMLEntry *entry4, const char *name5, XMLEntry *entry5, 
		const char *name6, XMLEntry *entry6, const char *name7, XMLEntry *entry7);
	void addChildXMLEntry(const char *name1, XMLEntry *entry1, const char *name2, XMLEntry *entry2, 
		const char *name3, XMLEntry *entry3, const char *name4, XMLEntry *entry4, const char *name5, XMLEntry *entry5, 
		const char *name6, XMLEntry *entry6, const char *name7, XMLEntry *entry7, const char *name8, XMLEntry *entry8);
	void addChildXMLEntry(const char *name1, XMLEntry *entry1, const char *name2, XMLEntry *entry2, 
		const char *name3, XMLEntry *entry3, const char *name4, XMLEntry *entry4, const char *name5, XMLEntry *entry5, 
		const char *name6, XMLEntry *entry6, const char *name7, XMLEntry *entry7, const char *name8, XMLEntry *entry8, 
		const char *name9, XMLEntry *entry9);
	void addChildXMLEntry(const char *name1, XMLEntry *entry1, const char *name2, XMLEntry *entry2, 
		const char *name3, XMLEntry *entry3, const char *name4, XMLEntry *entry4, const char *name5, XMLEntry *entry5, 
		const char *name6, XMLEntry *entry6, const char *name7, XMLEntry *entry7, const char *name8, XMLEntry *entry8, 
		const char *name9, XMLEntry *entry9, const char *name10, XMLEntry *entry10);
	XMLEntryContainer *addChildXMLEntryContainer(const char *name, XMLEntryContainer *container);

	std::map<std::string, XMLEntry *> &getChildren() { return xmlEntryChildren_; }

	// XMLEntry
	virtual bool readXML(XMLNode *node);
	virtual void writeXML(XMLNode *node);
	virtual unsigned int getData() { return eDataRequired; }
	virtual void getTypeName(std::string &result) { result = xmlTypeName_; }
	virtual void getDescription(std::string &result) { result = xmlDescription_; }
	virtual XMLEntryDocumentInfo generateDocumentation(XMLEntryDocumentGenerator &generator);
protected:
	std::map<std::string, XMLEntry *> xmlEntryChildren_;
	const char *xmlTypeName_, *xmlDescription_;
};

template <class T>
class XMLEntryList : public XMLEntry
{
public:
	XMLEntryList(const char *typeName, const char *description) :
	  XMLEntry(),
	  xmlTypeName_(typeName), xmlDescription_(description)
	{
	}
	virtual ~XMLEntryList()
	{
		std::list<T *>::iterator itor = xmlEntryChildren_.begin(),
			end = xmlEntryChildren_.end();
		for (;itor!=end; ++itor)
		{
			delete *itor;
		}
		xmlEntryChildren_.clear();
	}
	
	std::list<T *> &getChildren() { return xmlEntryChildren_; }

	virtual T *createXMLEntry() = 0;

	// XMLEntry
	virtual unsigned int getData() { return 0; }
	virtual void getTypeName(std::string &result) { result = xmlTypeName_; }
	virtual void getDescription(std::string &result) { result = xmlDescription_; }

	virtual bool readXML(XMLNode *node)
	{
		T *newEntry = createXMLEntry();
		if (!newEntry->readXML(node)) return false;
		xmlEntryChildren_.push_back(newEntry);

		return true;
	}

	virtual void writeXML(XMLNode *node)
	{
		// TODO: Fixme
		std::list<T *>::iterator itor = xmlEntryChildren_.begin(),
			end = xmlEntryChildren_.end();
		for (;itor!=end; ++itor)
		{
			(*itor)->writeXML(node);
		}
	}
protected:
	std::list<T *> xmlEntryChildren_;
	const char *xmlTypeName_, *xmlDescription_;
};

#endif
