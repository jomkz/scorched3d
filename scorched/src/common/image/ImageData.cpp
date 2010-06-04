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

#include <image/ImageData.h>

ImageData::ImageData() :
	referenceCount_(0),
	bits_(0),
	width_(0),
	height_(0),
	alignment_(0),
	components_(0)
{
}

ImageData::ImageData(int width, int height, bool alpha, unsigned char fill) :
	referenceCount_(0),
	bits_(0),
	width_(0),
	height_(0),
	alignment_(0),
	components_(0)
{
	createBlankInternal(width, height, alpha, fill);
}

ImageData::~ImageData()
{
	clear();
}

void ImageData::reference()
{
	referenceCount_++;
}

void ImageData::dereference()
{
	referenceCount_--;
	if (referenceCount_ <= 0) delete this;
}

void ImageData::clear()
{
	delete [] bits_;
	bits_ = 0;
	width_ = 0;
	height_ = 0;
	alignment_ = 0;
	components_ = 0;
}

void ImageData::createBlankInternal(int width, int height, bool alpha, unsigned char fill)
{
	clear();
	width_ = width;
	height_ = height;
	components_ = alpha?4:3;
	int bitsize = components_ * width * height;

	bits_ = new unsigned char[bitsize];
	memset(bits_, fill, bitsize);
}
