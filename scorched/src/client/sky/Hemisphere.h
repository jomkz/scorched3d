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

#if !defined(AFX_HEMISPHERE_H__3577D267_2B6C_4300_B0EE_61E1E50E57DD__INCLUDED_)
#define AFX_HEMISPHERE_H__3577D267_2B6C_4300_B0EE_61E1E50E57DD__INCLUDED_

class Image;
class Vector;
class Hemisphere  
{
public:
	enum Flags
	{
		eWidthTexture = 1
	};

	static void draw(float radius, float radius2,
		int heightSlices = 10, int rotationSlices = 20,
		int startHeightSlice = 0, int startRotationSlice = 0,
		int endHeightSlice = 10, int endRotationSlice = 10,
		bool inverse = false, unsigned int flags = 0);
	static void drawColored(float radius, float radius2, 
		int heightSlices, int rotationSlices,
		int startHeightSlice, int startRotationSlice,
		int endHeightSlice, int endRotationSlice,
		bool inverse, Image &colors, Vector &sunDir, int daytime, 
		bool horizonGlow);

private:
	Hemisphere();
	virtual ~Hemisphere();
};

#endif // !defined(AFX_HEMISPHERE_H__3577D267_2B6C_4300_B0EE_61E1E50E57DD__INCLUDED_)
