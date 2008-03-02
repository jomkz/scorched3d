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

#include <image/ImageHandle.h>

ImageHandle::ImageHandle() :
	owner_(true),
	width_(0), height_(0),
	alignment_(0), components_(0),
	bits_(0)
{
}

ImageHandle::ImageHandle(const Image &other) :
	owner_(true),
	width_(((Image &)other).getWidth()), 
	height_(((Image &)other).getHeight()),
	alignment_(((Image &)other).getAlignment()),
	components_(((Image &)other).getComponents()),
	bits_(((Image &)other).getBits())
{
	((Image &)other).removeOwnership();
}

ImageHandle::ImageHandle(const ImageHandle &other) :
	owner_(true),
	width_(((ImageHandle&)other).getWidth()), 
	height_(((ImageHandle&)other).getHeight()),
	alignment_(((ImageHandle&)other).getAlignment()),
	components_(((ImageHandle&)other).getComponents()),
	bits_(((ImageHandle&)other).getBits())
{
	((ImageHandle&)other).removeOwnership();
}

ImageHandle::~ImageHandle()
{
	if (owner_) delete [] bits_;
}

ImageHandle &ImageHandle::operator=(const ImageHandle &other)
{
	if (owner_) delete [] bits_;

	owner_ = true;
	width_ = ((ImageHandle &) other).getWidth();
	height_ = ((ImageHandle &) other).getHeight();
	alignment_ = ((ImageHandle &) other).getAlignment();
	components_ = ((ImageHandle &) other).getComponents();
	bits_ = ((ImageHandle &) other).getBits();

	((ImageHandle &) other).removeOwnership();

	return *this;
}

ImageHandle &ImageHandle::operator=(const Image &other)
{
	if (owner_) delete [] bits_;

	owner_ = true;
	width_ = ((ImageHandle &) other).getWidth();
	height_ = ((ImageHandle &) other).getHeight();
	alignment_ = ((ImageHandle &) other).getAlignment();
	components_ = ((ImageHandle &) other).getComponents();
	bits_ = ((ImageHandle &) other).getBits();

	((ImageHandle &) other).removeOwnership();

	return *this;
}
