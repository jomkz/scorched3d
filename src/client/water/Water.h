////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
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

#if !defined(__INCLUDE_Waterh_INCLUDE__)
#define __INCLUDE_Waterh_INCLUDE__

#include <common/ProgressCounter.h>
#include <common/Vector.h>
#include <image/ImageHandle.h>
#include <GLEXT/GLTexture.h>

class Water2;
class Water2Renderer;
class WaterMapPoints;
class WaterWaveDistance;
class WaterWaves;
class Water
{
public:
	Water();
	virtual ~Water();

	void draw();
	void generate(ProgressCounter *counter = 0);
	void simulate(float frameTime);
	bool explosion(Vector position, float size);

	bool getWaterOn() { return waterOn_; }
	float getWaterHeight() { return height_; }
	Image &getWaterBitmap() { return bitmapWater_; }
	float *getIndexErrors();
	float getWaveDistance(int x, int y);

	void bindWaterReflection();
	void unBindWaterReflection();
	void drawPoints();

	GLTexture &getReflectionTexture();

protected:
	bool waterOn_;
	float height_;
	Water2 *wMap_;
	Water2Renderer *wTex_;
	WaterMapPoints *wMapPoints_;
	WaterWaveDistance *wWaveDistance_;
	WaterWaves *waves_;
	GLTexture landTexWater_;
	ImageHandle bitmapWater_;

};

#endif // __INCLUDE_Waterh_INCLUDE__
