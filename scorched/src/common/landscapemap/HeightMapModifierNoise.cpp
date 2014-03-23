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

#include <landscapemap/HeightMapModifierNoise.h>
#include <landscapedef/LandscapeDefnHeightMap.h>
#include <common/simplexnoise.h>

#define SIMPLEX_MIN(a, b) ((a)<(b)?(a):(b))
#define SIMPLEX_MAX(a, b) ((a)>(b)?(a):(b))

struct LandInfo
{
	bool inLand;
	int x, y;
	unsigned int epoc;
	unsigned int height;
	fixed actualHeight;
};

static void inline getAdjacency(LandInfo *currentLandInfo, int direction, int sizex, int sizey,
	LandInfo **resultInfo, unsigned int *resultDist)
{
	*resultInfo = 0;
	*resultDist = 0;
	switch (direction)
	{
	case 0:
		if (currentLandInfo->y == 0) return;
		*resultDist = 1000;
		*resultInfo = currentLandInfo - sizex;
		break;
	case 1:
		if (currentLandInfo->y == 0) return;
		if (currentLandInfo->x == sizex - 1) return;
		*resultDist = 1414;
		*resultInfo = currentLandInfo - sizex + 1;
		break;
	case 2:
		if (currentLandInfo->x == sizex - 1) return;
		*resultDist = 1000;
		*resultInfo = currentLandInfo + 1;
		break;
	case 3:
		if (currentLandInfo->x == sizex - 1) return;
		if (currentLandInfo->y == sizey - 1) return;
		*resultDist = 1414;
		*resultInfo = currentLandInfo + sizex + 1;
		break;
	case 4:
		if (currentLandInfo->y == sizey - 1) return;
		*resultDist = 1014;
		*resultInfo = currentLandInfo + sizex;
		break;
	case 5:
		if (currentLandInfo->x == 0) return;
		if (currentLandInfo->y == sizey - 1) return;
		*resultDist = 1414;
		*resultInfo = currentLandInfo + sizex - 1;
		break;
	case 6:
		if (currentLandInfo->x == 0) return;
		*resultDist = 1000;
		*resultInfo = currentLandInfo - 1;
		break;
	case 7:
		if (currentLandInfo->x == 0) return;
		if (currentLandInfo->y == 0) return;
		*resultDist = 1414;
		*resultInfo = currentLandInfo - sizex - 1;
		break;
	}
}

