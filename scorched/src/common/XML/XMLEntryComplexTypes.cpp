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

#include <XML/XMLEntryComplexTypes.h>

XMLEntryModelID::XMLEntryModelID(bool required) :
	XMLEntryContainer("ModelID", "A reference to an ogre mesh", required),
	meshName("The ogre mesh resource name, this mesh must already be loaded in the ogre resources"),
	scale("The scale of the mesh, a scale of 1.0 is an unchanged scale", 0, fixed(1)),
	rotation("The rotation of the mesh around the blah axis TODO", 0, fixed(1)),
	brightness("The brightness of the mesh blah", 0, fixed(1))
{
	addChildXMLEntry("meshname", &meshName);
	addChildXMLEntry("scale", &scale);
	addChildXMLEntry("rotation", &rotation);
	addChildXMLEntry("brightness", &brightness);
}

XMLEntryModelID::~XMLEntryModelID()
{
}

XMLEntryParticleID::XMLEntryParticleID(bool required) :
	XMLEntryContainer("ParticleID", "A reference to an ogre particle definition", required),
	particleName("The ogre particle script name, this script  must already be loaded in the ogre resources")
{
	addChildXMLEntry("particlename", &particleName);
}

XMLEntryParticleID::~XMLEntryParticleID()
{
}

XMLEntryNumberParser::XMLEntryNumberParser(const char *parserName, const char *description) :
	description_(description), data_(XMLEntrySimpleType::eDataRequired),
	value_(parserName)
{

}

XMLEntryNumberParser::XMLEntryNumberParser(const char *parserName, const char *description, 
	unsigned int data,
	const std::string &value) :
	description_(description), data_(data),
	value_(parserName, value.c_str())
{
	
}

XMLEntryNumberParser::~XMLEntryNumberParser()
{

}

fixed XMLEntryNumberParser::getValue(ScorchedContext &context)
{
	return value_.getValue(context);
}


bool XMLEntryNumberParser::readXML(XMLNode *node, void *xmlData)
{
	if (!value_.setExpression(node->getContent()))
	{
		return node->returnError(S3D::formatStringBuffer(
			"Failed to set XMLEntryNumberParser with \"%s\"",
			node->getContent()));
	}
	return true;
}

void XMLEntryNumberParser::writeXML(XMLNode *node)
{
	node->setContent(value_.getExpression());
}

void XMLEntryNumberParser::getTypeName(std::string &result)
{
	result = "number expression";
}

void XMLEntryNumberParser::getDescription(std::string &result)
{
	result = description_;
}
