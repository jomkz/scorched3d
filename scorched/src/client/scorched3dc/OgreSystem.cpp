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

#include <scorched3dc/OgreSystem.h>

OgreSystem::OgreSystem() : 
	ogreRoot_(0), ogreWindow_(0),
	guiRenderer_(0)
{
}

OgreSystem::~OgreSystem()
{
	delete ogreRoot_;
	ogreRoot_ = 0;
}

bool OgreSystem::loadPlugin(const Ogre::String &pluginName, const Ogre::String &requiredName)
{
	Ogre::LogManager::getSingletonPtr()->logMessage(
		Ogre::String("Loading Plugin : ") + pluginName);

	// Load plugin
#ifdef _DEBUG
	ogreRoot_->loadPlugin(pluginName + Ogre::String("_d"));
#else
	ogreRoot_->loadPlugin(pluginName);
#endif;

	// Check if the required plugin is installed and ready for use
	// If not: exit the application
	bool found = false;
	Ogre::Root::PluginInstanceList ip = ogreRoot_->getInstalledPlugins();
	for (Ogre::Root::PluginInstanceList::iterator k = ip.begin(); k != ip.end(); k++)
	{
		Ogre::String actualName = (*k)->getName();
		if (actualName == requiredName)
		{
			found = true;
			break;
		}
	}
	if (!found)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage(
			Ogre::String("Failed to find plugin : ") + pluginName + " named " + requiredName);
	}
	return found;
}

bool OgreSystem::createWindow()
{
	Ogre::LogManager::getSingletonPtr()->logMessage("*** Creating Window ***");

	// configure
	// Grab the OpenGL RenderSystem, or exit
	Ogre::RenderSystem* rs = ogreRoot_->getRenderSystemByName("OpenGL Rendering Subsystem");
	if(!(rs->getName() == "OpenGL Rendering Subsystem"))
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("No OpenGL Rendering Subsystem found");
		return false; //No RenderSystem found
	}

	// configure our RenderSystem
	rs->setConfigOption("Full Screen", "No");
	rs->setConfigOption("VSync", "No");
	rs->setConfigOption("FSAA", "0");
	rs->setConfigOption("Video Mode", "1024 x 768 @ 32-bit");
	ogreRoot_->setRenderSystem(rs);

	ogreWindow_ = ogreRoot_->initialise(true, "Scorched3D");
	if (!ogreRoot_->isInitialised())
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Failed to initialize ogre root");
		return false;
	}

	// Setup some defaults
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
	Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(7);

	return true;
}

void OgreSystem::loadResources()
{
	// Landscape
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("data/land", "FileSystem", "Landscape");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("data/land/nvidia", "FileSystem", "Landscape");

	// HydraX
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("data/hydrax", "FileSystem", "Hydrax");

	// CEGUI
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("data/cegui/imagesets", "FileSystem", "Imagesets");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("data/cegui/fonts", "FileSystem", "Fonts");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("data/cegui/schemes", "FileSystem", "Schemes");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("data/cegui/looknfeel", "FileSystem", "LookNFeel");

	// SkyX
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("data/skyx", "FileSystem", "SkyX");

	// General
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("data/bloom", "FileSystem", "General");

	// load resources
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

bool OgreSystem::createUI()
{
	guiRenderer_ = &CEGUI::OgreRenderer::bootstrapSystem();
	CEGUI::Imageset::setDefaultResourceGroup("Imagesets");
	CEGUI::Font::setDefaultResourceGroup("Fonts");
	CEGUI::Scheme::setDefaultResourceGroup("Schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
	CEGUI::WindowManager::setDefaultResourceGroup("Layouts");

	CEGUI::SchemeManager::getSingleton().create("OgreTray.scheme");
	CEGUI::System::getSingleton().setDefaultMouseCursor("OgreTrayImages", "MouseArrow");

	return true;
}

bool OgreSystem::create()
{
	// Create ogre not specifying any configuration (.cfg) files to read
	ogreRoot_ = new Ogre::Root("", "");

	// Load the OpenGL RenderSystem and the SceneManager plugins
	Ogre::LogManager::getSingletonPtr()->logMessage("*** Loading Plugins ***");
	if (!loadPlugin(Ogre::String("RenderSystem_GL"), 
		Ogre::String("GL RenderSystem"))) return false;
	if (!loadPlugin(Ogre::String("Plugin_OctreeSceneManager"), 
		Ogre::String("Octree Scene Manager"))) return false;
	if (!loadPlugin(Ogre::String("Plugin_CgProgramManager"), 
		Ogre::String("Cg Program Manager"))) return false;
	if (!loadPlugin(Ogre::String("Plugin_ParticleFX"), 
		Ogre::String("ParticleFX"))) return false;

	// Create the window, initialize openGL
	if (!createWindow()) return false;

	loadResources();

	return true;
}