void HeightMapModifierNoise::generateTerrain(HeightMap &hmap, 
	LandscapeDefnHeightMapGenerateNoise &defn,
	RandomGenerator &generator, 
	RandomGenerator &offsetGenerator,
	ProgressCounter *counter)
{
	int sizex = hmap.getMapWidth() + 1;
	int sizey = hmap.getMapHeight() + 1;
	int randx = int(offsetGenerator.getRandUInt("HeightMapModifierNoise::randx") % 10000) - 5000;
	int randy = int(offsetGenerator.getRandUInt("HeightMapModifierNoise::randy") % 10000) - 5000;

	// The parameters that define the border of the landscape
	int border_octaves = defn.landscapenoiseoctaves.getValue();
	fixed border_persistence = defn.landscapenoisepersistence.getValue();
	fixed border_scale = defn.landscapenoisescale.getValue();

	// The parameters that define the random height added to the landscape
	int random_octaves = defn.detailnoiseoctaves.getValue();
	fixed random_persistence = defn.detailnoisepersistence.getValue();

	// The parameters that define the jaggedness of the top of the landscape
	int jagged_octaves = defn.jaggednessnoiseoctaves.getValue();
	fixed jagged_persistence = defn.jaggednessnoisepersistence.getValue();
	fixed jagged_scale = defn.jaggednessnoisescale.getValue();

	// Figure out the island border (its shape)
	if (counter) counter->setNewOp(LANG_STRING("Gen Perlin"));
	LandInfo *landInfo = new LandInfo[sizex * sizey];
	for (int y=0; y<sizey; y++)
	{
		fixed yDist = ((fixed(y) / fixed(sizey)) - fixed(true, 5000)) * fixed(2);
		for (int x=0; x<sizex; x++)
		{
			fixed xDist = ((fixed(x) / fixed(sizex)) - fixed(true, 5000)) * fixed(2);
			
			// Figure out if this point is inside or outside the island
			// Points further away from the mid point are increasingly more likely to be outside
			fixed len = (xDist * xDist + yDist * yDist).sqrt();
			fixed aim = (defn.landscapeoutlinefactor.getValue()*len*len) + defn.landscapeoutlinebase.getValue();
			fixed value = (octave_noise_2d_fixed(border_octaves, border_persistence, border_scale, x + randx, y + randy) + 1) / 2;

			// Create the initial land info for this point
			LandInfo &info = landInfo[x + y * sizex];
			info.x = x;
			info.y = y;
			info.inLand = value > aim;
			info.height = UINT_MAX;
			info.epoc = UINT_MAX;
			info.actualHeight = 0;
		}
	}

	// Get the border of the islands, this is the point where the land disappears into water
	// Add them to the border array
	if (counter) counter->setNewOp(LANG_STRING("Gen Border"));
	unsigned int maxHeight = 0;
	LandInfo **border = new LandInfo*[sizex * sizey];
	memset(border, 0, sizeof(border));
	LandInfo **nextBorder = border;
	LandInfo *current = landInfo;
	unsigned int epoc = 0;
	for (int y=0; y<sizey; y++)
	{
		for (int x=0; x<sizex; x++, current++)
		{
			if (!current->inLand) 
			{
				current->height = 0;
				for (int a=0; a<8; a++)
				{
					LandInfo *resultInfo;
					unsigned int resultDist;
					getAdjacency(current, a, sizex, sizey, &resultInfo, &resultDist);
					if (resultInfo && resultInfo->inLand)
					{
						if (resultInfo->epoc > epoc)
						{
							resultInfo->epoc = epoc;
							*nextBorder = resultInfo;
							nextBorder++;
						}
						if (resultInfo->epoc == epoc)
						{
							if (resultInfo->height > resultDist)
							{
								resultInfo->height = resultDist;
								maxHeight = resultDist;
							}
						}
					}
				}
			}
		}
	}

	// Process the border to get the next border to the border
	// Do this until we have covered all of the land
	// Each time we do it we will be one step further from the initial island border
	// so the distance to the shore increases
	LandInfo **currentBorder = border;
	for (;;)
	{
		int borderItems = nextBorder - currentBorder;
		if (borderItems == 0) break;
		epoc++;

		for (int b=0; b<borderItems; b++, currentBorder++)
		{
			for (int a=0; a<8; a++)
			{
				LandInfo *resultInfo;
				unsigned int resultDist;
				getAdjacency(*currentBorder, a, sizex, sizey, &resultInfo, &resultDist);
				if (resultInfo && resultInfo->inLand)
				{
					if (resultInfo->epoc > epoc)
					{
						resultInfo->epoc = epoc;
						*nextBorder = resultInfo;
						nextBorder++;
					}
					if (resultInfo->epoc >= epoc)
					{
						unsigned int newHeight = (*currentBorder)->height + resultDist;
						if (resultInfo->height > newHeight)
						{
							resultInfo->height = newHeight;
							maxHeight = SIMPLEX_MAX(newHeight, maxHeight);
						}
					}
				}
			}
		}
	}

	// Redistribute heights
	// Assign heights so that those closest to the water are the lowest heights
	// Add some randomness to make it less uniform
	{
		fixed SCALE_FACTOR = defn.landscapescalefactor.getValue();
		int borderItems = nextBorder - border;
		currentBorder = border;
		fixed jaggedStart = defn.jaggedstart.getValue();
		for (int i=0; i<borderItems; i++, currentBorder++) {
			// Let y(x) be the total area that we want at elevation <= x.
			// We want the higher elevations to occur less than lower
			// ones, and set the area to be y(x) = 1 - (1-x)^2.
			fixed y = fixed(i)/fixed(borderItems-1);
			// Now we have to solve for x, given the known y.
			//  *  y = 1 - (1-x)^2
			//  *  y = 1 - (1 - 2x + x^2)
			//  *  y = 2x - x^2
			//  *  x^2 - 2x + y = 0
			// From this we can use the quadratic equation to get:
			fixed x = SCALE_FACTOR.sqrt() - (SCALE_FACTOR*(fixed(1)-y)).sqrt();
			if (x > 1) x = 1;  
				
			x *= (octave_noise_2d_fixed(random_octaves, random_persistence, border_scale, 
				(*currentBorder)->x + randx, (*currentBorder)->y + randy) + 1) / 2;
			if (x > jaggedStart)
			{
				x = jaggedStart + (x - jaggedStart) * (octave_noise_2d_fixed(jagged_octaves, jagged_persistence, jagged_scale, 
					(*currentBorder)->x + randx, (*currentBorder)->y + randy) + 1) / 2;
			}

			(*currentBorder)->actualHeight = x;
		}
	}

	HeightMap::HeightData *currentData = hmap.getHeightData();
	current = landInfo;
	for (int y=0; y<sizey; y++)
	{
		for (int x=0; x<sizex; x++, current++, currentData++)
		{
			currentData->normal.zero();
			// current->inLand ? 1.0f:-1.0f
			currentData->height = current->actualHeight * defn.landscapeheightscale.getValue();
		}
	}

	delete [] landInfo;
	delete [] border;
}
