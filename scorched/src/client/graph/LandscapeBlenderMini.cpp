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

#include <graph/LandscapeBlenderMini.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapemap/LandscapeMaps.h>
#include <scorched3dc/OgreSystem.h>
#include <uistate/UIStatePlayingLand.h>

LandscapeBlenderMini::LandscapeBlenderMini(ScorchedClient &scorchedClient) :
	LandscapeBlender(scorchedClient, 
	scorchedClient.getLandscapeMaps().getGroundMaps().getHeightMap().getMapWidth() * 4,
	scorchedClient.getLandscapeMaps().getGroundMaps().getHeightMap().getMapHeight() * 4)
{
	heightMap = &scorchedClient.getLandscapeMaps().getGroundMaps().getHeightMap();

	// Create the texture map image (contains the top down image of the landscape)
	unsigned char *rawData = OGRE_ALLOC_T(unsigned char,
		blendMapWidth * blendMapHeight * sizeof(unsigned char)* 3,
		Ogre::MEMCATEGORY_GENERAL);
	textureMapImage.loadDynamicImage(rawData, blendMapWidth, blendMapHeight,
		1, Ogre::PF_BYTE_RGB, true);

	LandscapeTex *tex = scorchedClient.getLandscapeMaps().getDescriptions().getTex();
	LandscapeTexLayers *texLayers = (LandscapeTexLayers*)tex->texture.getValue();

	// Load the actual landscape textures so we can create one texture with the whole landscape on it (used for the grass map)
	textureMap = new Ogre::Image[numberSources];
	textureMap[0].load(S3D::replace(texLayers->texturelayer1.texturename.getValue(), "dds", "jpg"), LANDSCAPE_RESOURCE_GROUP);
	textureMap[1].load(S3D::replace(texLayers->texturelayer2.texturename.getValue(), "dds", "jpg"), LANDSCAPE_RESOURCE_GROUP);
	textureMap[2].load(S3D::replace(texLayers->texturelayer3.texturename.getValue(), "dds", "jpg"), LANDSCAPE_RESOURCE_GROUP);
	textureMapStone.load(S3D::replace(texLayers->texturelayerslope.texturename.getValue(), "dds", "jpg"), LANDSCAPE_RESOURCE_GROUP);
	textureMapDetail.load(S3D::replace(texLayers->texturelayerdetail.texturename.getValue(), "dds", "jpg"), LANDSCAPE_RESOURCE_GROUP);
}

LandscapeBlenderMini::~LandscapeBlenderMini()
{
	delete[] textureMap;
}

void LandscapeBlenderMini::calculate()
{
	// Caclulate values
	subCalculate(false);
}

void LandscapeBlenderMini::convertImageToTerrainAndWorldAndImageSpace(Ogre::uint16 x, Ogre::uint16 y,
	Ogre::Real *tsx, Ogre::Real *tsy,
	float *hxf, float *hyf,
	int *ix, int *iy)
{
	*tsx = ((Ogre::Real) x) / 4.0f;
	*tsy = ((Ogre::Real) y) / 4.0f;
	*hxf = *tsx;
	*hyf = *tsy;
	*ix = x;
	*iy = y;
}

void LandscapeBlenderMini::getHeightNormal(float tsx, float tsy, float hxf, float hyf, int ix, int iy, float *height, float *normal)
{
	int x = int(tsx);
	int y = int(tsy);
	*height = heightMap->getHeight(x, y).asFloat();
	fNormal = heightMap->getNormal(x, y);
	*normal = fNormal[2].asFloat();
}

void LandscapeBlenderMini::setLandscapeBlend(int i, float hxf, float hyf, int ix, int iy, float blend, Ogre::ColourValue &textureColorValue)
{
	if (blend > 0.0f)
	{
		// Find the coordinates on the source texture
		int colix = ix % textureMap[i].getWidth();
		int coliy = iy % textureMap[i].getHeight();

		// Texture color
		Ogre::ColourValue layerColorValue = textureMap[i].getColourAt(colix, coliy, 0);
		textureColorValue += layerColorValue * blend;
	}
}

void LandscapeBlenderMini::setStoneBlend(float hxf, float hyf, int ix, int iy, float blend, Ogre::ColourValue &textureColorValue)
{
	if (blend > 0.0f)
	{
		// Find the coordinates on the source texture
		int colix = ix % textureMapStone.getWidth();
		int coliy = iy % textureMapStone.getHeight();

		// Texture color
		Ogre::ColourValue layerColorValue = textureMapStone.getColourAt(colix, coliy, 0);
		textureColorValue += layerColorValue * blend;
	}
}

void LandscapeBlenderMini::setDefaultBlend(float hxf, float hyf, int ix, int iy, float blend, Ogre::ColourValue &textureColorValue)
{
	// Find the coordinates on the source texture
	int colix = ix % textureMapDetail.getWidth();
	int coliy = iy % textureMapDetail.getHeight();

	// Texture color
	Ogre::ColourValue layerColorValue = textureMapDetail.getColourAt(colix, coliy, 0);
	textureColorValue += layerColorValue * blend;
}

void LandscapeBlenderMini::setFullColorBlend(int ix, int iy, Ogre::ColourValue &blend)
{
	// Set the final texture color
	textureMapImage.setColourAt(blend, ix, iy, 0);
}
