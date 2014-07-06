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

#include <landscapedef/LandscapeTrees.h>
#include <landscapemap/LandscapeMaps.h>
#include <engine/ScorchedContext.h>
#include <common/ProgressCounter.h>
#include <common/RandomGenerator.h>
#include <common/Defines.h>
#include <XML/XMLParser.h>
#include <image/ImageFactory.h>

LandscapeTrees::LandscapeTrees() : 
	XMLEntryContainer("LandscapeTrees",
		"Attempts to place a large number of trees on the lanscape in clusters (groups), only actualy places the trees that pass all the criterias"),
	mincloseness("How close trees may be placed together", 0, 0),
	numobjects("The number of trees to attempt to place on the landscape"),
	maxobjects("The upper limit on the number of trees that are placed on the landscape", 0, 2000),
	mask("The name of a file containing a mask to use when placing trees", 0, ""),
	numclusters("The number of clusters (groups) of trees that will be created"),
	minheight("The minimum landscape heights to allow trees to be placed on"),
	maxheight("The maximum landscape heights to allow trees to be placed on")

{
	addChildXMLEntry("mincloseness", &mincloseness);
	addChildXMLEntry("numobjects", &numobjects);
	addChildXMLEntry("maxobjects", &maxobjects);
	addChildXMLEntry("mask", &mask);
	addChildXMLEntry("numclusters", &numclusters);
	addChildXMLEntry("minheight", &minheight);
	addChildXMLEntry("maxheight", &maxheight);
}

LandscapeTrees::~LandscapeTrees()
{
}

