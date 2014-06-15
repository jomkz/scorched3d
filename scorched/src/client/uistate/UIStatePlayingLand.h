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

#if !defined(__INCLUDE_UIStatePlayingLandh_INCLUDE__)
#define __INCLUDE_UIStatePlayingLandh_INCLUDE__

#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include <Hydrax/Hydrax.h>
#include <PagedGeometry.h>
#include <set>

class HeightMap;
class UIStatePlayingLand
{
public:
	UIStatePlayingLand(Ogre::SceneManager* sceneMgr,
		Ogre::Camera* camera,
		Ogre::Light *sunLight,
		Ogre::Light *shadowLight,
		Hydrax::Hydrax *hydrax);
	virtual ~UIStatePlayingLand();

	Ogre::Real getHeight(const Ogre::Vector3 &position);
	bool getIntersection(const Ogre::Ray &cameraRay, Ogre::Vector3 *outPosition);
	
	void updateLandscapeTextures();
	void update(float frameTime);
	void updateHeight(int x, int y, int w, int h);

protected:
	Ogre::Camera* camera_;
	Ogre::SceneManager* sceneMgr_;
	Ogre::TerrainGlobalOptions *terrainGlobalOptions_;
	Ogre::TerrainGroup *terrainGroup_;
	Ogre::Light *sunLight_, *shadowLight_;
	Forests::PagedGeometry *grass_;
	Hydrax::Hydrax *hydrax_;
	std::set<Ogre::Terrain *> dirtyTerrains_;
	Ogre::SceneNode *landscapeGrid_;
	HeightMap *hmap_;

	class LayerInfo
	{
	public:
		Ogre::Image normalMapImage;
		Ogre::Image grassLayerImage;
		Ogre::Image grassLayerDensity;
	};

	void create();
	void defineTerrainCreationOptions();
	void defineTerrain(long x, long y);
	void createLayerInfo(LayerInfo &layerInfo, int landscapeSquaresWidth, int landscapeSquaresHeight);
	void initLayers(Ogre::Terrain* terrain, LayerInfo &layerInfo, long tx, long ty);
	void updateHeightTerrain(int tx, int ty, const Ogre::Rect &updateRect);
	void updateAllTerrainHeight();
	void showLandscapePoints();
	void hideLandscapePoints();
	void createGrass(LayerInfo &layerInfo, int landscapeSquaresWidth, int landscapeSquaresHeight);
	void getBlendMapWidth(size_t &width, size_t &height);

	// Used by paging scene manager
	static inline float getTerrainHeight(const float x, const float z, void *userData = NULL);
};

#endif // __INCLUDE_UIStatePlayingLandh_INCLUDE__
