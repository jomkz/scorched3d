////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
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
//    You should have received a copy of the GNU General Public License
//    along with Scorched3D; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

#include <landscapedef/LandscapeDefn.h>
#include <landscapedef/LandscapeDefinitions.h>
#include <common/Defines.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

static LandscapeDefnType *fetchTankStartDefnType(const char *type)
{
	if (0 == strcmp(type, "height")) return new LandscapeDefnStartHeight;
	S3D::dialogMessage("LandscapeDefnType", S3D::formatStringBuffer("Unknown tankstart type %s", type));
	return 0;
}

static LandscapeDefnType *fetchHeightMapDefnType(const char *type)
{
	if (0 == strcmp(type, "generate")) return new LandscapeDefnHeightMapGenerate;
	if (0 == strcmp(type, "file")) return new LandscapeDefnHeightMapFile;
	S3D::dialogMessage("LandscapeDefnType", S3D::formatStringBuffer("Unknown heightmap type %s", type));
	return 0;
}

static LandscapeDefnType *fetchRoofMapDefnType(const char *type)
{
	if (0 == strcmp(type, "sky")) return new LandscapeDefnTypeNone;
	if (0 == strcmp(type, "cavern")) return new LandscapeDefnRoofCavern;
	S3D::dialogMessage("LandscapeDefnType", S3D::formatStringBuffer("Unknown roof type %s", type));
	return 0;
}

static LandscapeDefnType *fetchSurroundDefnType(const char *type)
{
	if (0 == strcmp(type, "none")) return new LandscapeDefnTypeNone;
	if (0 == strcmp(type, "generate")) return new LandscapeDefnHeightMapGenerate;
	if (0 == strcmp(type, "file")) return new LandscapeDefnHeightMapFile;
	S3D::dialogMessage("LandscapeDefnType", S3D::formatStringBuffer("Unknown surround type %s", type));
	return 0;
}

static bool parseMinMax(XMLNode *parent, const char *name, 
	fixed &min, fixed &max)
{
	XMLNode *node = 0;
	if (!parent->getNamedChild(name, node)) return false;
	if (!node->getNamedChild("max", max)) return false;
	if (!node->getNamedChild("min", min)) return false;
	return node->failChildren();
}

static bool parseMinMaxInt(XMLNode *parent, const char *name, 
	int &min, int &max)
{
	XMLNode *node = 0;
	if (!parent->getNamedChild(name, node)) return false;
	if (!node->getNamedChild("max", max)) return false;
	if (!node->getNamedChild("min", min)) return false;
	return node->failChildren();
}

bool LandscapeDefnTypeNone::readXML(XMLNode *node)
{
	return node->failChildren();
}

LandscapeDefnRoofCavern::LandscapeDefnRoofCavern() : heightmap(0)
{
}

LandscapeDefnRoofCavern::~LandscapeDefnRoofCavern()
{
	delete heightmap;
}

bool LandscapeDefnRoofCavern::readXML(XMLNode *node)
{
	if (!node->getNamedChild("width", width)) return false;
	if (!node->getNamedChild("height", height)) return false;
	{
		XMLNode *heightNode;
		std::string heightmaptype;
		if (!node->getNamedChild("heightmap", heightNode)) return false;
		if (!heightNode->getNamedParameter("type", heightmaptype)) return false;
		if (!(heightmap = fetchHeightMapDefnType(heightmaptype.c_str()))) return false;
		if (!heightmap->readXML(heightNode)) return false;
	}	
	return node->failChildren();
}

bool LandscapeDefnStartHeight::readXML(XMLNode *node)
{
	if (!node->getNamedChild("startcloseness", startcloseness)) return false;
	if (!parseMinMax(node, "height", heightmin, heightmax)) return false;
	if (!node->getNamedChild("startmask", startmask)) return false;
	if (!node->getNamedChild("flatness", flatness, false))
	{
		flatness = 0;
	}
	if (!startmask.empty())
	{
		if (!S3D::checkDataFile(startmask.c_str())) return false;
	}
	return node->failChildren();
}

bool LandscapeDefnHeightMapFile::readXML(XMLNode *node)
{
	if (!node->getNamedChild("file", file)) return false;
	if (!node->getNamedChild("levelsurround", levelsurround)) return false;
	if (!S3D::checkDataFile(file.c_str())) return false;
	return node->failChildren();
}

bool LandscapeDefnHeightMapGenerate::readXML(XMLNode *node)
{
	if (!node->getNamedChild("mask", mask)) return false;
	if (!parseMinMaxInt(node, "landhills", 
		landhillsmin, landhillsmax)) return false;
	if (!parseMinMax(node, "landheight", 
		landheightmin, landheightmax)) return false;
	if (!parseMinMax(node, "landpeakwidthx", 
		landpeakwidthxmin, landpeakwidthxmax)) return false;
	if (!parseMinMax(node, "landpeakwidthy", 
		landpeakwidthymin, landpeakwidthymax)) return false;
	if (!parseMinMax(node, "landpeakheight", 
		landpeakheightmin, landpeakheightmax)) return false;
	if (!node->getNamedChild("landsmoothing", landsmoothing)) return false;
	if (!node->getNamedChild("levelsurround", levelsurround)) return false;

	if (!mask.empty())
	{
		if (!S3D::checkDataFile(mask.c_str())) return false;
	}
	return node->failChildren();
}

LandscapeDefn::LandscapeDefn() :
	heightmap(0), tankstart(0), surround(0), roof(0)
{
}

LandscapeDefn::~LandscapeDefn()
{
}

bool LandscapeDefn::readXML(LandscapeDefinitions *definitions, XMLNode *node)
{
	if (!node->getNamedChild("minplayers", minplayers)) return false;
	if (!node->getNamedChild("maxplayers", maxplayers)) return false;
	if (!node->getNamedChild("landscapewidth", landscapewidth)) return false;
	if (!node->getNamedChild("landscapeheight", landscapeheight)) return false;

	{
		XMLNode *startNode;
		std::string tankstarttype;
		if (!node->getNamedChild("tankstart", startNode)) return false;
		if (!startNode->getNamedParameter("type", tankstarttype)) return false;
		if (!(tankstart = fetchTankStartDefnType(tankstarttype.c_str()))) return false;
		if (!tankstart->readXML(startNode)) return false;
	}
	{
		XMLNode *heightNode;
		std::string heightmaptype;
		if (!node->getNamedChild("heightmap", heightNode)) return false;
		if (!heightNode->getNamedParameter("type", heightmaptype)) return false;
		if (!(heightmap = fetchHeightMapDefnType(heightmaptype.c_str()))) return false;
		if (!heightmap->readXML(heightNode)) return false;
	}
	{
		XMLNode *surroundNode;
		std::string surroundtype;
		if (!node->getNamedChild("surround", surroundNode)) return false;
		if (!surroundNode->getNamedParameter("type", surroundtype)) return false;
		if (!(surround = fetchSurroundDefnType(surroundtype.c_str()))) return false;
		if (!surround->readXML(surroundNode)) return false;
	}
	{
		XMLNode *roofNode;
		std::string rooftype;
		if (!node->getNamedChild("roof", roofNode)) return false;
		if (!roofNode->getNamedParameter("type", rooftype)) return false;
		if (!(roof = fetchRoofMapDefnType(rooftype.c_str()))) return false;
		if (!roof->readXML(roofNode)) return false;
	}

	if (!texDefn.readXML(definitions, node)) return false;

	return node->failChildren();
}

