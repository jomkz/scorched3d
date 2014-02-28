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

#if !defined(__INCLUDE_LandscapeDefnHeightMaph_INCLUDE__)
#define __INCLUDE_LandscapeDefnHeightMaph_INCLUDE__

#include <XML/XMLEntrySimpleTypes.h>

class LandscapeDefnHeightMapMinMax : public XMLEntryContainer
{
public:
	LandscapeDefnHeightMapMinMax(const char *description);
	virtual ~LandscapeDefnHeightMapMinMax();

	XMLEntryFixed min, max;
};

class LandscapeDefnHeightMap : public XMLEntryContainer
{
public:
	enum DefnHeightMapType
	{
		eHeightMapFile, 
		eHeightMapGenerate,
		eHeightMapGenerateNoise
	};

	LandscapeDefnHeightMap(const char *name, const char *description);
	virtual ~LandscapeDefnHeightMap();

	virtual DefnHeightMapType getType() = 0;
};

class LandscapeDefnHeightMapChoice : public XMLEntryTypeChoice<LandscapeDefnHeightMap>
{
public:
	LandscapeDefnHeightMapChoice();
	virtual ~LandscapeDefnHeightMapChoice();

	virtual LandscapeDefnHeightMap *createXMLEntry(const std::string &type);
};

class LandscapeDefnHeightMapFile : public LandscapeDefnHeightMap
{
public:
	LandscapeDefnHeightMapFile();
	virtual ~LandscapeDefnHeightMapFile();

	XMLEntryFile file;
	XMLEntryBool levelsurround;

	virtual DefnHeightMapType getType() { return eHeightMapFile; }
};

class LandscapeDefnHeightMapGenerate : public LandscapeDefnHeightMap
{
public:
	LandscapeDefnHeightMapGenerate();
	virtual ~LandscapeDefnHeightMapGenerate();

	XMLEntryFile mask;

	XMLEntryFixed noisefactor;
	XMLEntryInt noisewidth, noiseheight;

	XMLEntryInt errosions;
	XMLEntryInt errosionlayering, errosionsurroundsize;
	XMLEntryFixed errosionforce, errosionmaxdepth;
	XMLEntryFixed errosionsurroundforce;
	
	LandscapeDefnHeightMapMinMax landhills;
	LandscapeDefnHeightMapMinMax landheight;
	LandscapeDefnHeightMapMinMax landpeakwidthx;
	LandscapeDefnHeightMapMinMax landpeakwidthy;
	LandscapeDefnHeightMapMinMax landpeakheight;
	XMLEntryFixed landsmoothing;
	XMLEntryBool levelsurround;

	virtual DefnHeightMapType getType() { return eHeightMapGenerate; }
};

class LandscapeDefnHeightMapGenerateNoise : public LandscapeDefnHeightMap
{
public:
	LandscapeDefnHeightMapGenerateNoise();
	virtual ~LandscapeDefnHeightMapGenerateNoise();

	virtual DefnHeightMapType getType() { return eHeightMapGenerateNoise; }
};

#endif // __INCLUDE_LandscapeDefnHeightMaph_INCLUDE__
