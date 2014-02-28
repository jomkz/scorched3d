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

#include <uistate/UIStatePlayingLand.h>
#include <scorched3dc/ScorchedUI.h>
#include <scorched3dc/OgreSystem.h>
#include <common/simplexnoise.h>
#include <common/DefinesAssert.h>
#include <client/ScorchedClient.h>
#include <client/ClientOptions.h>
#include <landscapemap/LandscapeMaps.h>

UIStatePlayingLand::UIStatePlayingLand(
	Ogre::SceneManager* sceneMgr, 
	Ogre::Camera* camera,
	Ogre::Light *sunLight,
	Ogre::Light *shadowLight,
	Hydrax::Hydrax *hydrax) : 
	sceneMgr_(sceneMgr), camera_(camera), 
	sunLight_(sunLight), shadowLight_(shadowLight),
	hydrax_(hydrax), landscapeGrid_(0), hmap_(0)
{
	create();
}

UIStatePlayingLand::~UIStatePlayingLand()
{
}

void UIStatePlayingLand::create()
{
	hmap_ = &ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getHeightMap();
	Ogre::Root *ogreRoot = ScorchedUI::instance()->getOgreSystem().getOgreRoot();
	Ogre::RenderWindow *ogreRenderWindow = ScorchedUI::instance()->getOgreSystem().getOgreRenderWindow();

	// Create the terrain objects
	terrainGroup_ = new Ogre::TerrainGroup(sceneMgr_, Ogre::Terrain::ALIGN_X_Z, 129, OgreSystem::OGRE_WORLD_SIZE);
    terrainGroup_->setOrigin(Ogre::Vector3(OgreSystem::OGRE_WORLD_SIZE / 2, 0, OgreSystem::OGRE_WORLD_SIZE + OgreSystem::OGRE_WORLD_SIZE / 2));

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

	// Load the terrain from the loaded data
	terrainGroup_->loadAllTerrains(true);

	// Generate the normal map image
	Ogre::Image normalMapImage;
	createNormalMap(normalMapImage);

	// Create the blend maps
	for (int x=0; x<landscapeWidth/128; x++)
	{
		for (int y=0; y<landscapeHeight/128; y++)
		{
			Ogre::Terrain* terrain = terrainGroup_->getTerrain(x, y);
			initBlendMaps(terrain, normalMapImage, x, y);
		}
	}

	terrainGroup_->update();
	terrainGroup_->freeTemporaryResources();
}

