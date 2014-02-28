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

#include <scorched3dc/UIStatePlayingLand.h>
#include <scorched3dc/ScorchedUI.h>
#include <scorched3dc/OgreSystem.h>
#include <scorched3dc/noiseutils.h>
#include <noise/noise.h>
#include <client/ScorchedClient.h>
#include <landscapemap/LandscapeMaps.h>

UIStatePlayingLand::UIStatePlayingLand(
	Ogre::SceneManager* sceneMgr, 
	Ogre::Camera* camera,
	Ogre::Light *sunLight,
	Ogre::Light *shadowLight,
	Hydrax::Hydrax *hydrax) : 
	sceneMgr_(sceneMgr), camera_(camera), 
	sunLight_(sunLight), shadowLight_(shadowLight),
	hydrax_(hydrax)
{
	create();
}

UIStatePlayingLand::~UIStatePlayingLand()
{
}

void UIStatePlayingLand::create()
{
	Ogre::Root *ogreRoot = ScorchedUI::instance()->getOgreSystem().getOgreRoot();
	Ogre::RenderWindow *ogreRenderWindow = ScorchedUI::instance()->getOgreSystem().getOgreRenderWindow();

	// Create the terrain objects
	terrainGroup_ = new Ogre::TerrainGroup(sceneMgr_, Ogre::Terrain::ALIGN_X_Z, 129, 3000.0f);
    terrainGroup_->setOrigin(Ogre::Vector3::ZERO);

	// Load terrain data
	defineOptions();
	int landscapeWidth = ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getLandscapeWidth();
	int landscapeHeight = ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getLandscapeHeight();
	for (int x=0; x<landscapeWidth/128; x++)
	{
		for (int y=0; y<landscapeHeight/128; y++)
		{
			defineTerrain(x, y);
		}
	}

	// Generate the terrain
	terrainGroup_->loadAllTerrains(true);

	for (int x=0; x<landscapeWidth/128; x++)
	{
		for (int y=0; y<landscapeHeight/128; y++)
		{
			Ogre::Terrain* terrain = terrainGroup_->getTerrain(x, y);
			initBlendMaps(terrain, x, y);
			hydrax_->getMaterialManager()->addDepthTechnique(terrain->getMaterial()->createTechnique());
		}
	}

	terrainGroup_->updateDerivedData(true);
	terrainGroup_->freeTemporaryResources();
}

void UIStatePlayingLand::defineOptions()
{
	// Terrain settings
	// Configure global
	terrainGlobalOptions_ = new Ogre::TerrainGlobalOptions();
    terrainGlobalOptions_->setMaxPixelError(8);
    terrainGlobalOptions_->setCompositeMapDistance(3000);       // testing composite map
	terrainGlobalOptions_->setDefaultResourceGroup("Landscape");

    // Important to set these so that the terrain knows what to use for derived (non-realtime) data
    terrainGlobalOptions_->setLightMapDirection(shadowLight_->getDerivedDirection());
    terrainGlobalOptions_->setCompositeMapAmbient(sceneMgr_->getAmbientLight());
    terrainGlobalOptions_->setCompositeMapDiffuse(shadowLight_->getDiffuseColour());

    // Configure default import settings for if we use imported image
    Ogre::Terrain::ImportData& defaultimp = terrainGroup_->getDefaultImportSettings();
    defaultimp.terrainSize = 129; 
    defaultimp.worldSize = 3000.0f;   
    defaultimp.inputScale = 200; 
    defaultimp.minBatchSize = 33;
    defaultimp.maxBatchSize = 65;

    // textures
	defaultimp.layerList.resize(5);
	defaultimp.layerList[0].worldSize = 100;
	defaultimp.layerList[0].textureNames.push_back("TxUGLUdirtUgrassU02.dds");
	defaultimp.layerList[0].textureNames.push_back("TxUGLUdirtUgrassU02_n.dds");
	defaultimp.layerList[1].worldSize = 50;
	defaultimp.layerList[1].textureNames.push_back("TxUsandU02.dds");
	defaultimp.layerList[1].textureNames.push_back("TxUsandU02_n.dds");
	defaultimp.layerList[2].worldSize = 200;
	defaultimp.layerList[2].textureNames.push_back("TxUAIUgrassUdirtU01.dds");
	defaultimp.layerList[2].textureNames.push_back("TxUAIUgrassUdirtU01_n.dds");
	defaultimp.layerList[3].worldSize = 200;
	defaultimp.layerList[3].textureNames.push_back("TxUscrubplainU01.dds");
	defaultimp.layerList[3].textureNames.push_back("TxUscrubplainU01_n.dds");
	defaultimp.layerList[4].worldSize = 200;
	defaultimp.layerList[4].textureNames.push_back("TxUcoastalUrockU01.dds");
	defaultimp.layerList[4].textureNames.push_back("TxUcoastalUrockU01_n.dds");
}

