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

#include <image/Image.h>
#include <image/ImageFactory.h>
#include <common/DefinesAssert.h>
#include "string.h"

Image::Image()
{
	data_ = new ImageData();
	data_->reference();
}

Image::Image(int width, int height, bool alpha)
{
	data_ = new ImageData(width, height, alpha?4:3, 255);
	data_->reference();
}

Image::Image(int width, int height, int components, unsigned char fill)
{
	data_ = new ImageData(width, height, components, fill);
	data_->reference();
}

Image::Image(const Image &other)
{
	data_ = other.data_;
	data_->reference();
}

Image::~Image()
{
	data_->dereference();
}

Image &Image::operator=(const Image &other)
{
	data_->dereference();
	data_ = other.data_;
	data_->reference();

	return *this;
}

unsigned char *Image::getBitsPos(int x, int y)
{
	DIALOG_ASSERT(x >= 0 && y >= 0 && x < getWidth() && y < getHeight());
	return &getBits()[(x * getComponents()) + (y * getWidth() * getComponents())];
}

unsigned char *Image::getBitsOffset(int offset)
{
	DIALOG_ASSERT(offset >=0 && offset < getComponents() * getWidth() * getHeight());
	return &getBits()[offset];
}

bool Image::writeToFile(const std::string &filename)
{
	if (!getBits() || getComponents() == 4) return false;

	// TODO
	DIALOG_ASSERT(0);
	return true;
}
