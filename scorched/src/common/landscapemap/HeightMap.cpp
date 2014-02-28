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

#include <common/Triangle.h>
#include <landscapemap/HeightMap.h>
#include <common/Defines.h>

static const int minMapShift = 3;
static FixedVector nvec(fixed(0), fixed(0), fixed(1));

HeightMap::HeightData HeightMap::EmptyHeightData = { 0, nvec };

HeightMap::HeightMap() : 
	width_(0), height_(0),
	heightData_(0), 
	invertedNormals_(false)
{
}

HeightMap::~HeightMap()
{
	delete [] heightData_;
	heightData_ = 0;
}

void HeightMap::create(const int width, const int height, bool invertedNormals, fixed initialHeight)
{
	invertedNormals_ = invertedNormals;
	width_ = width; 
	height_ = height;

	delete [] heightData_;
	heightData_ = new HeightData[(width_ + 1) * (height_ + 1)];

	reset(initialHeight);
}

void HeightMap::reset(fixed initialHeight)
{
	HeightData *current = heightData_;
	for (int y=0; y<=height_; y++)
	{
		for (int x=0; x<=width_; x++, current++)
		{
			current->height = initialHeight;
			current->normal[0] = fixed(0);
			current->normal[1] = fixed(0);
			current->normal[2] = invertedNormals_?fixed(-1):fixed(1);
		}
	}
}

bool HeightMap::getVector(FixedVector &vec, int x, int y)
{
	if (x < 0 || y < 0 || x>width_ || y>height_) return false;

	vec[0] = fixed(x);
	vec[1] = fixed(y);
	vec[2] = heightData_[(width_+1) * y + x].height;
	return true;
}

void HeightMap::getVectorPos(int pos, int &x, int &y, int dist)
{
	switch (pos)
	{
	case 0:
		x=-dist; y=0;
		break;
	case 1:
		x=0; y=dist;
		break;
	case 2:
		x=dist; y=0;
		break;
	default:
		x=0; y=-dist;
	}
}

bool HeightMap::getIntersect(Line &line, Vector &intersect)
{
	Vector direction = -((Vector &)line.getDirection()).Normalize();
	Vector start = line.getEnd();

	for (int i=0; i<1000; i++)
	{
		fixed height = getHeight((int) start[0], (int) start[1]);
		if (height > fixed(int(start[2])))
		{
			if (start[0] < 0 || start[0] > getMapWidth() ||
				start[1] < 0 || start[1] > getMapHeight())
			{
				return false;
			}

			intersect = start;
			return true;
		}

		start += direction;
	}
	return false;
}

fixed HeightMap::getInterpHeight(fixed w, fixed h)
{
	int ihx = w.floor().asInt(); 
	int ihy = h.floor().asInt();
	int ihx2 = ihx+1;
	int ihy2 = ihy+1; 
	
	if (ihx < 0 || ihy < 0 || ihx>width_ || ihy>height_ &&
		ihx2 < 0 || ihy2 < 0 || ihx2>width_ || ihy2>height_)
	{
		return 0;
	}

	HeightData *startData = &heightData_[(width_+1) * ihy + ihx];

	fixed heightA = startData->height;
	fixed heightB = (startData+width_+1)->height;
	fixed heightC = (startData+1)->height;
	fixed heightD = (startData+width_+2)->height;

	fixed fhx = w - fixed(ihx);
	fixed fhy = h - fixed(ihy);
	fixed heightDiffAB = heightB-heightA;
	fixed heightDiffCD = heightD-heightC;
	fixed heightE = heightA + (heightDiffAB * fhy);
	fixed heightF = heightC + (heightDiffCD * fhy);

	fixed heightDiffEF = heightF - heightE;
	fixed height = heightE + (heightDiffEF * fhx);	

	return height;
}

FixedVector &HeightMap::getNormal(int w, int h)
{
	FixedVector A, B, C, total;
	if (w >= 0 && h >= 0 && w<=width_ && h<=height_) 
	{
		int pos = (width_+1) * h + w;

		HeightMap::HeightData *heightData = &heightData_[pos];

		FixedVector &normal = heightData->normal;
		if (normal[0] == fixed(0) && 
			normal[1] == fixed(0) && 
			normal[2] == fixed(0))
		{
			int x = w;
			int y = h;

			C[0] = fixed(w);
			C[1] = fixed(h);
			C[2] = heightData->height;
			total.zero();

			int times = 0;
			for (int dist=1; dist<=3; dist+=2)
			{
				for (int a=0, b=1; a<4; a+=2, b+=2)
				{
					if (b>3) b=0;

					int aPosX, aPosY;
					getVectorPos(a, aPosX, aPosY, dist);
					if (!getVector(A, aPosX + x, aPosY + y)) continue;

					int bPosX, bPosY;				
					getVectorPos(b, bPosX, bPosY, dist);
					if (!getVector(B, bPosX + x, bPosY + y)) continue;

					A-=C;
					B.StoreInvert();
					B+=C;
					A *= B;
					A.StoreNormalize();
					total += A;
					times += 1;
				}

				if (times > 4) break;
			}

			normal = total.Normalize();
			if (invertedNormals_) normal.StoreInvert();
		}

		return normal; 
	}
	nvec = FixedVector(fixed(0), fixed(0), invertedNormals_?fixed(-1):fixed(1));
	return nvec; 
}

void HeightMap::getInterpNormal(fixed w, fixed h, FixedVector &normal)
{
	fixed ihx = w.floor();
	fixed ihy = h.floor();
	fixed ihx2 = ihx+1;
	fixed ihy2 = ihy+1;

	fixed fhx = w - ihx;
	fixed fhy = h - ihy;

	FixedVector &normalA = getNormal(ihx.asInt(), ihy.asInt());
	FixedVector &normalB = getNormal(ihx.asInt(), ihy2.asInt());
	FixedVector &normalC = getNormal(ihx2.asInt(), ihy.asInt());
	FixedVector &normalD = getNormal(ihx2.asInt(), ihy2.asInt());

	FixedVector normalDiffAB;
	normalDiffAB = normalB;
	normalDiffAB -= normalA;
	normalDiffAB *= fhy;
	FixedVector normalDiffCD;
	normalDiffCD = normalD;
	normalDiffCD -= normalC;
	normalDiffCD *= fhy;

	FixedVector normalE;
	normalE = normalA;
	normalE += normalDiffAB;
	FixedVector normalF;
	normalF = normalC;
	normalF += normalDiffCD;

	FixedVector normalDiffEF;
	normalDiffEF = normalF;
	normalDiffEF -= normalE;
	normalDiffEF *= fhx;

	normal = normalE;
	normal += normalDiffEF;
}

void HeightMap::setHeight(int w, int h, fixed height)
{
	DIALOG_ASSERT(w >= 0 && h >= 0 && w<=width_ && h<=height_);

	HeightData *data = &heightData_[(width_+1) * h + w];
	data->height = height;

	// Reset all of the normals around this position
	for (int dist=1; dist<=3; dist++)
	{
		for (int a=0; a<4; a++)
		{
			int aPosX, aPosY;
			getVectorPos(a, aPosX, aPosY, dist);

			int x = w + aPosX;
			int y = h + aPosY;
			if (x>=0 && y>=0 && x<=width_ && y<=height_)
			{
				heightData_[(width_+1) * y + x].normal.zero();
			}
		}
	}
	data->normal.zero();
}
