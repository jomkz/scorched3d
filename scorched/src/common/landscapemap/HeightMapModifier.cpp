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

#include <math.h>
#include <stdlib.h>
#include <landscapemap/HeightMapModifier.h>
#include <landscapedef/LandscapeDefn.h>
#include <common/Defines.h>
#include <image/ImageBitmap.h>
#include <image/ImageFactory.h>

void HeightMapModifier::levelSurround(HeightMap &hmap)
{
	for (int x=0; x<=hmap.getMapWidth(); x++)
	{	
		hmap.setHeight(x, 0, fixed(0));
		hmap.setHeight(x, 1, fixed(0));
		hmap.setHeight(x, hmap.getMapHeight(), fixed(0));
		hmap.setHeight(x, hmap.getMapHeight()-1, fixed(0));
	}

	for (int y=0; y<=hmap.getMapHeight(); y++)
	{
		hmap.setHeight(0, y, fixed(0));
		hmap.setHeight(1, y, fixed(0));
		hmap.setHeight(hmap.getMapWidth(), y, fixed(0));
		hmap.setHeight(hmap.getMapWidth()-1, y, fixed(0));
	}
}

void HeightMapModifier::smooth(HeightMap &hmap, 
							   LandscapeDefnHeightMapGenerate &defn,
							   ProgressCounter *counter)
{
	if (defn.landsmoothing == 0) return;
	if (counter) counter->setNewOp("Smoothing");

	fixed *newhMap_ = new fixed[(hmap.getMapWidth()+1) * (hmap.getMapHeight()+1)];

	fixed matrix[5][5];
	for (int i=0; i<5; i++)
	{
		for (int j=0; j<5; j++)
		{
			matrix[i][j] = fixed(defn.landsmoothing); // How much smoothing is done (> is more)
			if (i==2 && j==2) matrix[i][j] = fixed(1);
		}
	}

	int x;
	for (x=0; x<=hmap.getMapWidth(); x++)
	{
		if (counter) counter->setNewPercentage((100.0f * float(x)) / float(hmap.getMapWidth()));
		for (int y=0; y<=hmap.getMapHeight(); y++)
		{
			// Total is used to catch corner cases
			fixed total = fixed(0);
			fixed inc = fixed(0);
			for (int i=0; i<5; i++)
			{
				for (int j=0; j<5; j++)
				{
					int newi = i + x - 2;
					int newj = j + y - 2;
					if (newi>=0 && newi <= hmap.getMapWidth() &&
						newj>=0 && newj <= hmap.getMapHeight())
					{
						inc += matrix[i][j] * hmap.getHeight(newi, newj);
						total += matrix[i][j];
					}
				}
			}

			newhMap_[(hmap.getMapWidth()+1) * y + x] = inc / total;
		}
	}

	fixed *start = newhMap_;
	for (int y=0; y<=hmap.getMapHeight(); y++)
	{
		for (x=0; x<=hmap.getMapWidth(); x++)
		{
			hmap.setHeight(x, y, *(start++));
		}
	}
	delete [] newhMap_;
}

void HeightMapModifier::scale(HeightMap &hmap, 
							  LandscapeDefnHeightMapGenerate &defn,
							  RandomGenerator &generator, 
							  ProgressCounter *counter)
{
	if (counter) counter->setNewOp("Scaling Phase 1");

	fixed max = hmap.getHeight(0,0);
	fixed min = hmap.getHeight(0,0);

	int x;
	for (x=0; x<=hmap.getMapWidth(); x++)
	{
		if (counter) counter->setNewPercentage((100.0f * float(x)) / float(hmap.getMapWidth()));
		for (int y=0; y<=hmap.getMapHeight(); y++)
		{
			if (hmap.getHeight(x,y) > max) max = hmap.getHeight(x,y);
			if (hmap.getHeight(x,y) < min) min = hmap.getHeight(x,y);
		}
	}

	if (counter) counter->setNewOp("Scaling Phase 2");

	fixed realMax = ((fixed(defn.landheightmax) - fixed(defn.landheightmin)) * generator.getRandFixed()) + 
		defn.landheightmin;
	fixed per = realMax / max;

	for (x=0; x<=hmap.getMapWidth(); x++)
	{
		if (counter) counter->setNewPercentage((100.0f * float(x)) / float(hmap.getMapWidth()));
		for (int y=0; y<=hmap.getMapHeight(); y++)
		{
			fixed height = hmap.getHeight(x,y);
			hmap.setHeight(x,y, height * per);
		}
	}
}

