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

// should be a static local function
static void getTerrainImage(bool flipX, bool flipY, Ogre::Image& img)
{
    img.load("Island.png", "Landscape");
    if (flipX)   img.flipAroundY();
    if (flipY)   img.flipAroundX();
}

static void initBlendMaps(Ogre::Terrain* terrain)
{
	Ogre::TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
	Ogre::TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
	Ogre::Real minHeight0 = 70;
	Ogre::Real fadeDist0 = 15;
	Ogre::Real minHeight1 = 100;
	Ogre::Real fadeDist1 = 40;
	float* pBlend0 = blendMap0->getBlendPointer();
	float* pBlend1 = blendMap1->getBlendPointer();
	for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
	{
		for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
		{
			Ogre::Real tx, ty;
 
			blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
			Ogre::Real height = terrain->getHeightAtTerrainPosition(tx, ty);
			Ogre::Real val = (height - minHeight0) / fadeDist0;
			val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
			*pBlend0++ = val;
 
			val = (height - minHeight1) / fadeDist1;
			val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
			*pBlend1++ = val;
		}
	}
	blendMap0->dirty();
	blendMap1->dirty();
	blendMap0->update();
	blendMap1->update();
}

UIStatePlayingLand::UIStatePlayingLand(
	Ogre::SceneManager* sceneMgr, 
	Ogre::Camera* camera,
	Ogre::Light *sunLight,
	Ogre::Light *shadowLight) : 
	sceneMgr_(sceneMgr), camera_(camera), 
	sunLight_(sunLight), shadowLight_(shadowLight)
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
    terrainGroup_->setFilenameConvention(Ogre::String("Scorched3DO-Cache"), Ogre::String("dat"));
    terrainGroup_->setOrigin(Ogre::Vector3::ZERO);

	// Load terrain data
	terrainsImported_ = false;
	defineOptions();
	defineTerrain(0, 0);

	// Generate the terrain
	terrainGroup_->loadAllTerrains(true);
	if (terrainsImported_)
	{
		Ogre::TerrainGroup::TerrainIterator ti = terrainGroup_->getTerrainIterator();
		while(ti.hasMoreElements())
		{
			Ogre::Terrain* t = ti.getNext()->instance;
			initBlendMaps(t);
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
    defaultimp.worldSize = 3000.0f;      // Set up for island.cfg
    defaultimp.inputScale = 200; 
    defaultimp.minBatchSize = 33;
    defaultimp.maxBatchSize = 65;

    // textures
	defaultimp.layerList.resize(3);
	defaultimp.layerList[0].worldSize = 100;
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
	defaultimp.layerList[1].worldSize = 30;
	defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
	defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
	defaultimp.layerList[2].worldSize = 200;
	defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
	defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");
}

void UIStatePlayingLand::defineTerrain(long x, long y)
{
    Ogre::String filename = terrainGroup_->generateFilename(x, y);
    if (Ogre::ResourceGroupManager::getSingleton().resourceExists(terrainGroup_->getResourceGroup(), filename))
    {
        terrainGroup_->defineTerrain(x, y, filename);
    }
    else
    {
        Ogre::Image img;
        getTerrainImage(x % 2 != 0, y % 2 != 0, img);
        terrainGroup_->defineTerrain(x, y, &img);
        terrainsImported_ = true;
    }
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
