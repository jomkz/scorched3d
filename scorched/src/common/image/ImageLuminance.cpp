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

#include <string.h>
#include <stdlib.h>
#include <image/ImageLuminance.h>
#include <common/Defines.h>

ImageLuminance::ImageLuminance(const std::string &filename) : base_(NULL)
{
	loadFromFile(filename);
}

ImageLuminance::~ImageLuminance()
{
	free(base_);
}

void ImageLuminance::removeOwnership()
{
}

unsigned char *ImageLuminance::getBits()
{
	return base_;
}

int ImageLuminance::getWidth()
{
	return image.xsize;
}

int ImageLuminance::getHeight()
{
	return image.ysize;
}

int ImageLuminance::getComponents()
{
	return image.zsize;
}

int ImageLuminance::getAlignment()
{
	return image.zsize;
}

void ImageLuminance::convertShort(unsigned short *array, unsigned int length)
{
	unsigned short b1, b2;
	unsigned char *ptr;

	ptr = (unsigned char *) array;
	while (length--) 
	{
		b1 = *ptr++;
		b2 = *ptr++;
		*array++ = (unsigned short) ((b1 << 8) | (b2));
	}
}

void ImageLuminance::convertUint(unsigned *array, unsigned int length)
{
	unsigned int b1, b2, b3, b4;
	unsigned char *ptr;

	ptr = (unsigned char *) array;
	while (length--) 
	{
		b1 = *ptr++;
		b2 = *ptr++;
		b3 = *ptr++;
		b4 = *ptr++;
		*array++ = (b1 << 24) | (b2 << 16) | (b3 << 8) | (b4);
	}
}

void ImageLuminance::imageGetRow(FILE *file, unsigned char *buf, int y, int z, unsigned int *rowStart, int *rowSize)
{
	if ((image.type & 0xFF00) == 0x0100) 
	{
		unsigned char *tmp = (unsigned char *) malloc(image.xsize * 256);

		fseek(file, (long) rowStart[y + z * image.ysize], SEEK_SET);
		fread(tmp, 1, (unsigned int) rowSize[y + z * image.ysize], file);

		unsigned char *iPtr = tmp;
		unsigned char *oPtr = buf;
		for (;;) 
		{
			unsigned char pixel = *iPtr++;
			int count = (int) (pixel & 0x7F);
			if (!count) break;

			if (pixel & 0x80) 
			{
				while (count--) *oPtr++ = *iPtr++;
			} 
			else 
			{
				pixel = *iPtr++;
				while (count--) *oPtr++ = pixel;
			}
		}

		free(tmp);
	} 
	else 
	{
		fseek(file, 512 + (y * image.xsize) + (z * image.xsize * image.ysize), SEEK_SET);
		fread(buf, 1, image.xsize, file);
	}
}

bool ImageLuminance::loadFromFile(const std::string &fileName)
{
	union 
	{
		int testWord;
		char testByte[4];
	} endianTest;

	endianTest.testWord = 1;

	int swapFlag = 0;
	if (endianTest.testByte[0] == 1) swapFlag = 1;

	FILE *file = fopen(fileName.c_str(), "rb");
	if (file)
	{
		memset(&image, 0, sizeof(image));
		fread(&image, 1, 12, file);

		if (swapFlag) convertShort(&image.imagic, 6);

		base_ = (unsigned char *)
			malloc(image.xsize * image.ysize * sizeof(unsigned char));

		if (base_)
		{
			if ((image.type & 0xFF00) == 0x0100) 
			{
				int x = image.ysize * image.zsize * (int) sizeof(unsigned);
				unsigned int *rowStart = (unsigned int *) malloc(x);
				int *rowSize = (int *) malloc(x);

				if (rowStart && rowSize)
				{			
					fseek(file, 512, SEEK_SET);
					fread(rowStart, 1, x, file);
					fread(rowSize, 1, x, file);

					if (swapFlag) 
					{
						convertUint(rowStart, x / (int) sizeof(unsigned));
						convertUint((unsigned *) rowSize, x / (int) sizeof(int));
					}

					unsigned char *lptr = base_;
					for (int y = 0; y < image.ysize; y++) 
					{
						imageGetRow(file, lptr, y, 0, rowStart, rowSize);
						lptr += image.xsize;
					}

					free(rowStart);
					free(rowSize);
				}
			}
			else
			{
				unsigned char *lptr = base_;
				for (int y = 0; y < image.ysize; y++) 
				{
					imageGetRow(file, lptr, y, 0, 0, 0);
					lptr += image.xsize;
				}
			}
		}

		fclose(file);
	}

	return true;
}
