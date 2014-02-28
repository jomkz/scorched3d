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
#include <common/FileTemplate.h>

XMLEntryDocumentInfo::XMLEntryDocumentInfo()
{
}

XMLEntryDocumentInfo::~XMLEntryDocumentInfo()
{
}

XMLEntryDocumentGenerator::XMLEntryDocumentGenerator(const std::string &documentLocation) :
	documentLocation_(documentLocation)
{
}

XMLEntryDocumentGenerator::~XMLEntryDocumentGenerator()
{
}

void XMLEntryDocumentGenerator::addType(const std::string &typeName, const std::string &fileName, FileTemplateVariables *variables)
{
	DIALOG_ASSERT(types_.find(typeName) == types_.end());
	types_.insert(typeName);

	std::string fileLocation = documentLocation_ + "/" + typeName + ".html";
	FileTemplate::writeTemplateToFile(fileName, *variables, fileLocation);
	delete variables;
}

bool XMLEntryDocumentGenerator::hasType(const std::string &typeName)
{
	return types_.find(typeName) != types_.end();
}

XMLEntry::XMLEntry()
{
}

XMLEntry::~XMLEntry()
{
}

XMLEntryContainer::XMLEntryContainer(const char *typeName, const char *description) :
	XMLEntry(),
	xmlTypeName_(typeName), xmlDescription_(description)
{
}

XMLEntryContainer::~XMLEntryContainer()
{
}

void XMLEntryContainer::addChildXMLEntry(const char *name1, XMLEntry *entry1)
{
	xmlEntryChildren_[name1] = entry1;
}

void XMLEntryContainer::addChildXMLEntry(const char *name1, XMLEntry *entry1, const char *name2, XMLEntry *entry2)
{
	xmlEntryChildren_[name1] = entry1;
	xmlEntryChildren_[name2] = entry2;
}

void XMLEntryContainer::addChildXMLEntry(const char *name1, XMLEntry *entry1, const char *name2, XMLEntry *entry2, 
		const char *name3, XMLEntry *entry3)
{
	xmlEntryChildren_[name1] = entry1;
	xmlEntryChildren_[name2] = entry2;
	xmlEntryChildren_[name3] = entry3;
}

void XMLEntryContainer::addChildXMLEntry(const char *name1, XMLEntry *entry1, const char *name2, XMLEntry *entry2, 
		const char *name3, XMLEntry *entry3, const char *name4, XMLEntry *entry4)
{
	xmlEntryChildren_[name1] = entry1;
	xmlEntryChildren_[name2] = entry2;
	xmlEntryChildren_[name3] = entry3;
	xmlEntryChildren_[name4] = entry4;
}

void XMLEntryContainer::addChildXMLEntry(const char *name1, XMLEntry *entry1, const char *name2, XMLEntry *entry2, 
		const char *name3, XMLEntry *entry3, const char *name4, XMLEntry *entry4, const char *name5, XMLEntry *entry5)
{
	xmlEntryChildren_[name1] = entry1;
	xmlEntryChildren_[name2] = entry2;
	xmlEntryChildren_[name3] = entry3;
	xmlEntryChildren_[name4] = entry4;
	xmlEntryChildren_[name5] = entry5;
}

void XMLEntryContainer::addChildXMLEntry(const char *name1, XMLEntry *entry1, const char *name2, XMLEntry *entry2, 
		const char *name3, XMLEntry *entry3, const char *name4, XMLEntry *entry4, const char *name5, XMLEntry *entry5, 
		const char *name6, XMLEntry *entry6)
{
	xmlEntryChildren_[name1] = entry1;
	xmlEntryChildren_[name2] = entry2;
	xmlEntryChildren_[name3] = entry3;
	xmlEntryChildren_[name4] = entry4;
	xmlEntryChildren_[name5] = entry5;
	xmlEntryChildren_[name6] = entry6;
}

void XMLEntryContainer::addChildXMLEntry(const char *name1, XMLEntry *entry1, const char *name2, XMLEntry *entry2, 
		const char *name3, XMLEntry *entry3, const char *name4, XMLEntry *entry4, const char *name5, XMLEntry *entry5, 
		const char *name6, XMLEntry *entry6, const char *name7, XMLEntry *entry7)
{
	xmlEntryChildren_[name1] = entry1;
	xmlEntryChildren_[name2] = entry2;
	xmlEntryChildren_[name3] = entry3;
	xmlEntryChildren_[name4] = entry4;
	xmlEntryChildren_[name5] = entry5;
	xmlEntryChildren_[name6] = entry6;
	xmlEntryChildren_[name7] = entry7;
}

