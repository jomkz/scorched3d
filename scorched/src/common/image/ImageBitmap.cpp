////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2009
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

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <image/ImageBitmap.h>
#include <common/Defines.h>
#include <SDL/SDL.h>

ImageBitmap::ImageBitmap() :
	width_(0), height_(0), alpha_(false), newbits_(0),
	owner_(true)
{

}

ImageBitmap::ImageBitmap(int startWidth, int startHeight, bool alpha, unsigned char fill) : 
	width_(startWidth), height_(startHeight), alpha_(alpha), newbits_(0),
	owner_(true)
{
	createBlankInternal(startWidth, startHeight, alpha, fill);
}

ImageBitmap::~ImageBitmap()
{
	clear();
}

void ImageBitmap::clear()
{
	if (owner_) delete [] newbits_;
	newbits_ = 0;
	width_ = 0;
	height_ = 0;
}

bool ImageBitmap::loadFromFile(const char * filename, const char *alphafilename, bool invert)
{
	ImageBitmap bitmap;
	if (!bitmap.loadFromFile(filename, false)) return false;
	ImageBitmap alpha;
	if (!alpha.loadFromFile(alphafilename, false)) return false;

	if (bitmap.getBits() && alpha.getBits() && 
		bitmap.getWidth() == alpha.getWidth() &&
		bitmap.getHeight() == alpha.getHeight())
	{
		createBlankInternal(bitmap.getWidth(), bitmap.getHeight(), true);
		unsigned char *bbits = bitmap.getBits();
		unsigned char *abits = alpha.getBits();
		unsigned char *bits = getBits();
		for (int y=0; y<bitmap.getHeight(); y++)
		{
			for (int x=0; x<bitmap.getWidth(); x++)
			{
				bits[0] = bbits[0];
				bits[1] = bbits[1];
				bits[2] = bbits[2];

				unsigned char avg = (unsigned char)(int(abits[0] + abits[1] + abits[2]) / 3);
				if (invert)
				{
					bits[3] = (unsigned char)(255 - avg);
				}
				else
				{
					bits[3] = avg;
				}

				bbits += 3;
				abits += 3;
				bits += 4;
			}
		}
	}

	return true;
}

bool ImageBitmap::loadFromFile(const char * filename, bool alpha)
{
	SDL_Surface *image = SDL_LoadBMP(filename);
	if (!image) return false;

	if (image->format->BitsPerPixel != 24)
	{
		S3D::dialogExit("ImageBitmap",
			S3D::formatStringBuffer("ERROR: Bitmap \"%s\" is not encoded as a 24bit bitmap",
				filename));
	}

	// Create the internal byte array
	createBlankInternal(image->w, image->h, alpha);

	// Convert the returned bits from BGR to RGB
	// and flip the verticle scan lines
	unsigned char *from = (unsigned char *) image->pixels;
	for (int i=0; i<height_; i ++)
	{
		unsigned char *destRow = ((unsigned char *) newbits_) + ((height_ - i - 1) * (width_ * getComponents()));
		for (int j=0; j<width_; j++)
		{
			unsigned char *dest = destRow + (j * getComponents());

			dest[0] = from[2];
			dest[1] = from[1];
			dest[2] = from[0];
			if (alpha)
			{
				dest[3] = (unsigned char)(from[0]+from[1]+from[2]==0?0:255);
			}
			
			from+=3;
		}
	}
	
	SDL_FreeSurface(image);
	return true;
}

void ImageBitmap::createBlankInternal(int width, int height, bool alpha, unsigned char fill)
{
	clear();
	width_ = width;
	height_ = height;
	alpha_ = alpha;
	int bitsize = getComponents() * width * height;

	newbits_ = new unsigned char[bitsize];
	memset(newbits_, fill, bitsize);
}