void LandscapeTrees::getPositions(
	HeightMap &hmap,
	RandomGenerator &generator,
	std::list<FixedVector> &returnPositions,
	ProgressCounter *counter)
{
	Image bmap(256, 256);
	Image map = bmap;
	if (mask.getValue().c_str()[0])
	{	
		map = ImageFactory::loadImage(S3D::eModLocation, mask.getValue());
		if (!map.getBits())
		{
			S3D::dialogExit("LandscapeTrees",
				S3D::formatStringBuffer("Error: failed to find mask \"%s\"",
				mask.getValue().c_str()));
		}
		if (!map.getLossless())
		{
			S3D::dialogExit("LandscapeTrees", S3D::formatStringBuffer(
				"Error: Placement mask \"%s\" is not a lossless image format",
				mask.getValue().c_str()));
		}
	}

	// Generate a map of where the trees should go
	unsigned char objectMap[64 * 64];
	memset(objectMap, 0, sizeof(unsigned char) * 64 * 64);

	int groundMapWidth = hmap.getMapWidth();
	int groundMapHeight = hmap.getMapHeight();

	// A few points where trees will be clustered around
	int treeMapMultWidth  = groundMapWidth / 64;
	int treeMapMultHeight = groundMapHeight / 64;
	for (int i=0; i<numclusters.getValue(); i++)
	{
		// Get a random point
		int x = (generator.getRandFixed("LandscapeTrees") * 64).asInt();
		int y = (generator.getRandFixed("LandscapeTrees") * 64).asInt();

		// Check point is in the correct height band
		fixed height = 
			hmap.getHeight(
					x * treeMapMultWidth, y * treeMapMultHeight);
		FixedVector &normal =
			hmap.getNormal(
					x * treeMapMultWidth, y * treeMapMultHeight);

		int mx = int(map.getWidth() * (fixed(x) / 64).asInt());
		int my = int(map.getHeight() * (fixed(y) / 64).asInt());
		unsigned char *bits = map.getBits() +
			mx * 3 + my * map.getWidth() * 3;
		if (bits[0] > 127 &&
			height > minheight.getValue() && 
			height < maxheight.getValue() && 
			normal[2] > fixed(true, 7000))
		{
			// Group other areas around this point that are likely to get trees
			// Do a few groups
			int n = (generator.getRandFixed("LandscapeTrees") * 10).asInt() + 5;
			for (int j=0; j<n; j++)
			{
				// Check groups is within bounds
				int newX = x + (generator.getRandFixed("LandscapeTrees") * 8).asInt() - 4;
				int newY = y + (generator.getRandFixed("LandscapeTrees") * 8).asInt() - 4;
				if (newX >= 0 && newX < 64 &&
					newY >= 0 && newY < 64)
				{
					FixedVector &normal =
						hmap.getNormal(
							newX * treeMapMultWidth, newY * treeMapMultHeight);
					height = 
						hmap.getHeight(
							newX * treeMapMultWidth, newY * treeMapMultHeight);
					if (height > minheight.getValue() && 
						height < maxheight.getValue() && 
						normal[2] > fixed(true, 7000))
					{
						objectMap[newX + 64 * newY] = 64;
					}
				}
			}

			// Add the original point
			objectMap[x + 64 * y] = 255;
		}
	}

	// Smooth the treemap
	unsigned char objectMapCopy[64 * 64];
	memcpy(objectMapCopy, objectMap, sizeof(unsigned char) * 64 * 64);
	fixed matrix[3][3];
	for (int i=0; i<3; i++)
	{
		for (int j=0; j<3; j++)
		{
			matrix[i][j] = fixed(true, 7000); // How much smoothing is done (> is more)
			if (i==1 && j==1) matrix[i][j] = 4;
		}
	}
	for (int y=0; y<64; y++)
	{
		for (int x=0; x<64; x++)
		{
			// Total is used to catch corner cases
			fixed total = 0;
			fixed inc = 0;
			for (int i=0; i<3; i++)
			{
				for (int j=0; j<3; j++)
				{
					int newX = i + x - 1;
					int newY = j + y - 1;
					
					if (newX >= 0 && newX < 64 &&
						newY >= 0 && newY < 64)
					{
						inc += objectMapCopy[newX + 64 * newY];
						total += matrix[i][j];
					}
				}
			}
	 
			objectMap[x + 64 * y] = (unsigned char)(inc/total).asInt();
		}
	}

	// Add lots of trees, more chance of adding a tree where
	// the map is stongest
	int objectCount = 0;
	const int NoIterations = numobjects.getValue();
	for (int i=0; i<NoIterations && objectCount < maxobjects.getValue(); i++)
	{
		if (i % 1000 == 0) if (counter) 
			counter->setNewPercentage(
				S3D_MAX(float(i)/float(NoIterations), float(objectCount) / float(maxobjects.getValue())) *100.0f);

		fixed lx = generator.getRandFixed("LandscapeTrees") * fixed(groundMapWidth);
		fixed ly = generator.getRandFixed("LandscapeTrees") * fixed(groundMapHeight);
		int rx = (lx.asInt());
		int ry = (ly.asInt());
		int nx = rx / treeMapMultWidth;
		int ny = ry / treeMapMultHeight;
		int r = objectMap[nx + 64 * ny];
		int nr = (generator.getRandFixed("LandscapeTrees") * 512).asInt();

		if (nr < r)
		{
			fixed height = 
				hmap.getInterpHeight(lx, ly);
			if (height > minheight.getValue() + fixed(true, 5000))
			{
				objectCount ++;
				FixedVector position;
				position[0] = lx;
				position[1] = ly;
				position[2] = height;
				if (checkCloseness(position, returnPositions))
				{
					returnPositions.push_back(position);
				}
			}
		}
	}
}

bool LandscapeTrees::checkCloseness(FixedVector &position, std::list<FixedVector> &returnPositions)
{
	std::list<FixedVector>::iterator currentItor = returnPositions.begin(),
		endItor = returnPositions.end();
	for (; currentItor != endItor; ++currentItor)
	{
		FixedVector *object = &(*currentItor);

		fixed distx = (*object)[0] - position[0];
		fixed disty = (*object)[1] - position[1];
		if (distx.abs() < mincloseness.getValue() &&
			disty.abs() < mincloseness.getValue())
		{
			return false;
		}
	}
	return true;
}

LandscapeTreesList::LandscapeTreesList() :
	XMLEntryList<LandscapeTrees>("Defines a set of trees.")
{
}

LandscapeTreesList::~LandscapeTreesList()
{
}

LandscapeTrees *LandscapeTreesList::createXMLEntry(void *xmlData)
{
	return new LandscapeTrees();
}
