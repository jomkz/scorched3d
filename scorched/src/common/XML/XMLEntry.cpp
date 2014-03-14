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
#include <XML/XMLEntryRoot.h>

XMLEntry::XMLEntry()
{
}

XMLEntry::~XMLEntry()
{
}

std::string XMLEntry::getDataDescription(unsigned int data)
{
	std::string result = "";
	if (!(data & eDataRequired)) result.append(" optional");
	if (data & eDataDepricated) result.append(" depricated");
	if (data & eDataList) result.append(" list");
	if (data & eDataChoice) result.append(" choice");
	if (data & eDataContainer) result.append(" container");
	if (!(data & eDataContainer) && !(data & eDataChoice) && !(data & eDataList)) result.append(" primitive");
	if (result.length() > 0 && result[0] == ' ') result = std::string(result, 1);
	return result;
}

TemplateProvider *XMLEntry::getChild(TemplateData &data, const std::string &name)
{
	if (name == "THIS_TYPE_NAME")
	{
		std::string result;
		getTypeName(result);
		return TemplateProviderString::getStaticValue(result);
	}
	if (name == "THIS_DESCRIPTION")
	{
		std::string result;
		getDescription(result);
		return TemplateProviderString::getStaticValue(result);
	}
	if (name == "THIS_DATA")
	{
		std::string result = getDataDescription(getData());
		if (result.empty()) return 0;
		return TemplateProviderString::getStaticValue(result);
	}
	return 0;
}

XMLEntryContainer::XMLEntryContainer(const char *typeName, const char *description, bool required) :
	XMLEntry(),
	xmlTypeName_(typeName), xmlDescription_(description),
	required_(required)
{
}

XMLEntryContainer::~XMLEntryContainer()
{
}

void XMLEntryContainer::addChildXMLEntry(const char *name1, XMLEntry *entry1)
{
	xmlEntryChildrenList_.push_back(std::pair<std::string, XMLEntry *>(name1, entry1));
	xmlEntryChildren_[name1] = entry1;
}

void XMLEntryContainer::addChildXMLEntry(const char *name1, XMLEntry *entry1, const char *name2, XMLEntry *entry2)
{
	addChildXMLEntry(name1, entry1);
	addChildXMLEntry(name2, entry2);
}

void XMLEntryContainer::addChildXMLEntry(const char *name1, XMLEntry *entry1, const char *name2, XMLEntry *entry2, 
		const char *name3, XMLEntry *entry3)
{
	addChildXMLEntry(name1, entry1);
	addChildXMLEntry(name2, entry2);
	addChildXMLEntry(name3, entry3);
}

void XMLEntryContainer::addChildXMLEntry(const char *name1, XMLEntry *entry1, const char *name2, XMLEntry *entry2, 
		const char *name3, XMLEntry *entry3, const char *name4, XMLEntry *entry4)
{
	addChildXMLEntry(name1, entry1);
	addChildXMLEntry(name2, entry2);
	addChildXMLEntry(name3, entry3);
	addChildXMLEntry(name4, entry4);
}

void XMLEntryContainer::addChildXMLEntry(const char *name1, XMLEntry *entry1, const char *name2, XMLEntry *entry2, 
		const char *name3, XMLEntry *entry3, const char *name4, XMLEntry *entry4, const char *name5, XMLEntry *entry5)
{
	addChildXMLEntry(name1, entry1);
	addChildXMLEntry(name2, entry2);
	addChildXMLEntry(name3, entry3);
	addChildXMLEntry(name4, entry4);
	addChildXMLEntry(name5, entry5);
}

void XMLEntryContainer::addChildXMLEntry(const char *name1, XMLEntry *entry1, const char *name2, XMLEntry *entry2, 
		const char *name3, XMLEntry *entry3, const char *name4, XMLEntry *entry4, const char *name5, XMLEntry *entry5, 
		const char *name6, XMLEntry *entry6)
{
	addChildXMLEntry(name1, entry1);
	addChildXMLEntry(name2, entry2);
	addChildXMLEntry(name3, entry3);
	addChildXMLEntry(name4, entry4);
	addChildXMLEntry(name5, entry5);
	addChildXMLEntry(name6, entry6);
}

