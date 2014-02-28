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

	virtual bool readXML(XMLNode *parentNode) = 0;
	virtual void writeXML(XMLNode *parentNode) = 0;
protected:
};

template <class T>
class XMLEntryTypeChoice : public XMLEntry
{
public:
	XMLEntryTypeChoice(const char *name, const char *description) :
		XMLEntry(),
		name_(name), description_(description), value_(0)
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
	virtual bool readXML(XMLNode *parentNode)
	{
		XMLNode *child = 0;
		if (!parentNode->getNamedChild(name_, child))
		{
			return false;
		}
		if (child->getNamedParameter("type", type_)) 
		{
			return false;
		}
		value_ = createXMLEntry(type_);
		if (!value_)
		{
			return child->returnError(S3D::formatStringBuffer(
				"Failed to create the type : \"%s\"", type_.c_str()));
		}
		if (!value_->readXML(child)) return false;

		return true;
	}
	virtual void writeXML(XMLNode *parentNode)
	{
		// Add the comments for this node
		parentNode->addChild(new XMLNode("", 
			S3D::formatStringBuffer("%s", description_), 
			XMLNode::XMLCommentType));

		// Add the actual node
		XMLNode *newNode = new XMLNode(name_);
		parentNode->addChild(newNode);
		newNode->addParameter(new XMLNode("type", type_, XMLNode::XMLParameterType));

		// Add contents
		value_->writeXML(newNode);
	}

protected:
	std::string type_;
	T *value_;
	const char *name_, *description_;
};

class XMLEntryContainer : public XMLEntry
{
public:
	XMLEntryContainer(const char *name, const char *description);
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

protected:
	std::list<XMLEntry *> xmlEntryChildren_;
	const char *xmlEntryName_, *xmlEntryDescription_;
};

class XMLEntryGroup : public XMLEntryContainer
{
public:
	XMLEntryGroup(const char *name, const char *description);
	virtual ~XMLEntryGroup();

	// XMLEntry
	virtual bool readXML(XMLNode *parentNode);
	virtual void writeXML(XMLNode *parentNode);
};

template <class T>
class XMLEntryList : public XMLEntry
{
public:
	XMLEntryList(const char *tagName, const char *description) :
	  XMLEntry(),
	  tagName_(tagName), description_(description)
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
	virtual bool readXML(XMLNode *parentNode)
	{
		XMLNode *node = 0;
		while (parentNode->getNamedChild(tagName_, node, false, false))
		{
			T *newEntry = createXMLEntry();
			if (!newEntry->readXML(parentNode)) return false;
			xmlEntryChildren_.push_back(newEntry);
		}

		return true;
	}

	virtual void writeXML(XMLNode *parentNode)
	{
		std::list<T *>::iterator itor = xmlEntryChildren_.begin(),
			end = xmlEntryChildren_.end();
		for (;itor!=end; ++itor)
		{
			(*itor)->writeXML(parentNode);
		}
	}
protected:
	std::list<T *> xmlEntryChildren_;
	const char *tagName_, *description_;
};

#endif
