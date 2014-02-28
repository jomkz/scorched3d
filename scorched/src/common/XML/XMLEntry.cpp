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

void XMLEntryContainer::addChildXMLEntry(XMLEntry *entry)
{
	xmlEntryChildren_.push_back(entry);
}

bool XMLEntryContainer::readXML(XMLNode *parentNode)
{
	std::list<XMLEntry *>::iterator itor, end = xmlEntryChildren_.end();
	for (;itor!=end; itor++)
	{
		if (!(*itor)->readXML(parentNode)) return false;
	}
	return true;
}

void XMLEntryContainer::writeXML(XMLNode *parentNode)
{
	std::list<XMLEntry *>::iterator itor, end = xmlEntryChildren_.end();
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