void UIStatePlayingLand::defineOptions()
{
	// Terrain settings
	// Configure global
	terrainGlobalOptions_ = new Ogre::TerrainGlobalOptions();
    terrainGlobalOptions_->setMaxPixelError(8);
    terrainGlobalOptions_->setCompositeMapDistance(10000); 
	terrainGlobalOptions_->setUseVertexCompressionWhenAvailable(false);
	terrainGlobalOptions_->setVisibilityFlags(OgreSystem::VisibiltyMaskLandscape);
	terrainGlobalOptions_->setDefaultResourceGroup("Landscape");

    // Important to set these so that the terrain knows what to use for derived (non-realtime) data
    terrainGlobalOptions_->setLightMapDirection(shadowLight_->getDerivedDirection());
    terrainGlobalOptions_->setCompositeMapAmbient(sceneMgr_->getAmbientLight());
    terrainGlobalOptions_->setCompositeMapDiffuse(shadowLight_->getDiffuseColour());

    // Configure default import settings for if we use imported image
    Ogre::Terrain::ImportData& defaultimp = terrainGroup_->getDefaultImportSettings();
    defaultimp.terrainSize = 129; 
    defaultimp.worldSize = OgreSystem::OGRE_WORLD_SIZE;   
	defaultimp.inputScale = OgreSystem::OGRE_WORLD_HEIGHT_SCALE; 
    defaultimp.minBatchSize = 33;
    defaultimp.maxBatchSize = 65;

    // textures
	defaultimp.layerList.resize(5);
	defaultimp.layerList[0].worldSize = 100;
	defaultimp.layerList[0].textureNames.push_back("TxUscrubplainU01.dds");
	defaultimp.layerList[0].textureNames.push_back("TxUscrubplainU01_n.dds");
	defaultimp.layerList[1].worldSize = 250;
	defaultimp.layerList[1].textureNames.push_back("TxUsandU02.dds");
	defaultimp.layerList[1].textureNames.push_back("TxUsandU02_n.dds");
	defaultimp.layerList[2].worldSize = 500;
	defaultimp.layerList[2].textureNames.push_back("TxUAIUgrassUdirtU01.dds");
	defaultimp.layerList[2].textureNames.push_back("TxUAIUgrassUdirtU01_n.dds");
	defaultimp.layerList[3].worldSize = 300;
	defaultimp.layerList[3].textureNames.push_back("terrainsnowstone01.dds");
	defaultimp.layerList[3].textureNames.push_back("terrainsnowstone01_n.dds");
	defaultimp.layerList[4].worldSize = 200;
	defaultimp.layerList[4].textureNames.push_back("TxUcoastalUrockU01.dds");
	defaultimp.layerList[4].textureNames.push_back("TxUcoastalUrockU01_n.dds");
	/*
	defaultimp.layerList.resize(5);
	defaultimp.layerList[0].worldSize = 100;
	defaultimp.layerList[0].textureNames.push_back("purple.dds");
	defaultimp.layerList[0].textureNames.push_back("TxUscrubplainU01_n.dds");
	defaultimp.layerList[1].worldSize = 50;
	defaultimp.layerList[1].textureNames.push_back("green.dds");
	defaultimp.layerList[1].textureNames.push_back("TxUsandU02_n.dds");
	defaultimp.layerList[2].worldSize = 200;
	defaultimp.layerList[2].textureNames.push_back("blue.dds");
	defaultimp.layerList[2].textureNames.push_back("TxUAIUgrassUdirtU01_n.dds");
	defaultimp.layerList[3].worldSize = 100;
	defaultimp.layerList[3].textureNames.push_back("white.dds");
	defaultimp.layerList[3].textureNames.push_back("terrainsnowstone01_n.dds");
	defaultimp.layerList[4].worldSize = 200;
	defaultimp.layerList[4].textureNames.push_back("red.dds");
	defaultimp.layerList[4].textureNames.push_back("TxUcoastalUrockU01_n.dds");
	*/
}

void UIStatePlayingLand::createNormalMap(Ogre::Image &normalMapImage)
{
	// Add the entire landscape normal data to an image
	// The image we can resize to match the blend map size
	// This image is smoothed during the resize process and is a quick/dirty way
	// of smoothing the landscape normal data over the blend map
	// We don't need to do this with the height data as we can get this from the landscape itself
	unsigned char *rawData = OGRE_ALLOC_T(unsigned char, 
		(hmap_->getMapHeight() + 1)*(hmap_->getMapWidth() + 1)*sizeof(unsigned char), 
		Ogre::MEMCATEGORY_GENERAL);
	unsigned char *currentRawData = rawData;
	for (int y=0; y<hmap_->getMapHeight() + 1; y++)
	{
		HeightMap::HeightData *data = hmap_->getHeightData(hmap_->getMapHeight() - y);
		for (int x=0; x<hmap_->getMapWidth() + 1; x++, currentRawData++, data++)
		{
			FixedVector &normal = data->normal;
			*currentRawData = (unsigned char) ((normal[2].asFloat() + 1.0f) * 126.0f);
		}
	}
	
	Ogre::Terrain* firstTerrain = terrainGroup_->getTerrain(0, 0);

	// Do this rather than firstTerrain->getLayerBlendMapSize() as it seems to be wrong in release mode
	size_t blendMapWidth, blendMapHeight;
	Ogre::TerrainLayerBlendMap* blendMap0 = firstTerrain->getLayerBlendMap(1);
	blendMap0->convertUVToImageSpace(1.0, 1.0, &blendMapWidth, &blendMapHeight);
	blendMapWidth++; // They are indexed by Zero
	blendMapHeight++;

	normalMapImage.loadDynamicImage(rawData, hmap_->getMapWidth() + 1, hmap_->getMapHeight() + 1, 1, Ogre::PF_L8, true);
	int newWidth = hmap_->getMapWidth() / 128 * blendMapWidth;
	int newHeight = hmap_->getMapHeight() / 128 * blendMapHeight;
	normalMapImage.resize(newWidth, newHeight);
}

