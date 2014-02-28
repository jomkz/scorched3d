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

#include <XML/XMLEntryRoot.h>
#include <XML/XMLFile.h>

XMLEntryRoot::XMLEntryRoot(S3D::FileLocation fileLocation, const char *fileName, const char *rootNodeName,
	const char *typeName, const char *description, bool required) : 
	XMLEntryContainer(typeName, description, required),
	fileLocation_(fileLocation),
	fileName_(fileName), 
	rootNodeName_(rootNodeName)
{
}

XMLEntryRoot::~XMLEntryRoot()
{
}

bool XMLEntryRoot::parseFile(void *xmlData)
{
	std::string filePath = S3D::getLocation(fileLocation_, fileName_);
	XMLFile file;
	if (!file.readFile(filePath))
	{
		S3D::dialogMessage("Scorched3D", 
			S3D::formatStringBuffer("ERROR: Failed to parse XML file \"%s\"\n"
			"%s",
			filePath.c_str(),
			file.getParserError()));		
		return false;
	}
	XMLNode *rootNode = file.getRootNode();
	if (!rootNode) 
	{
		S3D::dialogMessage("Scorched3D", 
			S3D::formatStringBuffer("ERROR: Empty XML file \"%s\"",
			filePath.c_str()));		
		return false;
	}
	if (strcmp(rootNodeName_, rootNode->getName()))
	{
		S3D::dialogMessage("Scorched3D", 
			S3D::formatStringBuffer("ERROR: XML file \"%s\" has invalid root node name, expected %s",
			filePath.c_str(),
			rootNodeName_));		
		return false;
	}

	if (!readXML(rootNode, xmlData)) return false;
	return true;
}

XMLEntryDocumentInfo XMLEntryRoot::generateDocumentation(XMLEntryDocumentGenerator &generator)
{
	XMLEntryDocumentInfo info;
	if (generator.hasType(xmlTypeName_)) return info;
	generator.addRootTypeTags(this, xmlEntryChildrenList_, xmlTypeName_, "docs/XMLEntryContainer.html");
	return info;
}