void HeightMapModifier::addCirclePeak(HeightMap &hmap, FixedVector &start, 
									  fixed sizew, fixed sizew2, fixed sizeh,
									  RandomGenerator &offsetGenerator)
{
	fixed maxdist = MAX(sizew2, sizew);
	fixed sizewsq = sizew * sizew * fixed(true, 12000);
	int startx = MAX(0, (start[0] - maxdist).asInt());
	int starty = MAX(0, (start[1] - maxdist).asInt());
	int endx = MIN(hmap.getMapWidth(), (start[0] + maxdist).asInt());
	int endy = MIN(hmap.getMapHeight(), (start[1] + maxdist).asInt());

	fixed posX, posY;
	fixed pt2 = fixed(1) / fixed(5);
	for (int x=startx; x<=endx; x++)
	{
		for (int y=starty; y<=endy; y++)
		{
			posX = fixed(x);
			posY = fixed(y);

			fixed distX = (posX - start[0]) * sizew2 / sizew;
			fixed distY = posY - start[1];
			fixed distsq = distX * distX + distY * distY;

			if (distsq < sizewsq)
			{
				fixed dist = distsq.sqrt();
				fixed distRand = (dist / fixed(20)) * pt2;
				if (distRand > pt2) distRand = pt2;
				dist *= offsetGenerator.getRandFixed() * distRand + fixed(1) - (distRand / fixed(2));
				if (dist < sizew)
				{
					fixed newHeight = (dist * fixed::XPI / sizew).cos() * sizeh / fixed(4) + sizeh / fixed(4);
					newHeight += hmap.getHeight(x,y);
					hmap.setHeight(x, y, newHeight);
				}
			}
		}
	}
}

void HeightMapModifier::generateTerrain(HeightMap &hmap, 
										LandscapeDefnHeightMapGenerate &defn,
										RandomGenerator &generator,
										RandomGenerator &offsetGenerator,
										ProgressCounter *counter)
{
	if (counter) counter->setNewOp("Teraform Landscape");

	// Create a default mask that allows everything
	ImageBitmap bmap(256, 256);
	ImageHandle maskMap = bmap;

	// Check if we need to load a new mask
	if (!defn.mask.empty())
	{
		std::string fileName = S3D::getDataFile(defn.mask.c_str());
		maskMap = ImageFactory::loadImageHandle(fileName);
		if (!maskMap.getBits())
		{
			S3D::dialogExit("Landscape", S3D::formatStringBuffer(
				"Error: Failed to find mask map \"%s\"",
				fileName.c_str()));
		}
	}

	// Generate the landscape
	hmap.reset();
	fixed useWidthX = fixed(hmap.getMapWidth()); // Use all the width
	fixed useWidthY = fixed(hmap.getMapHeight());
	fixed useBorderX = (fixed(hmap.getMapWidth()) - useWidthX) / fixed(2);
	fixed useBorderY = (fixed(hmap.getMapHeight()) - useWidthY) / fixed(2);
	fixed maskMultX = fixed(maskMap.getWidth()) / fixed(hmap.getMapWidth());
	fixed maskMultY = fixed(maskMap.getHeight()) / fixed(hmap.getMapHeight());

	const int noItter = (fixed(defn.landhillsmax - defn.landhillsmin) *
		generator.getRandFixed() + fixed(defn.landhillsmin)).asInt();

	for (int i=0; i<noItter; i++)
	{
		if (counter) counter->setNewPercentage((100.0f * float(i)) / float(noItter));

		// Choose settings for a random hemisphere
		fixed sizew = (fixed(defn.landpeakwidthxmax) - fixed(defn.landpeakwidthxmin)) * generator.getRandFixed() 
			+ fixed(defn.landpeakwidthxmin);
		fixed sizew2 = (fixed(defn.landpeakwidthymax) - fixed(defn.landpeakwidthymin)) * generator.getRandFixed() 
			+ fixed(defn.landpeakwidthymin) + sizew;
		fixed sizeh = ((fixed(defn.landpeakheightmax) - fixed(defn.landpeakheightmin)) * generator.getRandFixed() 
					   + defn.landpeakheightmin) * MAX(sizew, sizew2);

		// Choose a border around this hemisphere
		fixed bordersizex = fixed(0);
		fixed bordersizey = fixed(0);
		if (defn.levelsurround)
		{
			fixed bordersize = MAX(sizew, sizew2) * fixed(true, 12000); // 1.2
			bordersizex = bordersize + useBorderX;
			bordersizey = bordersize + useBorderY;
		}

		// Choose a point for this hemisphere
		fixed sx = (generator.getRandFixed() * (fixed(hmap.getMapWidth()) - 
												(bordersizex * fixed(2)))) + bordersizex;
		fixed sy = (generator.getRandFixed() * (fixed(hmap.getMapHeight()) - 
												(bordersizey * fixed(2)))) + bordersizey;

		// Check if the point passes the mask
		bool ok = true;
		if (sx.asInt() >= 0 && sx.asInt() < hmap.getMapWidth() &&
			sy.asInt() >= 0 && sy.asInt() < hmap.getMapHeight())
		{
			int bx = (sx * maskMultX).asInt();
			int by = maskMap.getWidth() - (sy * maskMultY).asInt();
			if (bx >= 0 && bx < maskMap.getWidth() && 
				by >= 0 && by < maskMap.getHeight())
			{
			unsigned char maskPt = maskMap.getBits()[(bx * 3) + (by * maskMap.getWidth() * 3)];

			//printf("%i %i %i %s\n", maskPt, bx, by, defn.mask.c_str());
			ok = ((generator.getRandFixed() * fixed(255)) < fixed(maskPt));
			}
		}

		if (ok)
		{
			// Actually add the hemisphere
			FixedVector start(sx, sy, fixed(0));
			addCirclePeak(hmap, start, sizew, sizew2, sizeh, offsetGenerator);
		}
	}

	scale(hmap, defn, generator, counter);
	if (defn.levelsurround) levelSurround(hmap);
	smooth(hmap, defn, counter);
	if (defn.levelsurround) levelSurround(hmap);
}

