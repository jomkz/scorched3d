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

void XMLEntryDocumentGenerator::getTypeReference(const std::string &referingType, const std::string &typeName, std::string &resultType)
{
	if (typeName == "number" || typeName == "string" || typeName == "boolean" || typeName =="vector")
	{
		resultType = typeName;
		return;
	}

	std::string fileLocation = documentLocation_ + "/" + typeName + ".html";
	resultType.append("<a href='").append(fileLocation).append("'>").append(typeName).append("</a>");

	std::map<std::string, TypeEntry>::iterator itor = types_.find(typeName);
	if (itor != types_.end())
	{
		itor->second.typeReferences.push_back(referingType);
	}
	else
	{
		TypeEntry entry;
		entry.typeReferences.push_back(referingType);
		types_[typeName] = entry;
	}
}

void XMLEntryDocumentGenerator::addType(const std::string &typeName, const std::string &fileName, FileTemplateVariables *variables)
{
	DIALOG_ASSERT(!hasType(typeName));

	std::map<std::string, TypeEntry>::iterator itor = types_.find(typeName);
	if (itor != types_.end())
	{
		itor->second.fileName = fileName;
		itor->second.variables = variables;
	}
	else
	{
		TypeEntry entry;
		entry.fileName = fileName;
		entry.variables = variables;
		types_[typeName] = entry;
	}
}

bool XMLEntryDocumentGenerator::hasType(const std::string &typeName)
{
	std::map<std::string, TypeEntry>::iterator itor = types_.find(typeName);
	if (itor == types_.end()) return false;
	return itor->second.variables != 0;
}

void XMLEntryDocumentGenerator::writeDocumentation()
{
	std::map<std::string, TypeEntry>::iterator itor = types_.begin(),
		end = types_.end();
	for (;itor!=end;++itor)
	{
		if (itor->second.variables)
		{
			if (!itor->second.typeReferences.empty())
			{
				std::list<std::string>::iterator refItor = itor->second.typeReferences.begin(),
					refEnd = itor->second.typeReferences.end();
				for (;refItor!=refEnd;++refItor)
				{
					FileTemplateVariables *references = itor->second.variables->addLoopVariable("REFERENCES");
					references->addVariableValue("REFERENCE_NAME", refItor->c_str());
				}
			}

			std::string fileLocation = documentLocation_ + "/" + itor->first + ".html";
			FileTemplate::writeTemplateToFile(itor->second.fileName, *itor->second.variables, fileLocation);
			delete itor->second.variables;
		}
	}
}

FileTemplateVariables *XMLEntryDocumentGenerator::addTypeTags(XMLEntry *coreType,
	std::list<std::pair<std::string, XMLEntry *> > &children)
{
	std::string typeType, typeTypeConverted,
		typeDescription, typeDescriptionConverted;
	coreType->getTypeName(typeType);
	coreType->getDescription(typeDescription);
	XMLNode::removeSpecialChars(typeDescription, typeDescriptionConverted);
		
	FileTemplateVariables *mainVariables = new FileTemplateVariables();
	mainVariables->addVariableValue("TYPE_NAME", typeType.c_str());
	mainVariables->addVariableValue("TYPE_DESCRIPTION", typeDescriptionConverted.c_str());

	std::list<std::pair<std::string, XMLEntry *> >::iterator itor = children.begin(), end = children.end();
	for (;itor!=end; itor++)
	{
		XMLEntryDocumentInfo childInfo = itor->second->generateDocumentation(*this);
		FileTemplateVariables *childVariables = mainVariables->addLoopVariable("CHILD");
		std::string childTypeName, childTypeNameConverted, childDescription;
		itor->second->getTypeName(childTypeName);
		itor->second->getDescription(childDescription);
		getTypeReference(typeType, childTypeName, childTypeNameConverted);
		std::string childOptions;
		childOptions.append((itor->second->getData() & XMLEntry::eDataRequired) != 0?"":"Optional, ");
		if ((itor->second->getData() & XMLEntry::eDataList) != 0)
		{
			childOptions.append("List, ");
		}
		else
		{
			childOptions.append((itor->second->getData() & XMLEntry::eDataChoice) != 0?"Choice, ":"");
		}
		if (!childOptions.empty()) childOptions = childOptions.substr(0, childOptions.length()-2);

		childVariables->addVariableValue("CHILD_TAGNAME", itor->first.c_str());
		childVariables->addVariableValue("CHILD_TYPE", childTypeNameConverted.c_str());
		childVariables->addVariableValue("CHILD_DESCRIPTION", childDescription.c_str());
		if (!childOptions.empty()) childVariables->addVariableValue("CHILD_OPTIONS", childOptions.c_str());

		FileTemplateVariables *tagVariables = mainVariables->addLoopVariable("TAG");
		tagVariables->addVariableValue("TAG_NAME", itor->first.c_str());
		tagVariables->addVariableValue("TAG_TYPE", childTypeNameConverted.c_str());
		if ((itor->second->getData() & XMLEntry::eDataChoice) != 0)
		{
			tagVariables->addVariableValue("TAG_CHOICE", "TRUE");
		}
		if (!childOptions.empty()) tagVariables->addVariableValue("TAG_OPTIONS", childOptions.c_str());
	}
	return mainVariables;
}

XMLEntry::XMLEntry()
{
}

XMLEntry::~XMLEntry()
{
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
	FileTemplateVariables *mainVariables = generator.addTypeTags(this, xmlEntryChildrenList_);
	generator.addType(xmlTypeName_, "docs/XMLEntryContainer.html", mainVariables);
	return info;
}
