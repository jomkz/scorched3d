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

LandscapeDefnHeightMapMinMax::LandscapeDefnHeightMapMinMax(const char *tagName, const char *description) :
	XMLEntryGroup(tagName, description),
	min("min", "The minimum value"),
	max("max", "The maximum value")
{
	addChildXMLEntry(&min, &max);
}

LandscapeDefnHeightMapMinMax::~LandscapeDefnHeightMapMinMax()
{
}

LandscapeDefnHeightMapChoice::LandscapeDefnHeightMapChoice() :
	XMLEntryTypeChoice<LandscapeDefnHeightMap>("heightmap", 
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
	file("file", "The filename to load the landsacape from"),
	levelsurround("levelsurround", "Should the border of the landscape be flattened to zero height")
{
	addChildXMLEntry(&file, &levelsurround);
}

LandscapeDefnHeightMapFile::~LandscapeDefnHeightMapFile()
{
}

LandscapeDefnHeightMapGenerate::LandscapeDefnHeightMapGenerate() :
	LandscapeDefnHeightMap("LandscapeDefnHeightMapGenerate", 
		"Procedurally/randomly generates the landscape"),
	mask("mask", "A filename containing a mask file, the mask defines areas that will not have height applied to them"),
	landsmoothing("landsmoothing", "The amount of smoothing to apply to the landscape"),
	levelsurround("levelsurround", "Should the border of the landscape be flattened to zero height"),
	noisefactor("noisefactor", "The amount of noise to apply to the landscape", 0, 1),
	noisewidth("noisewidth", "The grid size for the noise", 0, 64),
	noiseheight("noiseheight", "The grid size for the noise", 0, 64),
	errosions("errosions", "The amount of errosion to apply to the landscape", 0, 0),
	errosionlayering("errosionlayering", "", 0, 0),
	errosionsurroundsize("errosionsurroundsize", "", 0, 25),
	errosionforce("errosionforce", "", 0, fixed(1) / fixed(25)),
	errosionsurroundforce("errosionsurroundforce", "", 0, 1),
	errosionmaxdepth("errosionmaxdepth", "", 0, 4),
	landhills("landhills", "Defines the minimum and maximum hills that will be generated on the landscape"),
	landpeakwidthx("landpeakwidthx", "Defines the minimum and maximum x width of the generated hills"),
	landpeakwidthy("landpeakwidthy", "Defines the minimum and maximum y width of the generated hills"),
	landpeakheight("landpeakheight", "Defines the minimum and maximum height of the generated hills"),
	landheight("landheight", "Defines the minimum and maximum landscape height, the generated landscape will be scaled to fit between these values")
{
	addChildXMLEntry(&landhills, &landpeakwidthx, &landpeakwidthy, &landpeakheight, &landheight);
	addChildXMLEntry(&mask, &landsmoothing, &levelsurround);
	addChildXMLEntry(&noisefactor, &noisewidth, &noiseheight);
	addChildXMLEntry(&errosions, &errosionlayering, &errosionsurroundsize, &errosionforce, &errosionsurroundforce, &errosionmaxdepth);
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
