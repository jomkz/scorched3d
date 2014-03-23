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

LandscapeDefnHeightMap *LandscapeDefnHeightMapChoice::createXMLEntry(const std::string &type, void *xmlData)
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
	"Procedurally/randomly generates the landscape using perlin noise generators"),
	landscapenoiseoctaves("Parameter for the noise that generates the island outline. "
		" Adding more octaves increases the detail of Perlin noise, with the added drawback of increasing the calculation time."),
	landscapenoisepersistence("Parameter for the noise that generates the island outline. "
		"  A multiplier that determines how quickly the amplitudes diminish for each successive octave in a Perlin-noise function. "
		"  i.e. how much detail from each octave is left in the final result"),
	landscapenoisescale("Parameter for the noise that generates the island outline. "
		"  A multiplier that determines how much the noise is scaled, more scale causes the noise to become less random"),
	landscapeoutlinefactor("Parameter that defines the island outline.  "
		"  A factor that the noise value at a given point must be greate than to be outside the island border."
		"  Generally a larger factor will mean that the border will be more irregular."
		"  In general the base + the factor should = 1, where the base < 1 and the factor < 1"),
	landscapeoutlinebase("Parameter that defines the island outline.  "
		"  A straight addition to the outline factor that the noise value at a given point must be greate than to be outside the island border."
		"  Generally a larger base will mean that the border will be more regular."
		"  In general the base + the factor should = 1, where the base < 1 and the factor < 1"),
	landscapescalefactor("Parameter that defines the scale used when re-distributing heights across the landscape."),
	landscapeheightscale("Parameter that defines the height of the final landscape. "
		"  The redistributed heights are multiplies by this value to get the final height"),
	detailnoiseoctaves("Parameter for the noise that adds some random detail to the island. "
		" Adding more octaves increases the detail of Perlin noise, with the added drawback of increasing the calculation time."),
	detailnoisepersistence("Parameter for the noise that adds some random detail to the island. "
		"  A multiplier that determines how quickly the amplitudes diminish for each successive octave in a Perlin-noise function. "
		"  i.e. how much detail from each octave is left in the final result"),
	jaggednessnoiseoctaves("Parameter for the noise that defines the jaggedness of the top of the landscape. "
		" Adding more octaves increases the detail of Perlin noise, with the added drawback of increasing the calculation time."),
	jaggednessnoisepersistence("Parameter for the noise that defines the jaggedness of the top of the landscape. "
		"  A multiplier that determines how quickly the amplitudes diminish for each successive octave in a Perlin-noise function. "
		"  i.e. how much detail from each octave is left in the final result"),
	jaggednessnoisescale("Parameter for the noise that defines the jaggedness of the top of the landscape. "
		"  A multiplier that determines how much the noise is scaled, more scale causes the noise to become less random"),
	jaggedstart("Parameter for the noise that defines the jaggedness of the top of the landscape. "
		"  The fraction of the landscape that below which that jaggedness will not be applied")
{
	addChildXMLEntry("landscapenoiseoctaves", &landscapenoiseoctaves,
		"landscapenoisepersistence", &landscapenoisepersistence,
		"landscapenoisescale", &landscapenoisescale,
		"landscapescalefactor", &landscapescalefactor,
		"landscapeheightscale", &landscapeheightscale,
		"landscapeoutlinefactor", &landscapeoutlinefactor,
		"landscapeoutlinebase", &landscapeoutlinebase);
	addChildXMLEntry("detailnoiseoctaves", &detailnoiseoctaves,
		"detailnoisepersistence", &detailnoisepersistence);
	addChildXMLEntry("jaggednessnoiseoctaves", &jaggednessnoiseoctaves,
		"jaggednessnoisepersistence", &jaggednessnoisepersistence,
		"jaggednessnoisescale", &jaggednessnoisescale,
		"jaggedstart", &jaggedstart);
}

LandscapeDefnHeightMapGenerateNoise::~LandscapeDefnHeightMapGenerateNoise()
{
}
