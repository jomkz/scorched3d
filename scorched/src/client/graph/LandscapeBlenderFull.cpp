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

#include <graph/LandscapeBlenderFull.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapemap/LandscapeMaps.h>
#include <scorched3dc/OgreSystem.h>

LandscapeBlenderFull::LandscapeBlenderFull(ScorchedClient &scorchedClient, UIStatePlayingLand::LayersInfo &layersInfo, 
		size_t blendMapWidth, size_t blendMapHeight,
		Ogre::TerrainGroup *terrainGroup) :
	LandscapeBlender(scorchedClient, blendMapWidth, blendMapHeight),
	layersInfo_(layersInfo),
	terrainGroup_(terrainGroup)
{
	imageMultiplierX = float(blendMapWidth) / 128.0f;
	imageMultiplierY = float(blendMapHeight) / 128.0f;
	pBlend = new float*[numberSources];

	LandscapeTex *tex = scorchedClient.getLandscapeMaps().getDescriptions().getTex();
	LandscapeTexLayers *texLayers = (LandscapeTexLayers*)tex->texture.getValue();

	// Load the actual landscape textures so we can create one texture with the whole landscape on it (used for the grass map)
	textureMap = new Ogre::Image[numberSources];
	textureMapWorldSize = new float[numberSources];
	textureMap[0].load(S3D::replace(texLayers->texturelayer1.texturename.getValue(), "dds", "jpg"), LANDSCAPE_RESOURCE_GROUP);
	textureMap[1].load(S3D::replace(texLayers->texturelayer2.texturename.getValue(), "dds", "jpg"), LANDSCAPE_RESOURCE_GROUP);
	textureMap[2].load(S3D::replace(texLayers->texturelayer3.texturename.getValue(), "dds", "jpg"), LANDSCAPE_RESOURCE_GROUP);
	textureMapWorldSize[0] = texLayers->texturelayer1.worldsize.getValue().asFloat();
	textureMapWorldSize[1] = texLayers->texturelayer2.worldsize.getValue().asFloat();
	textureMapWorldSize[2] = texLayers->texturelayer3.worldsize.getValue().asFloat();
	textureMapStone.load(S3D::replace(texLayers->texturelayerslope.texturename.getValue(), "dds", "jpg"), LANDSCAPE_RESOURCE_GROUP);
	textureMapWorldSizeStone = texLayers->texturelayerslope.worldsize.getValue().asFloat();
	textureMapDetail.load(S3D::replace(texLayers->texturelayerdetail.texturename.getValue(), "dds", "jpg"), LANDSCAPE_RESOURCE_GROUP);
	textureMapWorldSizeDetail = texLayers->texturelayerdetail.worldsize.getValue().asFloat();
}

LandscapeBlenderFull::~LandscapeBlenderFull()
{
	delete[] textureMap;
	delete[] textureMapWorldSize;
	delete pBlend;
}

void LandscapeBlenderFull::calculate(int landscapeSquaresWidth, int landscapeSquaresHeight)
{
	// Create the layers including blend maps
	for (int x = 0; x<landscapeSquaresWidth; x++)
	{
		for (int y = 0; y<landscapeSquaresHeight; y++)
		{
			terrain = terrainGroup_->getTerrain(x, y);
			calculateTerrain(x, y);
		}
	}
}

void LandscapeBlenderFull::calculateTerrain(int tx, int ty)
{
	sx = tx * 128.0f;
	sy = ty * 128.0f;

	// Figure out which blend maps are from which layers
	Ogre::TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
	Ogre::TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
	Ogre::TerrainLayerBlendMap* blendMap2 = terrain->getLayerBlendMap(3);
	Ogre::TerrainLayerBlendMap* blendMapStone = terrain->getLayerBlendMap(4);
	pBlend[0] = blendMap0->getBlendPointer();
	pBlend[1] = blendMap1->getBlendPointer();
	pBlend[2] = blendMap2->getBlendPointer();
	pBlendStone = blendMapStone->getBlendPointer();
	blendMap0_ = blendMap0;

	// Caclulate values
	subCalculate(true);

	// Set on terrain
	blendMap0->dirty();
	blendMap1->dirty();
	blendMap2->dirty();
	blendMapStone->dirty();
	blendMap0->update();
	blendMap1->update();
	blendMap2->update();
	blendMapStone->update();
}

