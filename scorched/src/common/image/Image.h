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

#if !defined(AFX_Image_H__0EBAA0E7_3103_43A4_90C0_5708ECE6DB43__INCLUDED_)
#define AFX_Image_H__0EBAA0E7_3103_43A4_90C0_5708ECE6DB43__INCLUDED_

#include <string>

class ImageHandle;
class Image  
{
public:
	virtual ~Image();

	virtual unsigned char *getBits() = 0;
	virtual unsigned char *getBitsOffset(int offset);
	virtual unsigned char *getBitsPos(int x, int y);
	virtual int getWidth() = 0;
	virtual int getHeight() = 0;

	virtual void removeOwnership() = 0;

	virtual int getAlignment() = 0;
	virtual int getComponents() = 0;

	virtual bool writeToFile(const std::string &filename);

#ifndef S3D_SERVER
	ImageHandle createAlphaMult(float mult);
	ImageHandle createResize(int newWidth, int newHeight);
#endif
};

#endif // !defined(AFX_Image_H__0EBAA0E7_3103_43A4_90C0_5708ECE6DB43__INCLUDED_)