void UIStatePlayingLand::defineTerrain(long tx, long ty)
{
	int startX = tx * 128;
	int startY = ty * 128;

	HeightMap &map = ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getHeightMap();
	float *heightData = OGRE_ALLOC_T(float, 129*129, Ogre::MEMCATEGORY_GEOMETRY);
	float *currentPoint = heightData;
	for (int y=0; y<129; y++)
	{
		HeightMap::HeightData *data = map.getHeightData(startX, map.getMapHeight() - (startY + y));
		for (int x=0; x<129; x++, currentPoint++, data++)
		{
			float height = data->height.asFloat();
			*currentPoint = height;
		}
	}
	
	Ogre::Terrain::ImportData newImport(terrainGroup_->getDefaultImportSettings());
	newImport.inputFloat = heightData;
	newImport.deleteInputData = false;

	terrainGroup_->defineTerrain(tx, ty, &newImport);
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

bool UIStatePlayingLand::getIntersection(const Ogre::Ray &cameraRay, Ogre::Vector3 *outPosition)
{
	Ogre::TerrainGroup::RayResult result = 
		terrainGroup_->rayIntersects(cameraRay);
	if (result.hit) 
	{
		*outPosition = result.position;
		return true;
	}
	return false;
}

void UIStatePlayingLand::initBlendMaps(Ogre::Terrain* terrain, Ogre::Image &normalMapImage, long tx, long ty)
{
	unsigned int seed = ScorchedClient::instance()->getLandscapeMaps().getDescriptions().getSeed();

	/*
	// Try enabling a color map for color variation
	Ogre::Image colourMapImage;
	colourMapImage.load("colormap.jpg", terrain->getResourceGroup());
    if (tx % 2 == 0) colourMapImage.flipAroundY();
    if (ty % 2 == 0) colourMapImage.flipAroundX();
	terrain->setGlobalColourMapEnabled(true, colourMapImage.getWidth());
	Ogre::TexturePtr colourMap  = terrain->getGlobalColourMap();
	colourMap->loadImage(colourMapImage);
	*/

	// Constants
	const int numberSources = 3;
	const float maxHeight = 18.0f; // Last texture ends at height 
	const float blendHeightFactor = 0.4f; // Ends blend when % into height band
	const float blendNormalSlopeStart = 0.95f; // Starts blending slope at %
	const float blendNormalSlopeLength = 0.15f; // Blends when % more slope
	const float maxOffsetHeight = (maxHeight / numberSources) / 3.0f;

	Ogre::TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
	Ogre::TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
	Ogre::TerrainLayerBlendMap* blendMap2 = terrain->getLayerBlendMap(3);
	Ogre::TerrainLayerBlendMap* blendMapStone = terrain->getLayerBlendMap(4);

	// Do this rather than firstTerrain->getLayerBlendMapSize() as it seems to be wrong in release mode
	size_t blendMapWidth, blendMapHeight;
	blendMap0->convertUVToImageSpace(1.0, 1.0, &blendMapWidth, &blendMapHeight);
	blendMapWidth++; // They are indexed by Zero
	blendMapHeight++;

	float imageMultiplierX = float(blendMapWidth) / 128.0f;
	float imageMultiplierY = float(blendMapHeight) / 128.0f;
	float sx = tx * 128.0f;
	float sy = ty * 128.0f;
	float *pBlend[numberSources];
	pBlend[0] = blendMap0->getBlendPointer();
	pBlend[1] = blendMap1->getBlendPointer();
	pBlend[2] = blendMap2->getBlendPointer();
	float* pBlendStone = blendMapStone->getBlendPointer();
	for (Ogre::uint16 y = 0; y < blendMapHeight; ++y)
	{
		for (Ogre::uint16 x = 0; x < blendMapWidth; ++x)
		{
			// Figure out the height map coords
			Ogre::Real tsx, tsy;
			blendMap0->convertImageToTerrainSpace(x, y, &tsx, &tsy);
			float hxf = (tsx * 128.0f) + sx;
			float hyf = (tsy * 128.0f) + sy;
			int hx = int(hxf);
			int hy = int(hyf);

			// Get height and normal information
			int ix = int(hxf * imageMultiplierX);
			int iy = int(hyf * imageMultiplierY);
			Ogre::ColourValue tempColor = normalMapImage.getColourAt(ix, iy, 0);
			float normal = tempColor.g;
			// This stops the height bands from being too uniform
			float offSetHeight = octave_noise_2d(1, 0.8f, 1.0f / 16.0f, -hxf, -hyf) 
				* maxOffsetHeight;
			float height = terrain->getHeightAtTerrainPosition(tsx, tsy) / 
				OgreSystem::OGRE_WORLD_HEIGHT_SCALE - offSetHeight;

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
			// Generate a noise map for the base map
			// This is like the detail map that adds texture variation across the terrain
			// Don't add it right at the lowest height, or at the max height
			if (heightPer > 0.25f && heightPer < float(numberSources) - 0.5f) 
			{
				float baseValue = octave_noise_2d(1, 0.8f, 1.0f / 8.0f, hxf, hyf);
				if (baseValue > 0.0f)
				{
					if (heightPer < 0.5f) baseValue *= (heightPer - 0.25f) * 4.0f;
					else if (heightPer > float(numberSources) - 0.75f) baseValue *= 1.0f - ((heightPer - (float(numberSources) - 0.75f)) * 4.0f);
					blendFirstAmount *= Ogre::Math::Clamp(1.0f - baseValue, (Ogre::Real) 0, (Ogre::Real) 1);
					blendSecondAmount *= Ogre::Math::Clamp(1.0f - baseValue, (Ogre::Real) 0, (Ogre::Real) 1);
				}
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

void UIStatePlayingLand::updateLandscapeTextures()
{
	int landscapeWidth = ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getLandscapeWidth();
	int landscapeHeight = ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getLandscapeHeight();
	for (int x=0; x<landscapeWidth/128; x++)
	{
		for (int y=0; y<landscapeHeight/128; y++)
		{
			Ogre::Terrain* terrain = terrainGroup_->getTerrain(x, y);
			Ogre::MaterialPtr material = terrain->getMaterial();
			Ogre::Technique *technique = material->createTechnique();

			hydrax_->getMaterialManager()->addDepthTechnique(technique);
		}
	}
}

void UIStatePlayingLand::update(float frameTime)
{
	bool landscapeEnabled = (sceneMgr_->getVisibilityMask() & OgreSystem::VisibiltyMaskLandscape) != 0;
	if (landscapeEnabled != ClientOptions::instance()->getLandscapeDraw())
	{
		sceneMgr_->setVisibilityMask(sceneMgr_->getVisibilityMask() ^ OgreSystem::VisibiltyMaskLandscape);
	}
	if ((landscapeGrid_!=0) != ClientOptions::instance()->getLandscapeGridDraw()) 
	{
		if (ClientOptions::instance()->getLandscapeGridDraw()) showLandscapePoints();
		else hideLandscapePoints();
	}

	if (!dirtyTerrains_.empty())
	{
		std::set<Ogre::Terrain *>::iterator itor;
		for (itor = dirtyTerrains_.begin();
			itor != dirtyTerrains_.end();
			++itor)
		{
			(*itor)->update(); // TODO: Optimize to only do once
		}
	}
}

void UIStatePlayingLand::updateHeight(int x, int y, int w, int h)
{
	// Disregard if totaly outside of heightmap
	if (x + w < 0) return;
	if (y + h < 0) return;
	if (x >= hmap_->getMapWidth()) return;
	if (y >= hmap_->getMapHeight()) return;

	// Move hit box inside of bounds
	if (x < 0) { w += x; x = 0; }
	if (y < 0) { h += y; y = 0; }
	int dx = x + w - hmap_->getMapWidth();
	if (dx > 0) { w -= dx; }
	int dy = y + h - hmap_->getMapHeight();
	if (dy > 0) { h -= dy; }

	// Find which terrain instance this is on
	int sx = x / 128;
	int rx = x % 128;
	int sy = y / 128;
	int ry = y % 128;

	if (rx + w >= 128 && ry + h >= 128)
	{
		updateHeightTerrain(sx, sy, rx, ry, 127 - rx, 127 - ry);
		updateHeightTerrain(sx + 1, sy, 0, ry, rx + w - 128, 127 - ry);
		updateHeightTerrain(sx, sy + 1, rx, 0, 127 - rx, ry + h - 128);
		updateHeightTerrain(sx + 1, sy + 1, 0, 0, rx + w - 128, ry + h - 128);
	}
	else if (rx + w >= 128)
	{
		updateHeightTerrain(sx, sy, rx, ry, 127 - rx, h);
		updateHeightTerrain(sx + 1, sy, 0 , ry, rx + w - 128, h);
	}
	else if (ry + h >= 128)
	{
		updateHeightTerrain(sx, sy, rx, ry, w, 127 - ry);
		updateHeightTerrain(sx, sy + 1, rx , 0, w, ry + h - 128);
	}
	else
	{
		updateHeightTerrain(sx, sy, rx, ry, w, h);
	}
}

void UIStatePlayingLand::updateHeightTerrain(int tx, int ty, int x, int y, int w, int h)
{
	DIALOG_ASSERT(x >= 0 && y >= 0 && x + w < 128 && y + h < 128);

	int mapX = tx * 128 + x;
	int mapY = ty * 128 + y;
	DIALOG_ASSERT(mapX > 0 && mapY > 0 && mapX + w < hmap_->getMapWidth() && mapY + h < hmap_->getMapHeight());

	Ogre::Terrain *terrain = terrainGroup_->getTerrain(tx, (hmap_->getMapHeight() / 128) - 1 - ty);
	for (int b=0; b<=h; b++)
	{
		HeightMap::HeightData *mapHeightData = hmap_->getHeightData(mapX, mapY + b);
		float *landscapeHeightData = terrain->getHeightData(x, 127 - y - b);
		for (int a=0; a<=w; a++, mapHeightData++, landscapeHeightData++)
		{
			//DIALOG_ASSERT(mapX + a < hmap_->getMapWidth() && mapY + b < hmap_->getMapHeight());
			//DIALOG_ASSERT(x + a < 128 && 127 - y - b >= 0);

			float height = mapHeightData->height.getInternalData() * OgreSystem::OGRE_WORLD_HEIGHT_SCALE_FIXED;
			(*landscapeHeightData) = height;
		}
	}
	terrain->dirtyRect(Ogre::Rect(x, 127 - y - h, x + w + 1, 127 - y + 1));
	dirtyTerrains_.insert(terrain);
}

void UIStatePlayingLand::showLandscapePoints() 
{
	// A grid of lines showing the landscape as it is in the height map
	// This could fail in a horrible way as the hmap is being accessed while the client thread is running
	// Oh, well cba synchronizing for this debug case
	Ogre::SceneManager *sceneManager = ScorchedUI::instance()->getOgreSystem().getOgreLandscapeSceneManager();
	landscapeGrid_ = sceneManager->getRootSceneNode()->createChildSceneNode("LandscapeGrid");
	HeightMap::HeightData *heightData = hmap_->getHeightData();
	for (int y=0; y<hmap_->getMapHeight() + 1; y++)
	{
		Ogre::ManualObject* manual = sceneManager->createManualObject();
		manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);
		for (int x=0; x<hmap_->getMapWidth() + 1; x++, heightData++)
		{
			manual->position(OgreSystem::OGRE_WORLD_SCALE * x, 
				heightData->height.asFloat() * OgreSystem::OGRE_WORLD_HEIGHT_SCALE, 
				OgreSystem::OGRE_WORLD_SCALE * y);
		}
		manual->end();
		landscapeGrid_->attachObject(manual);
	}
}

void UIStatePlayingLand::hideLandscapePoints()
{
	OgreSystem::destroySceneNode(landscapeGrid_);
	landscapeGrid_ = 0;
}