void UIStatePlayingLand::defineTerrain(long tx, long ty)
{
	int startX = tx * 128;
	int startY = ty * 128;

	HeightMap &map = ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getHeightMap();
	float *heightData = new float[129 * 129];
	float *currentPoint = heightData;
	for (int y=0; y<129; y++)
	{
		for (int x=0; x<129; x++)
		{
			float height = map.getHeight(startX + x, startY + y).asFloat();
			*currentPoint = height / 8.0f;
			currentPoint++;
		}
	}
	
	terrainGroup_->defineTerrain(tx, ty, heightData);
}

Ogre::Real UIStatePlayingLand::getHeight(const Ogre::Vector3 &position)
{
	Ogre::Vector3 newPosition = position + Ogre::Vector3(0,1000000,0);
	Ogre::Ray ray(newPosition, Ogre::Vector3::NEGATIVE_UNIT_Y);
	Ogre::TerrainGroup::RayResult result = 
		terrainGroup_->rayIntersects(ray);
	if (result.hit) 
	{
		return result.terrain->getHeightAtWorldPosition(newPosition);
	}
	return 0.0f;
}

void UIStatePlayingLand::initBlendMaps(Ogre::Terrain* terrain, long tx, long ty)
{
	unsigned int seed = ScorchedClient::instance()->getLandscapeMaps().getDefinitions().getSeed();

	// Try enabling a color map for color variation
	Ogre::Image colourMapImage;
	colourMapImage.load("seemlessnoise.jpg", terrain->getResourceGroup());
	terrain->setGlobalColourMapEnabled(true, colourMapImage.getWidth());
	Ogre::TexturePtr colourMap  = terrain->getGlobalColourMap();
	colourMap->loadImage(colourMapImage);

	// Generate a noise map for the base map
	// This is like the detail map that adds texture variation across the terrain
	utils::NoiseMap baseMapHeightMap;
	{
		module::Perlin myModule;
		myModule.SetOctaveCount(3);
		myModule.SetFrequency(2.0);
		myModule.SetSeed(seed);

		utils::NoiseMapBuilderPlane heightMapBuilder;
		heightMapBuilder.SetSourceModule(myModule);
		heightMapBuilder.SetDestNoiseMap(baseMapHeightMap);
		heightMapBuilder.SetDestSize(terrain->getLayerBlendMapSize(), terrain->getLayerBlendMapSize());
		heightMapBuilder.SetBounds(2.0 + tx * 4.0, 6.0 + tx * 4.0, 1.0 + ty * 4.0, 5.0 + ty * 4.0);
		heightMapBuilder.Build();
	}
	// Generate a noise map for the difference map
	// This is like the detail map that adds height band variation across the terrain
	utils::NoiseMap differenceMapHeightMap;
	{
		module::Perlin myModule;
		myModule.SetOctaveCount(3);
		myModule.SetFrequency(2.0);
		myModule.SetSeed(seed + 5);

		utils::NoiseMapBuilderPlane heightMapBuilder;
		heightMapBuilder.SetSourceModule(myModule);
		heightMapBuilder.SetDestNoiseMap(differenceMapHeightMap);
		heightMapBuilder.SetDestSize(terrain->getLayerBlendMapSize(), terrain->getLayerBlendMapSize());
		heightMapBuilder.SetBounds(2.0 + tx * 4.0, 6.0 + tx * 4.0, 1.0 + ty * 4.0, 5.0 + ty * 4.0);
		heightMapBuilder.Build();
	}

	// Determine the maximum height of the map
	HeightMap &hMap = ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getHeightMap();
	float hMapMaxHeight = 0;
	for (int ma=0; ma<hMap.getMapWidth(); ma++)
	{
		for (int mb=0;mb<hMap.getMapHeight(); mb++)
		{
			float height = hMap.getHeight(ma, mb).asFloat();
			if (height > hMapMaxHeight) hMapMaxHeight = height;
		}
	}

	const int numberSources = 3;
	const float maxHeight = 30.0f; // Last texture ends at height 30
	const float blendHeightFactor = 0.4f; // Ends blend when 40% into height band
	const float blendNormalSlopeStart = 0.9f; // Starts blending slope at .80
	const float blendNormalSlopeLength = 0.3f; // Blends when 30% more slope
	const float maxOffsetHeight = (maxHeight / numberSources) / 6.0f;

	Ogre::TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
	Ogre::TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
	Ogre::TerrainLayerBlendMap* blendMap2 = terrain->getLayerBlendMap(3);
	Ogre::TerrainLayerBlendMap* blendMapStone = terrain->getLayerBlendMap(4);

	float *pBlend[numberSources];
	pBlend[0] = blendMap0->getBlendPointer();
	pBlend[1] = blendMap1->getBlendPointer();
	pBlend[2] = blendMap2->getBlendPointer();
	float* pBlendStone = blendMapStone->getBlendPointer();
	for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
	{
		for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
		{
			// Figure out the height map coords
			Ogre::Real tsx, tsy;
			blendMap0->convertImageToTerrainSpace(x, y, &tsx, &tsy);
			int hx = int(tsx * float(128)) + tx * 128;
			int hy = int(tsy * float(128)) + ty * 128;

			// Get height and normal information
			HeightMap::HeightData &data = hMap.getHeightData(hx, hy);
			float normal = data.normal[2].asFloat();
			float offSetHeight = (1.0f + differenceMapHeightMap.GetValue(x, y)) * maxOffsetHeight;
			float height = data.height.asFloat() - offSetHeight;

			// Find the index of the current texture by deviding the height into strips
			float heightPer = (height / maxHeight) * (float) numberSources;
			int heightIndex = (int) heightPer;
			if (heightIndex >= numberSources) 
			{
				heightIndex = numberSources - 1;
			}

			// Check if we are in a blending transition phase
			float blendFirstAmount = 1.0f;
			float blendSecondAmount = 0.0f;
			if (heightIndex < numberSources - 1)
			{
				float remainderIndex = heightPer - heightIndex;
				if (remainderIndex > blendHeightFactor)
				{
					// We need to do some blending, figure how much
					remainderIndex -= blendHeightFactor;
					blendSecondAmount = remainderIndex / (1.0f - blendHeightFactor);
					blendFirstAmount = 1.0f - blendSecondAmount;
				}
			}

			// Check to see if we need to blend in the side texture
			float blendSideAmount = 0.0f;
			float blendShoreAmount = 0.0f;
			if (normal < blendNormalSlopeStart)
			{
				if (normal < blendNormalSlopeStart - blendNormalSlopeLength)
				{
					// Only use the side texture
					blendSideAmount = 1.0f;
					blendFirstAmount = 0.0f;
					blendSecondAmount = 0.0f;
				}
				else
				{
					// Blend in the side texture
					float remainderIndex = normal - (blendNormalSlopeStart - blendNormalSlopeLength);
					remainderIndex /= blendNormalSlopeLength;
				
					blendSideAmount = (1.0f - remainderIndex);
					blendFirstAmount *= remainderIndex;
					blendSecondAmount *= remainderIndex;
				}
			}

			// Add in the base value
			float baseValue = baseMapHeightMap.GetValue(x, y);
			if (baseValue > 0.0f && heightIndex > 0) {
				blendFirstAmount *= Ogre::Math::Clamp(1.0f - baseValue, (Ogre::Real) 0, (Ogre::Real) 1);
				blendSecondAmount *= Ogre::Math::Clamp(1.0f - baseValue, (Ogre::Real) 0, (Ogre::Real) 1);
			}

			// Update maps
			for (int i=0; i<numberSources; i++)
			{
				if (i==heightIndex) (*pBlend[i]) = blendFirstAmount;
				else if (i==heightIndex+1) (*pBlend[i]) = blendSecondAmount;
				else (*pBlend[i]) = 0.0f;
				pBlend[i]++;
			}
			(*pBlendStone) = blendSideAmount;
			pBlendStone++;
		}
	}

	blendMap0->dirty();
	blendMap1->dirty();
	blendMap2->dirty();
	blendMapStone->dirty();
	blendMap0->update();
	blendMap1->update();
	blendMap2->update();
	blendMapStone->update();
}