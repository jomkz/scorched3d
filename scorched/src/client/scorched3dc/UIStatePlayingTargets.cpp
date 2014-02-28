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

#include <scorched3dc/UIStatePlayingTargets.h>
#include <client/ScorchedClient.h>
#include <client/ClientOptions.h>
#include <common/DefinesString.h>
#include <scorched3dc/ScorchedUI.h>
#include <scorched3dc/OgreSystem.h>
#include <landscapemap/LandscapeMaps.h>
#include <OGRE/OgreManualObject.h>

UIStatePlayingTargets::UIStatePlayingTargets(Ogre::SceneManager* sceneMgr) :
	sceneMgr_(sceneMgr)
{
	create();
}

UIStatePlayingTargets::~UIStatePlayingTargets()
{

}

void UIStatePlayingTargets::update(float frameTime)
{
	bool targetsEnabled = (sceneMgr_->getVisibilityMask() & OgreSystem::VisibiltyMaskTargets) != 0;
	if (targetsEnabled != ClientOptions::instance()->getTargetsDraw())
	{
		sceneMgr_->setVisibilityMask(sceneMgr_->getVisibilityMask() ^ OgreSystem::VisibiltyMaskTargets);
	}
}

void UIStatePlayingTargets::create()
{
	/*
	// An grid of points showing the landscape as it is in the height map
	Ogre::SceneManager *sceneManager = ScorchedUI::instance()->getOgreSystem().getOgreLandscapeSceneManager();
	Ogre::ManualObject* manual = sceneManager->createManualObject("manual");
	manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_POINT_LIST);
	HeightMap &hMap = ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getHeightMap();
	HeightMap::HeightData *heightData = hMap.getHeightData();
	for (int y=0; y<hMap.getMapHeight() + 1; y++)
	{
		for (int x=0; x<hMap.getMapWidth() + 1; x++, heightData++)
		{
			manual->position(OgreSystem::OGRE_WORLD_SCALE * x, 
				heightData->height.asFloat() * OgreSystem::OGRE_WORLD_HEIGHT_SCALE, 
				OgreSystem::OGRE_WORLD_SCALE * y);
		}
	}
	manual->end();
	sceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(manual);
	*/
}
