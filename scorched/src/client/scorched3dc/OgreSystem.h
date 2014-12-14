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

#if !defined(__INCLUDE_OgreSystemh_INCLUDE__)
#define __INCLUDE_OgreSystemh_INCLUDE__

class OgreSystem
{
public:
	enum VisibiltyMasks
	{
		VisibiltyMaskLandscape = 0x00000001,
		VisibiltyMaskTargets =   0x00000010,
	};

	static const float OGRE_WORLD_SIZE;
	static const float OGRE_WORLD_HEIGHT_SCALE;
	static const float OGRE_WORLD_SCALE;
	static const float OGRE_WORLD_HEIGHT_SCALE_FIXED;
	static const float OGRE_WORLD_SCALE_FIXED;

	static void destroyAllAttachedMovableObjects(Ogre::SceneNode* node);
	static void destroySceneNode(Ogre::SceneNode* node);

	OgreSystem();
	virtual ~OgreSystem();

	Ogre::Root *getOgreRoot() { return ogreRoot_; }
	Ogre::RenderWindow *getOgreRenderWindow() { return ogreWindow_; }
	Ogre::SceneManager *getOgreLandscapeSceneManager() { return landscapeSceneManager_; }
	CEGUI::OgreRenderer *getGUIRenderer() { return guiRenderer_; }

	bool create();
	bool createUI();

protected:
	// Ogre
	Ogre::Root *ogreRoot_;
	Ogre::RenderWindow* ogreWindow_;
	Ogre::SceneManager* landscapeSceneManager_;

	// CEGUI
	CEGUI::OgreRenderer* guiRenderer_;

	bool loadPlugin(const Ogre::String &pluginName, const Ogre::String &requiredName);
	bool createWindow();
	void loadResources();
};

#endif // __INCLUDE_OgreSystemh_INCLUDE__