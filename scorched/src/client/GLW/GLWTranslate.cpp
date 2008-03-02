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

#include <GLW/GLWTranslate.h>

float GLWTranslate::posX_(0.0f);
float GLWTranslate::posY_(0.0f);

GLWTranslate::GLWTranslate(float x, float y) : x_(x), y_(y)
{
	posX_ += x_;
	posY_ += y_;
}

GLWTranslate::~GLWTranslate()
{
	posX_ -= x_;
	posY_ -= y_;
}

float GLWTranslate::getPosX()
{
	return posX_;
}

float GLWTranslate::getPosY()
{
	return posY_;
}
