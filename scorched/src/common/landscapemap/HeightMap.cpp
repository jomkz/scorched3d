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

#include <common/Triangle.h>
#include <landscapemap/HeightMap.h>
#include <common/Defines.h>

static const int minMapShift = 3;

HeightMap::HeightMap() : 
	hMap_(0), normals_(0), minMap_(0), maxMap_(0), backupMap_(0),
	nvec(fixed(0), fixed(0), fixed(1))
{
}

HeightMap::~HeightMap()
{
	delete [] hMap_;
	delete [] normals_;
	delete [] minMap_;
	delete [] maxMap_;
	delete [] backupMap_;
}

void HeightMap::create(const int width, const int height)
{
	width_ = width; 
	height_ = height;

	delete [] hMap_;
	delete [] normals_;
	delete [] backupMap_;
	hMap_ = new fixed[(width_ + 1) * (height_ + 1)];
	normals_ = new FixedVector[(width_ + 1) * (height_ + 1)];
	backupMap_ = new fixed[(width_ + 1) * (height_ + 1)];

    delete [] minMap_;
	delete [] maxMap_;
	minWidth_ = width >> minMapShift;
	minHeight_ = height >> minMapShift;
	minMap_ = new fixed[(minWidth_ + 1) * (minHeight_ + 1)];
	maxMap_ = new fixed[(minWidth_ + 1) * (minHeight_ + 1)];

	reset();
}

void HeightMap::backup()
{
	memcpy(backupMap_, hMap_, sizeof(fixed)  * (width_ + 1) * (height_ + 1));
}

void HeightMap::reset()
{
	memset(hMap_, 0, sizeof(fixed)  * (width_ + 1) * (height_ + 1));
	memset(backupMap_, 0, sizeof(fixed)  * (width_ + 1) * (height_ + 1));
	resetNormals();
	for (int i=0; i<(minWidth_ + 1) * (minHeight_ + 1); i++) minMap_[i] = fixed::MAX_FIXED;
	for (int i=0; i<(minWidth_ + 1) * (minHeight_ + 1); i++) maxMap_[i] = fixed(0);
}

void HeightMap::resetNormals()
{
	memset(normals_, 0, sizeof(FixedVector)  * (width_ + 1) * (height_ + 1));
}

bool HeightMap::getVector(FixedVector &vec, int x, int y)
{
	if (x < 0 || y < 0 || x>width_ || y>height_) return false;

	vec[0] = fixed(x);
	vec[1] = fixed(y);
	vec[2] = getHeight(x,y);
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
	fixed ihx = w.floor(); 
	fixed ihy = h.floor();
	fixed ihx2 = ihx+1;
	fixed ihy2 = ihy+1; 

	fixed fhx = w - ihx;
	fixed fhy = h - ihy;
	
	fixed heightA = getHeight(ihx.asInt(), ihy.asInt());
	fixed heightB = getHeight(ihx.asInt(), ihy2.asInt());
	fixed heightC = getHeight(ihx2.asInt(), ihy.asInt());
	fixed heightD = getHeight(ihx2.asInt(), ihy2.asInt());

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
	if (w >= 0 && h >= 0 && w<=width_ && h<=height_) 
	{
		int pos = (width_+1) * h + w;
		FixedVector &normal = normals_[pos];
		if (normal[0] == fixed(0) && 
			normal[1] == fixed(0) && 
			normal[2] == fixed(0))
		{
			int x = w;
			int y = h;

			static FixedVector C;
			getVector(C, x, y);

			static FixedVector total;
			total.zero();

			int times = 0;
			for (int dist=1; dist<=3; dist+=2)
			{
				for (int a=0, b=1; a<4; a++, b++)
				{
					if (b>3) b=0;

					static FixedVector A;
					int aPosX, aPosY;
					getVectorPos(a, aPosX, aPosY, dist);
					if (!getVector(A, aPosX + x, aPosY + y)) continue;

					static FixedVector B;
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
		}

		return normal; 
	}
	nvec.zero();
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

	static FixedVector normalDiffAB;
	normalDiffAB = normalB;
	normalDiffAB -= normalA;
	normalDiffAB *= fhy;
	static FixedVector normalDiffCD;
	normalDiffCD = normalD;
	normalDiffCD -= normalC;
	normalDiffCD *= fhy;

	static FixedVector normalE;
	normalE = normalA;
	normalE += normalDiffAB;
	static FixedVector normalF;
	normalF = normalC;
	normalF += normalDiffCD;

	static FixedVector normalDiffEF;
	normalDiffEF = normalF;
	normalDiffEF -= normalE;
	normalDiffEF *= fhx;

	normal = normalE;
	normal += normalDiffEF;
}

void HeightMap::setHeight(int w, int h, fixed height)
{
	DIALOG_ASSERT(w >= 0 && h >= 0 && w<=width_ && h<=height_);
	hMap_[(width_+1) * h + w] = height;

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
				normals_[(width_+1) * y + x].zero();
			}
		}
	}
	normals_[(width_+1) * h + w].zero();

	int newW = w >> minMapShift;
	int newH = h >> minMapShift;
	DIALOG_ASSERT(newW >= 0 && newH >= 0 && newW<=minWidth_ && newH<=minHeight_);
	int minOffSet = (minWidth_+1) * newH + newW;
	fixed *minHeight = &minMap_[minOffSet];
	if (*minHeight > height)
	{
		*minHeight = height;
	}
	fixed *maxHeight = &maxMap_[minOffSet];
	if (*maxHeight < height)
	{
		*maxHeight = height;
	}
}

void HeightMap::resetMinHeight()
{
	for (int i=0; i<(minWidth_ + 1) * (minHeight_ + 1); i++) minMap_[i] = fixed::MAX_FIXED;
	for (int i=0; i<(minWidth_ + 1) * (minHeight_ + 1); i++) maxMap_[i] = fixed(0);
	for (int h=0; h<height_; h++)
	{
		for (int w=0; w<width_; w++)
		{
			fixed height = getHeight(w, h);
			int newW = w >> minMapShift;
			int newH = h >> minMapShift;
			DIALOG_ASSERT(newW >= 0 && newH >= 0 && newW<=minWidth_ && newH<=minHeight_);
			int minOffSet = (minWidth_+1) * newH + newW;
			fixed *minHeight = &minMap_[minOffSet];
			if (*minHeight > height)
			{
				*minHeight = height;
			}
			fixed *maxHeight = &maxMap_[minOffSet];
			if (*maxHeight < height)
			{
				*maxHeight = height;
			}
		}
	}
}

fixed HeightMap::getMinHeight(int w, int h)
{
	DIALOG_ASSERT(w >= 0 && h >= 0 && w<=minWidth_ && h<=minHeight_);
	fixed minHeight = minMap_[(minWidth_+1) * h + w];
	return minHeight;
}

fixed HeightMap::getMaxHeight(int w, int h)
{
	DIALOG_ASSERT(w >= 0 && h >= 0 && w<=minWidth_ && h<=minHeight_);
	fixed maxHeight = maxMap_[(minWidth_+1) * h + w];
	return maxHeight;
}