void XMLEntryContainer::addChildXMLEntry(const char *name1, XMLEntry *entry1, const char *name2, XMLEntry *entry2, 
		const char *name3, XMLEntry *entry3, const char *name4, XMLEntry *entry4, const char *name5, XMLEntry *entry5, 
		const char *name6, XMLEntry *entry6, const char *name7, XMLEntry *entry7)
{
	addChildXMLEntry(name1, entry1);
	addChildXMLEntry(name2, entry2);
	addChildXMLEntry(name3, entry3);
	addChildXMLEntry(name4, entry4);
	addChildXMLEntry(name5, entry5);
	addChildXMLEntry(name6, entry6);
	addChildXMLEntry(name7, entry7);
}

void XMLEntryContainer::addChildXMLEntry(const char *name1, XMLEntry *entry1, const char *name2, XMLEntry *entry2, 
		const char *name3, XMLEntry *entry3, const char *name4, XMLEntry *entry4, const char *name5, XMLEntry *entry5, 
		const char *name6, XMLEntry *entry6, const char *name7, XMLEntry *entry7, const char *name8, XMLEntry *entry8)
{
	addChildXMLEntry(name1, entry1);
	addChildXMLEntry(name2, entry2);
	addChildXMLEntry(name3, entry3);
	addChildXMLEntry(name4, entry4);
	addChildXMLEntry(name5, entry5);
	addChildXMLEntry(name6, entry6);
	addChildXMLEntry(name7, entry7);
	addChildXMLEntry(name8, entry8);
}

void XMLEntryContainer::addChildXMLEntry(const char *name1, XMLEntry *entry1, const char *name2, XMLEntry *entry2, 
		const char *name3, XMLEntry *entry3, const char *name4, XMLEntry *entry4, const char *name5, XMLEntry *entry5, 
		const char *name6, XMLEntry *entry6, const char *name7, XMLEntry *entry7, const char *name8, XMLEntry *entry8, 
		const char *name9, XMLEntry *entry9)
{
	addChildXMLEntry(name1, entry1);
	addChildXMLEntry(name2, entry2);
	addChildXMLEntry(name3, entry3);
	addChildXMLEntry(name4, entry4);
	addChildXMLEntry(name5, entry5);
	addChildXMLEntry(name6, entry6);
	addChildXMLEntry(name7, entry7);
	addChildXMLEntry(name8, entry8);
	addChildXMLEntry(name9, entry9);
}

void XMLEntryContainer::addChildXMLEntry(const char *name1, XMLEntry *entry1, const char *name2, XMLEntry *entry2, 
		const char *name3, XMLEntry *entry3, const char *name4, XMLEntry *entry4, const char *name5, XMLEntry *entry5, 
		const char *name6, XMLEntry *entry6, const char *name7, XMLEntry *entry7, const char *name8, XMLEntry *entry8, 
		const char *name9, XMLEntry *entry9, const char *name10, XMLEntry *entry10)
{
	addChildXMLEntry(name1, entry1);
	addChildXMLEntry(name2, entry2);
	addChildXMLEntry(name3, entry3);
	addChildXMLEntry(name4, entry4);
	addChildXMLEntry(name5, entry5);
	addChildXMLEntry(name6, entry6);
	addChildXMLEntry(name7, entry7);
	addChildXMLEntry(name8, entry8);
	addChildXMLEntry(name9, entry9);
	addChildXMLEntry(name10, entry10);
}

XMLEntryContainer *XMLEntryContainer::addChildXMLEntryContainer(const char *name, XMLEntryContainer *container)
{
	addChildXMLEntry(name, container);
	return container;
}

