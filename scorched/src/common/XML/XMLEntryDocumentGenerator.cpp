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

#include <XML/XMLEntryDocumentGenerator.h>
#include <template/TemplateRenderer.h>

XMLEntryDocumentGenerator::XMLEntryDocumentGenerator(const std::string &directory) :
	directory_(directory)
{
}

XMLEntryDocumentGenerator::~XMLEntryDocumentGenerator()
{
}

void XMLEntryDocumentGenerator::addXMLEntryRoot(XMLEntry *root)
{
	if (!(root->getData() & XMLEntry::eDataContainer))
	{
		std::string result;
		root->getTypeName(result);
		S3D::dialogExit("XMLEntryDocumentGenerator",
			S3D::formatStringBuffer("Invalid type %s", result.c_str()));
	}

	addType(0, root, eRoot);
}

void XMLEntryDocumentGenerator::writeDocumentation()
{
	{
		std::map<std::string, XMLEntryInfo> rootTypes;
		types_.swap(rootTypes);
		std::map<std::string, XMLEntryInfo>::iterator itor = rootTypes.begin(),
			end = rootTypes.end();
		for (; itor != end; ++itor)
		{
			addEntry(0, itor->second.entry);
		}
	}
	{
		TemplateRenderer templateRenderer;
		std::map<std::string, XMLEntryInfo>::iterator itor = types_.begin(),
			end = types_.end();
		for (; itor != end; ++itor)
		{
			std::string templateFileName;
			switch(itor->second.type) 
			{
			case eContainer:
				templateFileName = "docs/XMLEntryContainer.html";
				break;
			case eChoice:
				templateFileName = "docs/XMLEntryTypeChoice.html";
				break;
			}
			if (!templateFileName.empty())
			{
				TemplateProviderLocal local(0);
				local.addLocalVariable("CONTAINER", itor->second.entry);
				TemplateProviderString scorchedVersion(S3D::ScorchedVersion);
				local.addLocalVariable("SCORCHED_VERSION", &scorchedVersion);
				std::list<TemplateProvider *> referingTypesList;
				std::set<std::string>::iterator titor = itor->second.referingTypes.begin(),
					tend = itor->second.referingTypes.end();
				for (; titor != tend; ++titor)
				{
					TemplateProviderString *referingType = new TemplateProviderString(*titor);
					data_.addTmpValue(referingType);
					referingTypesList.push_back(referingType);
				}
				TemplateProviderList referingTypes(referingTypesList);
				if (!referingTypesList.empty())
				{
					local.addLocalVariable("REFERING_TYPES", &referingTypes);
				}

				std::string outFile = S3D::formatStringBuffer("%s/%s.html", directory_.c_str(), itor->first.c_str());
				templateRenderer.renderTemplateToFile(data_, &local,
					templateFileName, outFile);
			}
		}
	}

	{
		TemplateRenderer templateRenderer;
		TemplateProviderLocal local;
		std::string outFile = S3D::formatStringBuffer("%s/%s", directory_.c_str(), "styles.css");
		templateRenderer.renderTemplateToFile(data_, &local,
			"docs/styles.css", outFile);
	}
	{
		TemplateRenderer templateRenderer;
		TemplateProviderLocal local;
		std::list<TemplateProvider *> filesList, typesList;
		std::map<std::string, XMLEntryInfo>::iterator itor = types_.begin(),
			end = types_.end();
		for (; itor != end; ++itor)
		{
			if (itor->second.entry->isXMLEntryRoot())
			{
				filesList.push_back(itor->second.entry);
			}
			typesList.push_back(itor->second.entry);
		}

		TemplateProviderList files(filesList);
		local.addLocalVariable("FILES", &files);
		TemplateProviderList types(typesList);
		local.addLocalVariable("TYPES", &types);

		std::string outFile = S3D::formatStringBuffer("%s/%s", directory_.c_str(), "index.html");
		templateRenderer.renderTemplateToFile(data_, &local,
			"docs/index.html", outFile);
	}
}

void XMLEntryDocumentGenerator::addEntry(XMLEntry *referingType, XMLEntry *entry)
{
	unsigned int data = entry->getData();
	if (data & XMLEntry::eDataList)
	{
		addList(referingType, (XMLEntryListBase *) entry);
	}
	else if (data & XMLEntry::eDataContainer)
	{
		addContainer(referingType, (XMLEntryContainer *) entry);
	}
	else if (data & XMLEntry::eDataChoice)
	{
		addChoice(referingType, (XMLEntryTypeChoiceBase *) entry);
	}
	else
	{
		addType(referingType, entry, eValue);
	}
}

void XMLEntryDocumentGenerator::addContainer(XMLEntry *referingType, XMLEntryContainer *container)
{
	if (!addType(referingType, container, eContainer)) return;

	std::map<std::string, XMLEntry *> &children = container->getChildren();
	std::map<std::string, XMLEntry *>::iterator itor = children.begin(),
		end = children.end();
	for (; itor != end; ++itor)
	{
		addEntry(container, itor->second);
	}
}

void XMLEntryDocumentGenerator::addChoice(XMLEntry *referingType, XMLEntryTypeChoiceBase *choice)
{
	if (!addType(referingType, choice, eChoice)) return;

	std::list<XMLEntry *> children;
	choice->getAllTypeInstances(data_, children);
	std::list<XMLEntry *>::iterator itor = children.begin(),
		end = children.end();
	for (; itor != end; ++itor)
	{
		addEntry(choice, *itor);
	}
}

void XMLEntryDocumentGenerator::addList(XMLEntry *referingType, XMLEntryListBase *list)
{
	std::list<XMLEntry *> children;
	list->getAllTypeInstances(data_, children);
	std::list<XMLEntry *>::iterator itor = children.begin(),
		end = children.end();
	for (; itor != end; ++itor)
	{
		addEntry(referingType, *itor);
	}
}

bool XMLEntryDocumentGenerator::addType(XMLEntry *referingType, XMLEntry *entry, XMLEntryType type)
{
	if (!(entry->getData() & XMLEntry::eDataChoice) &&
		!(entry->getData() & XMLEntry::eDataList) &&
		!(entry->getData() & XMLEntry::eDataContainer))
	{
		return false;
	}

	std::string typeName;
	entry->getTypeName(typeName);

	std::map<std::string, XMLEntryInfo>::iterator existingType = types_.find(typeName);
	if (existingType != types_.end())
	{
		if (referingType)
		{
			std::string referingTypeName;
			referingType->getTypeName(referingTypeName);
			existingType->second.referingTypes.insert(referingTypeName);
		}

		return false;
	}
	else
	{
		XMLEntryInfo info;
		info.entry = entry;
		info.type = type;
		if (referingType)
		{
			std::string referingTypeName;
			referingType->getTypeName(referingTypeName);
			info.referingTypes.insert(referingTypeName);
		}
		types_[typeName] = info;

		return true;
	}
}
