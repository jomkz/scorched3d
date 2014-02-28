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

#include <landscapedef/LandscapeDefnHeightMap.h>

LandscapeDefnHeightMapMinMax::LandscapeDefnHeightMapMinMax(const char *description) :
	XMLEntryContainer("LandscapeDefnHeightMapMinMax", description),
	min("The minimum value"),
	max("The maximum value")
{
	addChildXMLEntry("min", &min, "max", &max);
}

LandscapeDefnHeightMapMinMax::~LandscapeDefnHeightMapMinMax()
{
}

LandscapeDefnHeightMapChoice::LandscapeDefnHeightMapChoice() :
	XMLEntryTypeChoice<LandscapeDefnHeightMap>("LandscapeDefnHeightMapChoice", 
		"Defines the landscape surface (the height map)")
{
}

LandscapeDefnHeightMapChoice::~LandscapeDefnHeightMapChoice()
{
}

LandscapeDefnHeightMap *LandscapeDefnHeightMapChoice::createXMLEntry(const std::string &type)
{
	if (0 == strcmp(type.c_str(), "generatenoise")) return new LandscapeDefnHeightMapGenerateNoise;
	if (0 == strcmp(type.c_str(), "generate")) return new LandscapeDefnHeightMapGenerate;
	if (0 == strcmp(type.c_str(), "file")) return new LandscapeDefnHeightMapFile;
	S3D::dialogMessage("LandscapeDefnType", S3D::formatStringBuffer("Unknown heightmap type %s", type));
	return 0;
}

void LandscapeDefnHeightMapChoice::getAllTypes(std::set<std::string> &allTypes)
{
	allTypes.insert("generatenoise");
	allTypes.insert("generate");
	allTypes.insert("file");
}

LandscapeDefnHeightMap::LandscapeDefnHeightMap(const char *name, const char *description) :
	XMLEntryContainer(name, description)
{
}

LandscapeDefnHeightMap::~LandscapeDefnHeightMap()
{
}

LandscapeDefnHeightMapFile::LandscapeDefnHeightMapFile() :
	LandscapeDefnHeightMap("LandscapeDefnHeightMapFile", 
		"Loads the landscape surface heights from a file"),
	file("The filename to load the landsacape from"),
	levelsurround("Should the border of the landscape be flattened to zero height")
{
	addChildXMLEntry("file", &file, "levelsurround", &levelsurround);
}

LandscapeDefnHeightMapFile::~LandscapeDefnHeightMapFile()
{
}

LandscapeDefnHeightMapGenerate::LandscapeDefnHeightMapGenerate() :
	LandscapeDefnHeightMap("LandscapeDefnHeightMapGenerate", 
		"Procedurally/randomly generates the landscape"),
	mask("A filename containing a mask file, the mask defines areas that will not have height applied to them"),
	landsmoothing("The amount of smoothing to apply to the landscape"),
	levelsurround("Should the border of the landscape be flattened to zero height"),
	noisefactor("The amount of noise to apply to the landscape", 0, 1),
	noisewidth("The grid size for the noise", 0, 64),
	noiseheight("The grid size for the noise", 0, 64),
	errosions("The number of errosion streams to apply to the landscape", 0, 0),
	errosionlayering("The number of errossions that can be applied to the same point in the landscape", 0, 0),
	errosionforce("The depth to errode the landscape per distance erroded", 0, fixed(1) / fixed(25)),
	errosionmaxdepth("The maximum amount of errossion (depth) that can be take from a point on the landscape", 0, 4),
	errosionsurroundsize("The area surrounding erroded points that will be smoothed to make the errosion less severe", 0, 25),
	errosionsurroundforce("The depth that will be taken from the surrounding points per distance away", 0, 1),
	landhills("Defines the minimum and maximum hills that will be generated on the landscape"),
	landpeakwidthx("Defines the minimum and maximum x width of the generated hills"),
	landpeakwidthy("Defines the minimum and maximum y width of the generated hills"),
	landpeakheight("Defines the minimum and maximum height of the generated hills"),
	landheight("Defines the minimum and maximum landscape height, the generated landscape will be scaled to fit between these values")
{
	addChildXMLEntry("landhills", &landhills, 
		"landpeakwidthx", &landpeakwidthx, 
		"landpeakwidthy", &landpeakwidthy, 
		"landpeakheight", &landpeakheight, 
		"landheight", &landheight);
	addChildXMLEntry("mask", &mask, 
		"landsmoothing", &landsmoothing, 
		"levelsurround", &levelsurround);
	addChildXMLEntry("noisefactor", &noisefactor, 
		"noisewidth", &noisewidth, 
		"noiseheight", &noiseheight);
	addChildXMLEntry("errosions", &errosions, 
		"errosionlayering", &errosionlayering, 
		"errosionforce", &errosionforce, 
		"errosionmaxdepth", &errosionmaxdepth,
		"errosionsurroundforce", &errosionsurroundforce, 
		"errosionsurroundsize", &errosionsurroundsize);
}

LandscapeDefnHeightMapGenerate::~LandscapeDefnHeightMapGenerate()
{
}

LandscapeDefnHeightMapGenerateNoise::LandscapeDefnHeightMapGenerateNoise() :
	LandscapeDefnHeightMap("LandscapeDefnHeightMapGenerateNoise", 
		"Procedurally/randomly generates the landscape using perlin noise generators")
{

}

LandscapeDefnHeightMapGenerateNoise::~LandscapeDefnHeightMapGenerateNoise()
{
}
