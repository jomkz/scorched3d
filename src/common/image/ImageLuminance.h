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


// ImageLuminance.h: interface for the ImageLuminance class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ImageLumINANCE_H__15020260_678A_40DF_B49C_AB1796BB3EED__INCLUDED_)
#define AFX_ImageLumINANCE_H__15020260_678A_40DF_B49C_AB1796BB3EED__INCLUDED_

#include <stdio.h>
#include <image/Image.h>

class ImageLuminance : public Image
{
public:
	ImageLuminance(const std::string &filename);
	virtual ~ImageLuminance();

	virtual unsigned char *getBits();
	virtual void removeOwnership();

	virtual int getWidth();
	virtual int getHeight();

	virtual int getComponents();
	virtual int getAlignment();

protected:
	struct ImageRec
	{
	  unsigned short imagic;
	  unsigned short type;
	  unsigned short dim;
	  unsigned short xsize, ysize, zsize;
	} image;

	unsigned char *base_;

	void convertShort(unsigned short *array, unsigned int length);
	void convertUint(unsigned *array, unsigned int length);

	void imageGetRow(FILE *file, unsigned char *buf, int y, int z, unsigned int *rowStart, int *rowSize);
	bool loadFromFile(const std::string &filename);
};

#endif // !defined(AFX_ImageLumINANCE_H__15020260_678A_40DF_B49C_AB1796BB3EED__INCLUDED_)