bool XMLEntryContainer::readXML(XMLNode *node, void *xmlData)
{
	std::set<std::string> seenTags;
	{
		std::list<XMLNode *>::iterator itor = node->getChildren().begin(), end = node->getChildren().end();
		for (;itor!=end;++itor)
		{
			std::map<std::string, XMLEntry *>::iterator findItor =
				xmlEntryChildren_.find((*itor)->getName());
			if (findItor == xmlEntryChildren_.end()) return (*itor)->returnError("Unrecognised XML element");
			if (!readXMLEntry(*itor, xmlData, (*itor)->getName(), findItor->second)) return (*itor)->returnError("Failed to parse XML element");
			seenTags.insert((*itor)->getName());
		}
	}
	{
		std::map<std::string, XMLEntry *>::iterator itor = xmlEntryChildren_.begin(), end = xmlEntryChildren_.end();
		for (;itor!=end;++itor)
		{
			if (seenTags.find(itor->first) == seenTags.end() &&
				itor->second->getData() & eDataRequired)
			{
				return node->returnError(S3D::formatStringBuffer(
					"Failed to find required child XML element \"%s\"", itor->first.c_str()));
			}
		}
	}

	return true;
}

bool XMLEntryContainer::readXMLEntry(XMLNode *node, void *xmlData, const char *name, XMLEntry *entry)
{
	return entry->readXML(node, xmlData);
}

void XMLEntryContainer::writeXML(XMLNode *parentNode)
{
	std::map<std::string, XMLEntry *>::iterator itor = xmlEntryChildren_.begin(), end = xmlEntryChildren_.end();
	for (;itor!=end;++itor)
	{
		if (itor->second->getData() & eDataDepricated) continue;

		// Add the comments for this node
		std::string description;
		itor->second->getDescription(description);
		parentNode->addChild(new XMLNode("", 
			S3D::formatStringBuffer("%s", description.c_str()), 
			XMLNode::XMLCommentType));

		if (itor->second->getData() & eDataList)
		{
			XMLEntryListBase *list = (XMLEntryListBase*) itor->second;
			std::list<XMLEntry *> children;
			list->getListChildren(children);
			std::list<XMLEntry *>::iterator listItor = children.begin(), endList = children.end();
			for (;listItor != endList; ++listItor)
			{
				// Add the actual node
				XMLNode *newNode = new XMLNode(itor->first.c_str());
				parentNode->addChild(newNode);
				list->writeListXML(newNode, (*listItor));
			}
		}
		else
		{
			// Add the actual node
			XMLNode *newNode = new XMLNode(itor->first.c_str());
			parentNode->addChild(newNode);
			itor->second->writeXML(newNode);
		}
	}
}

void XMLEntryContainer::getStringProperty(TemplateData &data, std::string &result)
{
	getTypeName(result);
}

void XMLEntryContainer::getListProperty(TemplateData &data, std::list<TemplateProvider *> &result)
{
	std::list<std::pair<std::string, XMLEntry *> >::iterator itor = xmlEntryChildrenList_.begin(),
		end = xmlEntryChildrenList_.end();
	for (; itor != end; ++itor)
	{
		TemplateProviderString *value = new TemplateProviderString(itor->first);
		data.addTmpValue(value);
		TemplateProviderLocal *local = new TemplateProviderLocal(itor->second);
		local->addLocalVariable("THIS_NAME", value);
		data.addTmpValue(local);

		result.push_back(local);
	}
}

TemplateProvider *XMLEntryContainer::getChild(TemplateData &data, const std::string &name)
{
	TemplateProvider *result = XMLEntry::getChild(data, name);
	if (result) return result;
	std::map<std::string, XMLEntry *>::iterator itor = xmlEntryChildren_.find(name);
	if (itor != xmlEntryChildren_.end())
	{
		return itor->second;
	}
	return 0;
}
