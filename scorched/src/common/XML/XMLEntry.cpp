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

#include <XML/XMLEntry.h>

XMLEntry::XMLEntry()
{
}

XMLEntry::~XMLEntry()
{
}

XMLEntryContainer::XMLEntryContainer()
{
}

XMLEntryContainer::~XMLEntryContainer()
{
}

void XMLEntryContainer::addChildXMLEntry(XMLEntry *entry1)
{
	xmlEntryChildren_.push_back(entry1);
}

void XMLEntryContainer::addChildXMLEntry(XMLEntry *entry1, XMLEntry *entry2)
{
	xmlEntryChildren_.push_back(entry1);
	xmlEntryChildren_.push_back(entry2);
}

void XMLEntryContainer::addChildXMLEntry(XMLEntry *entry1, XMLEntry *entry2, XMLEntry *entry3)
{
	xmlEntryChildren_.push_back(entry1);
	xmlEntryChildren_.push_back(entry2);
	xmlEntryChildren_.push_back(entry3);
}

void XMLEntryContainer::addChildXMLEntry(XMLEntry *entry1, XMLEntry *entry2, XMLEntry *entry3, XMLEntry *entry4)
{
	xmlEntryChildren_.push_back(entry1);
	xmlEntryChildren_.push_back(entry2);
	xmlEntryChildren_.push_back(entry3);
	xmlEntryChildren_.push_back(entry4);
}

void XMLEntryContainer::addChildXMLEntry(XMLEntry *entry1, XMLEntry *entry2, XMLEntry *entry3, XMLEntry *entry4, XMLEntry *entry5)
{
	xmlEntryChildren_.push_back(entry1);
	xmlEntryChildren_.push_back(entry2);
	xmlEntryChildren_.push_back(entry3);
	xmlEntryChildren_.push_back(entry4);
	xmlEntryChildren_.push_back(entry5);
}

void XMLEntryContainer::addChildXMLEntry(XMLEntry *entry1, XMLEntry *entry2, XMLEntry *entry3, XMLEntry *entry4, XMLEntry *entry5,
		XMLEntry *entry6)
{
	xmlEntryChildren_.push_back(entry1);
	xmlEntryChildren_.push_back(entry2);
	xmlEntryChildren_.push_back(entry3);
	xmlEntryChildren_.push_back(entry4);
	xmlEntryChildren_.push_back(entry5);
	xmlEntryChildren_.push_back(entry6);
}

void XMLEntryContainer::addChildXMLEntry(XMLEntry *entry1, XMLEntry *entry2, XMLEntry *entry3, XMLEntry *entry4, XMLEntry *entry5,
		XMLEntry *entry6, XMLEntry *entry7)
{
	xmlEntryChildren_.push_back(entry1);
	xmlEntryChildren_.push_back(entry2);
	xmlEntryChildren_.push_back(entry3);
	xmlEntryChildren_.push_back(entry4);
	xmlEntryChildren_.push_back(entry5);
	xmlEntryChildren_.push_back(entry6);
	xmlEntryChildren_.push_back(entry7);
}

void XMLEntryContainer::addChildXMLEntry(XMLEntry *entry1, XMLEntry *entry2, XMLEntry *entry3, XMLEntry *entry4, XMLEntry *entry5,
		XMLEntry *entry6, XMLEntry *entry7, XMLEntry *entry8)
{
	xmlEntryChildren_.push_back(entry1);
	xmlEntryChildren_.push_back(entry2);
	xmlEntryChildren_.push_back(entry3);
	xmlEntryChildren_.push_back(entry4);
	xmlEntryChildren_.push_back(entry5);
	xmlEntryChildren_.push_back(entry6);
	xmlEntryChildren_.push_back(entry7);
	xmlEntryChildren_.push_back(entry8);
}

void XMLEntryContainer::addChildXMLEntry(XMLEntry *entry1, XMLEntry *entry2, XMLEntry *entry3, XMLEntry *entry4, XMLEntry *entry5,
		XMLEntry *entry6, XMLEntry *entry7, XMLEntry *entry8, XMLEntry *entry9)
{
	xmlEntryChildren_.push_back(entry1);
	xmlEntryChildren_.push_back(entry2);
	xmlEntryChildren_.push_back(entry3);
	xmlEntryChildren_.push_back(entry4);
	xmlEntryChildren_.push_back(entry5);
	xmlEntryChildren_.push_back(entry6);
	xmlEntryChildren_.push_back(entry7);
	xmlEntryChildren_.push_back(entry8);
	xmlEntryChildren_.push_back(entry9);
}


void XMLEntryContainer::addChildXMLEntry(XMLEntry *entry1, XMLEntry *entry2, XMLEntry *entry3, XMLEntry *entry4, XMLEntry *entry5,
		XMLEntry *entry6, XMLEntry *entry7, XMLEntry *entry8, XMLEntry *entry9, XMLEntry *entry10)
{
	xmlEntryChildren_.push_back(entry1);
	xmlEntryChildren_.push_back(entry2);
	xmlEntryChildren_.push_back(entry3);
	xmlEntryChildren_.push_back(entry4);
	xmlEntryChildren_.push_back(entry5);
	xmlEntryChildren_.push_back(entry6);
	xmlEntryChildren_.push_back(entry7);
	xmlEntryChildren_.push_back(entry8);
	xmlEntryChildren_.push_back(entry9);
	xmlEntryChildren_.push_back(entry10);
}

XMLEntryContainer *XMLEntryContainer::addChildXMLEntryContainer(XMLEntryContainer *container)
{
	xmlEntryChildren_.push_back(container);
	return container;
}

bool XMLEntryContainer::readXML(XMLNode *parentNode)
{
	std::list<XMLEntry *>::iterator itor = xmlEntryChildren_.begin(), end = xmlEntryChildren_.end();
	for (;itor!=end; itor++)
	{
		if (!(*itor)->readXML(parentNode)) return false;
	}
	return true;
}

void XMLEntryContainer::writeXML(XMLNode *parentNode)
{
	std::list<XMLEntry *>::iterator itor = xmlEntryChildren_.begin(), end = xmlEntryChildren_.end();
	for (;itor!=end; itor++)
	{
		(*itor)->writeXML(parentNode);
	}
}

XMLEntryGroup::XMLEntryGroup(const std::string &name, const std::string &description) :
	xmlEntryName_(name), xmlEntryDescription_(description)
{
}

XMLEntryGroup::~XMLEntryGroup()
{
}

bool XMLEntryGroup::readXML(XMLNode *parentNode)
{
	XMLNode *node = 0;
	if (!parentNode->getNamedChild(xmlEntryName_.c_str(), node)) return false;
	
	if (!XMLEntryContainer::readXML(node)) return false;
	return node->failChildren();
}

void XMLEntryGroup::writeXML(XMLNode *parentNode)
{
	// Add the comments for this node
	parentNode->addChild(new XMLNode("", 
		S3D::formatStringBuffer("%s", xmlEntryDescription_.c_str()), 
		XMLNode::XMLCommentType));

	// Add the actual node
	XMLNode *newNode = new XMLNode(xmlEntryName_.c_str());
	parentNode->addChild(newNode);

	XMLEntryContainer::writeXML(newNode);
}
