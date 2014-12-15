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

#if !defined(AFX_LandscapeBlenderFull_H__516D85F7_420B_43EB_B0BE_563DCBE1B143__INCLUDED_)
#define AFX_LandscapeBlenderFull_H__516D85F7_420B_43EB_B0BE_563DCBE1B143__INCLUDED_

#include <graph/LandscapeBlender.h>
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include <uistate/UIStatePlayingLand.h>

class LandscapeBlenderFull : public LandscapeBlender
{
public:
	LandscapeBlenderFull(ScorchedClient &scorchedClient, UIStatePlayingLand::LayersInfo &layersInfo, 
		size_t blendMapWidth, size_t blendMapHeight,
		Ogre::TerrainGroup *terrainGroup);
	virtual ~LandscapeBlenderFull();

	void calculate(int landscapeSquaresWidth, int landscapeSquaresHeight);
protected:
	UIStatePlayingLand::LayersInfo &layersInfo_;
	Ogre::TerrainGroup *terrainGroup_;
	Ogre::TerrainLayerBlendMap* blendMap0_;
	Ogre::Terrain* terrain;
	float **pBlend, *pBlendStone;
	float sx, sy;
	float imageMultiplierX, imageMultiplierY;
	Ogre::Image *textureMap, textureMapStone, textureMapDetail;
	float *textureMapWorldSize, textureMapWorldSizeStone, textureMapWorldSizeDetail;

	void calculateTerrain(int tx, int ty);
	virtual void convertImageToTerrainAndWorldAndImageSpace(Ogre::uint16 x, Ogre::uint16 y,
		Ogre::Real *tsx, Ogre::Real *tsy, 
		float *hxf, float *hyf,
		int *ix, int *iy);
	virtual void getHeightNormal(float tsx, float tsy, float hxf, float hyf, int ix, int iy, float *height, float *normal);
	virtual void setLandscapeBlend(int i, float hxf, float hyf, int ix, int iy, float blend, Ogre::ColourValue &textureColorValue);
	virtual void setStoneBlend(float hxf, float hyf, int ix, int iy, float blend, Ogre::ColourValue &textureColorValue);
	virtual void setDefaultBlend(float hxf, float hyf, int ix, int iy, float blend, Ogre::ColourValue &textureColorValue);
	virtual void setFullColorBlend(int ix, int iy, Ogre::ColourValue &blend);

private:
};

#endif // !defined(AFX_LandscapeBlenderFull_H__516D85F7_420B_43EB_B0BE_563DCBE1B143__INCLUDED_)
