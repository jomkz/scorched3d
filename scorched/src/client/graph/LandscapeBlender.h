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

#if !defined(AFX_LandscapeBlender_H__516D85F7_420B_43EB_B0BE_563DCBE1B143__INCLUDED_)
#define AFX_LandscapeBlender_H__516D85F7_420B_43EB_B0BE_563DCBE1B143__INCLUDED_

#include <client/ScorchedClient.h>

class LandscapeBlender
{
public:
	LandscapeBlender(ScorchedClient &scorchedClient, size_t blendMapWidth, size_t blendMapHeight);
	virtual ~LandscapeBlender();

protected:
	size_t blendMapWidth, blendMapHeight;
	int layerOffsetNoiseOctaves;
	float layerOffsetNoisePersistence, layerOffsetNoiseScale;
	float maxOffsetHeight, maxHeight;
	int numberSources;
	float *blendHeightFactor;
	float blendNormalSlopeStart, blendNormalSlopeLength;
	float blendDetailHeightStartFactor, blendDetailHeightEndFactor, blendDetailHeightStartFactorFactor;
	float blendDetailHeightEndFactorFactor, blendDetailAmount;
	float blendDetailNoisePersistence, blendDetailNoiseScale;
	int blendDetailNoiseOctaves;

	void subCalculate(bool useNoise);
	virtual void convertImageToTerrainAndWorldAndImageSpace(Ogre::uint16 x, Ogre::uint16 y,
		Ogre::Real *tsx, Ogre::Real *tsy, 
		float *hxf, float *hyf,
		int *ix, int *iy) = 0;
	virtual void getHeightNormal(float tsx, float tsy, float hxf, float hyf, int ix, int iy, float *height, float *normal) = 0;
	virtual void setLandscapeBlend(int i, float hxf, float hyf, int ix, int iy, float blend, Ogre::ColourValue &textureColorValue) = 0;
	virtual void setStoneBlend(float hxf, float hyf, int ix, int iy, float blend, Ogre::ColourValue &textureColorValue) = 0;
	virtual void setDefaultBlend(float hxf, float hyf, int ix, int iy, float blend, Ogre::ColourValue &textureColorValue) = 0;
	virtual void setFullColorBlend(int ix, int iy, Ogre::ColourValue &blend) = 0;

private:
};

#endif // !defined(AFX_LandscapeBlender_H__516D85F7_420B_43EB_B0BE_563DCBE1B143__INCLUDED_)
