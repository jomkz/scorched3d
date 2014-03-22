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

#if !defined(__INCLUDE_XMLEntryRooth_INCLUDE__)
#define __INCLUDE_XMLEntryRooth_INCLUDE__

#include <XML/XMLEntry.h>
#include <XML/XMLFile.h>

class XMLEntryRootI
{
public:
	XMLEntryRootI();
	virtual ~XMLEntryRootI();

	virtual const char *getRootFileName() = 0 ;
	virtual S3D::FileLocation getRootFileLocation() = 0;
	virtual const char *getRootNodeName() = 0;
};

template <class T>
class XMLEntryRoot : public T, public XMLEntryRootI
{
public:
	XMLEntryRoot(S3D::FileLocation fileLocation, const char *fileName, const char *rootNodeName,
		const char *typeName, const char *description, bool required = true) : 
		T(typeName, description, required),
		fileLocation_(fileLocation),
		fileName_(fileName), 
		rootNodeName_(rootNodeName)
	{
	}

	virtual ~XMLEntryRoot()
	{
	}

	bool loadFile(bool required, void *xmlData = 0)
	{
		std::string filePath = S3D::getLocation(fileLocation_, fileName_);
		return loadFromFile(filePath, required, xmlData);
	}

	bool loadFromFile(const std::string &filePath, bool required, void *xmlData = 0)
	{
		XMLFile file;
		if (!file.readFile(filePath, required))
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
			if (!required) return true;
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

	bool saveFile(void *xmlData = 0)
	{
		std::string filePath = S3D::getLocation(fileLocation_, fileName_);
		return saveToFile(filePath, xmlData);
	}

	bool saveToFile(const std::string &filePath, void *xmlData = 0)
	{
		XMLNode rootNode(rootNodeName_);
		writeXML(&rootNode);
	
		if (!rootNode.writeToFile(filePath)) return false;
		return true;
	}

	void checkFile()
	{
		// Check the options files are writeable
		std::string filePath = S3D::getLocation(fileLocation_, fileName_);
		FILE *checkfile = fopen(filePath.c_str(), "a");
		if (!checkfile)
		{
			S3D::dialogMessage(
				"Scorched3D", S3D::formatStringBuffer(
				"Warning: Your settings file (%s) cannot be\n"
				"written to.  Your settings will not be saved from one game to the next.\n\n"
				"To fix this problem correct the permissions for this file.",
				filePath.c_str()));
		}
		else fclose(checkfile);
	}

	virtual const char *getRootFileName() { return fileName_; }
	virtual S3D::FileLocation getRootFileLocation() { return fileLocation_; }
	virtual const char *getRootNodeName() { return rootNodeName_; }
	virtual bool isXMLEntryRoot() { return true; }

	virtual TemplateProvider *getChild(TemplateData &data, const std::string &name)
	{
		TemplateProvider *result = XMLEntry::getChild(data, name);
		if (result) return result;
		if (name == "THIS_FILE_NAME")
		{
			return TemplateProviderString::getStaticValue(getRootFileName());
		}
		if (name == "THIS_FILE_LOCATION")
		{
			return TemplateProviderString::getStaticValue(S3D::getLocationConstant(getRootFileLocation()));
		}
		if (name == "THIS_ROOT_NODE")
		{
			return TemplateProviderString::getStaticValue(getRootNodeName());
		}
		return 0;
	}
protected:
	S3D::FileLocation fileLocation_;
	const char *fileName_;
	const char *rootNodeName_;
};

#endif