void XMLEntryContainer::addChildXMLEntry(const char *name1, XMLEntry *entry1, const char *name2, XMLEntry *entry2, 
		const char *name3, XMLEntry *entry3, const char *name4, XMLEntry *entry4, const char *name5, XMLEntry *entry5, 
		const char *name6, XMLEntry *entry6, const char *name7, XMLEntry *entry7, const char *name8, XMLEntry *entry8)
{
	xmlEntryChildren_[name1] = entry1;
	xmlEntryChildren_[name2] = entry2;
	xmlEntryChildren_[name3] = entry3;
	xmlEntryChildren_[name4] = entry4;
	xmlEntryChildren_[name5] = entry5;
	xmlEntryChildren_[name6] = entry6;
	xmlEntryChildren_[name7] = entry7;
	xmlEntryChildren_[name8] = entry8;
}

void XMLEntryContainer::addChildXMLEntry(const char *name1, XMLEntry *entry1, const char *name2, XMLEntry *entry2, 
		const char *name3, XMLEntry *entry3, const char *name4, XMLEntry *entry4, const char *name5, XMLEntry *entry5, 
		const char *name6, XMLEntry *entry6, const char *name7, XMLEntry *entry7, const char *name8, XMLEntry *entry8, 
		const char *name9, XMLEntry *entry9)
{
	xmlEntryChildren_[name1] = entry1;
	xmlEntryChildren_[name2] = entry2;
	xmlEntryChildren_[name3] = entry3;
	xmlEntryChildren_[name4] = entry4;
	xmlEntryChildren_[name5] = entry5;
	xmlEntryChildren_[name6] = entry6;
	xmlEntryChildren_[name7] = entry7;
	xmlEntryChildren_[name8] = entry8;
	xmlEntryChildren_[name9] = entry9;
}

void XMLEntryContainer::addChildXMLEntry(const char *name1, XMLEntry *entry1, const char *name2, XMLEntry *entry2, 
		const char *name3, XMLEntry *entry3, const char *name4, XMLEntry *entry4, const char *name5, XMLEntry *entry5, 
		const char *name6, XMLEntry *entry6, const char *name7, XMLEntry *entry7, const char *name8, XMLEntry *entry8, 
		const char *name9, XMLEntry *entry9, const char *name10, XMLEntry *entry10)
{
	xmlEntryChildren_[name1] = entry1;
	xmlEntryChildren_[name2] = entry2;
	xmlEntryChildren_[name3] = entry3;
	xmlEntryChildren_[name4] = entry4;
	xmlEntryChildren_[name5] = entry5;
	xmlEntryChildren_[name6] = entry6;
	xmlEntryChildren_[name7] = entry7;
	xmlEntryChildren_[name8] = entry8;
	xmlEntryChildren_[name9] = entry9;
	xmlEntryChildren_[name10] = entry10;
}

XMLEntryContainer *XMLEntryContainer::addChildXMLEntryContainer(const char *name, XMLEntryContainer *container)
{
	xmlEntryChildren_[name] = container;
	return container;
}

bool XMLEntryContainer::readXML(XMLNode *node)
{
	std::set<std::string> seenTags;
	{
		std::list<XMLNode *>::iterator itor = node->getChildren().begin(), end = node->getChildren().end();
		for (;itor!=end;++itor)
		{
			std::map<std::string, XMLEntry *>::iterator findItor =
				xmlEntryChildren_.find((*itor)->getName());
			if (findItor == xmlEntryChildren_.end()) return (*itor)->returnError("Unrecognised XML element");
			if (!findItor->second->readXML(*itor)) return (*itor)->returnError("Failed to parse XML element");
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

		// Add the actual node
		XMLNode *newNode = new XMLNode(itor->first.c_str());
		parentNode->addChild(newNode);
		itor->second->writeXML(newNode);
	}
}

XMLEntryDocumentInfo XMLEntryContainer::generateDocumentation(XMLEntryDocumentGenerator &generator)
{
	XMLEntryDocumentInfo info;
	if (generator.hasType(xmlTypeName_)) return info;

	FileTemplateVariables *mainVariables = new FileTemplateVariables();
	mainVariables->addVariableValue("TYPE_NAME", xmlTypeName_);
	std::map<std::string, XMLEntry *>::iterator itor = xmlEntryChildren_.begin(), end = xmlEntryChildren_.end();
	for (;itor!=end; itor++)
	{
		XMLEntryDocumentInfo childInfo = itor->second->generateDocumentation(generator);
		FileTemplateVariables *childVariables = mainVariables->addLoopVariable("CHILD");
		std::string childTypeName, childTypeNameConverted, childDescription;
		itor->second->getTypeName(childTypeName);
		itor->second->getDescription(childDescription);
		XMLNode::removeSpecialChars(childTypeName, childTypeNameConverted);
		bool required = (itor->second->getData() & eDataRequired) != 0;

		childVariables->addVariableValue("CHILD_TAGNAME", itor->first.c_str());
		childVariables->addVariableValue("CHILD_TYPE", childTypeNameConverted.c_str());
		childVariables->addVariableValue("CHILD_DESCRIPTION", childDescription.c_str());
		childVariables->addVariableValue("CHILD_OPTIONAL", required?"false":"true");
	}
	generator.addType(xmlTypeName_, "docs/XMLEntryContainer.html", mainVariables);
	return info;
}