void LandscapeBlenderFull::convertImageToTerrainAndWorldAndImageSpace(Ogre::uint16 x, Ogre::uint16 y,
	Ogre::Real *tsx, Ogre::Real *tsy,
	float *hxf, float *hyf,
	int *ix, int *iy)
{
	blendMap0_->convertImageToTerrainSpace(x, y, tsx, tsy);
	*hxf = (*tsx * 128.0f) + sx;
	*hyf = (*tsy * 128.0f) + sy;
	*ix = int(*hxf * imageMultiplierX);
	*iy = layersInfo_.normalMapImage.getHeight() - int(*hyf * imageMultiplierY);
}

void LandscapeBlenderFull::getHeightNormal(float tsx, float tsy, float hxf, float hyf, int ix, int iy, float *height, float *normal)
{
	Ogre::ColourValue tempColor = layersInfo_.normalMapImage.getColourAt(ix, iy, 0);
	*normal = tempColor.g;
	*height = terrain->getHeightAtTerrainPosition(tsx, tsy) /
		OgreSystem::OGRE_WORLD_HEIGHT_SCALE;
}

void LandscapeBlenderFull::setLandscapeBlend(int i, float hxf, float hyf, int ix, int iy, float blend, Ogre::ColourValue &textureColorValue)
{
	// Landscape layer color
	(*pBlend[i]) = blend;
	pBlend[i]++;

	// Grass density
	if (layersInfo_.layers[i]->hasGrass)
	{
		Ogre::ColourValue grassDensityValue(blend, blend, blend, 1.0f);
		layersInfo_.layers[i]->grassLayerDensity.setColourAt(grassDensityValue, ix, iy, 0);
	}

	if (blend > 0.0f)
	{
		// Find the coordinates on the source texture
		int colix = int(hxf * OgreSystem::OGRE_WORLD_SCALE / textureMapWorldSize[i] *
			float(textureMap[i].getWidth())) % textureMap[i].getWidth();
		int coliy = int(hyf * OgreSystem::OGRE_WORLD_SCALE / textureMapWorldSize[i] *
			float(textureMap[i].getHeight())) % textureMap[i].getHeight();

		// Texture color
		Ogre::ColourValue layerColorValue = textureMap[i].getColourAt(colix, coliy, 0);
		textureColorValue += layerColorValue * blend;
	}
}



void LandscapeBlenderFull::setStoneBlend(float hxf, float hyf, int ix, int iy, float blend, Ogre::ColourValue &textureColorValue)
{
	// Stone color
	(*pBlendStone) = blend;
	pBlendStone++;

	// Texture color
	if (blend > 0.0f)
	{
		// Find the coordinates on the source texture
		int colix = int(hxf * OgreSystem::OGRE_WORLD_SCALE / textureMapWorldSizeStone *
			float(textureMapStone.getWidth())) % textureMapStone.getWidth();
		int coliy = int(hyf * OgreSystem::OGRE_WORLD_SCALE / textureMapWorldSizeStone *
			float(textureMapStone.getHeight())) % textureMapStone.getHeight();

		// Texture color
		Ogre::ColourValue layerColorValue = textureMapStone.getColourAt(colix, coliy, 0);
		textureColorValue += layerColorValue * blend;
	}
}

void LandscapeBlenderFull::setDefaultBlend(float hxf, float hyf, int ix, int iy, float blend, Ogre::ColourValue &textureColorValue)
{
	// Find the coordinates on the source texture
	int colix = int(hxf * OgreSystem::OGRE_WORLD_SCALE / textureMapWorldSizeDetail *
		float(textureMapDetail.getWidth())) % textureMapDetail.getWidth();
	int coliy = int(hyf * OgreSystem::OGRE_WORLD_SCALE / textureMapWorldSizeDetail *
		float(textureMapDetail.getHeight())) % textureMapDetail.getHeight();

	// Texture color
	Ogre::ColourValue layerColorValue = textureMapDetail.getColourAt(colix, coliy, 0);
	textureColorValue += layerColorValue * blend;
}

void LandscapeBlenderFull::setFullColorBlend(int ix, int iy, Ogre::ColourValue &blend)
{
	// Set the final texture color
	layersInfo_.textureMapImage.setColourAt(blend, ix, iy, 0);
}
