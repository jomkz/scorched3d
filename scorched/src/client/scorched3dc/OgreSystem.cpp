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
#include <dialogs/GUIConsole.h>
#include <dialogs/GUIFrameTimer.h>
#include <client/ClientOptions.h>
#include <OgreShadowCameraSetupFocused.h>

const float OgreSystem::OGRE_WORLD_SIZE(6000.0f);
const float OgreSystem::OGRE_WORLD_HEIGHT_SCALE(50.0f);
const float OgreSystem::OGRE_WORLD_SCALE(6000.0f / 128.0f);
const float OgreSystem::OGRE_WORLD_HEIGHT_SCALE_FIXED(50.0f / fixed::FIXED_RESOLUTION_FLOAT);
const float OgreSystem::OGRE_WORLD_SCALE_FIXED((6000.0f / 128.0f) / fixed::FIXED_RESOLUTION_FLOAT);

OgreSystem::OgreSystem() : 
	ogreRoot_(0), ogreWindow_(0),
	guiRenderer_(0), landscapeSceneManager_(0)
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
	// ogreWindow_->setVSyncEnabled();

	// Create scene manager
	landscapeSceneManager_ = ogreRoot_->createSceneManager(Ogre::ST_GENERIC, "PlayingSceneManager");
	if (ClientOptions::instance()->getShadows())
	{
		landscapeSceneManager_->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED);
		//landscapeSceneManager_->setShadowFarDistance(1000.0f);
		landscapeSceneManager_->setShadowTextureConfig(0, 2048, 2048, Ogre::PF_X8R8G8B8);
		landscapeSceneManager_->setShadowCameraSetup(Ogre::ShadowCameraSetupPtr(new Ogre::FocusedShadowCameraSetup()));
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
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
		S3D::getDataFile("data/land"), "FileSystem", "Landscape");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
		S3D::getDataFile("data/land/nvidia"), "FileSystem", "Landscape");

	// HydraX
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
		S3D::getDataFile("data/hydrax"), "FileSystem", "Hydrax");

	// CEGUI
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
		S3D::getDataFile("data/cegui/imagesets"), "FileSystem", "Imagesets");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
		S3D::getDataFile("data/cegui/fonts"), "FileSystem", "Fonts");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
		S3D::getDataFile("data/cegui/schemes"), "FileSystem", "Schemes");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
		S3D::getDataFile("data/cegui/looknfeel"), "FileSystem", "LookNFeel");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
		S3D::getDataFile("data/cegui/layouts"), "FileSystem", "Layouts");

	// SkyX
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
		S3D::getDataFile("data/skyx"), "FileSystem", "SkyX");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
		S3D::getTempFile("."), "FileSystem", "SkyX");

	// General
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
		S3D::getDataFile("data/bloom"), "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
		S3D::getDataFile("data/general/materials"), "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
		S3D::getDataFile("data/general/textures"), "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
		S3D::getDataFile("data/grass"), "FileSystem", "General");

	// Particle Universe
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
		S3D::getDataFile("data/particleuniverse/core"), "FileSystem", "ParticleUniverse");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
		S3D::getDataFile("data/particleuniverse/textures"), "FileSystem", "ParticleUniverse");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
		S3D::getDataFile("data/particleuniverse/materials"), "FileSystem", "ParticleUniverse");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
		S3D::getDataFile("data/particleuniverse/scripts"), "FileSystem", "ParticleUniverse");

	// Models
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
		S3D::getDataFile("data/models"), "FileSystem", "Models");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
		S3D::getDataFile("data/models/abrams"), "FileSystem", "Models");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
		S3D::getDataFile("data/models/projectiles"), "FileSystem", "Models");

	// load resources
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

bool OgreSystem::createUI()
{
	CEGUI::DefaultLogger *logger = new CEGUI::DefaultLogger();
	CEGUI::Logger::getSingleton().setLogFilename(S3D::getTempFile("CEGUI.log"));
	guiRenderer_ = &CEGUI::OgreRenderer::bootstrapSystem();
	CEGUI::ImageManager::setImagesetDefaultResourceGroup("Imagesets");
	CEGUI::Font::setDefaultResourceGroup("Fonts");
	CEGUI::Scheme::setDefaultResourceGroup("Schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
	CEGUI::WindowManager::setDefaultResourceGroup("Layouts");

	CEGUI::SchemeManager::getSingleton().createFromFile("OgreTray.scheme");
	CEGUI::System::getSingleton().getDefaultGUIContext().setDefaultFont("DejaVuSans-10");
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("OgreTrayImages/MouseArrow");
	CEGUI::System::getSingleton().getDefaultGUIContext().setDefaultTooltipType("OgreTray/Tooltip");

	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window *root = wmgr.loadLayoutFromFile("Root.layout");
	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(root);
	GUIConsole::instance()->setVisible(false);
	GUIFrameTimer::instance()->setVisible(true);
	return true;
}

bool OgreSystem::create()
{
	// Create ogre not specifying any configuration (.cfg) files to read
	ogreRoot_ = new Ogre::Root("", "", S3D::getTempFile("Ogre3D.log"));

	// Load the OpenGL RenderSystem and the SceneManager plugins
	Ogre::LogManager::getSingletonPtr()->logMessage("*** Loading Plugins ***");
	if (!loadPlugin(Ogre::String("RenderSystem_GL"), 
		Ogre::String("GL RenderSystem"))) return false;
	if (!loadPlugin(Ogre::String("Plugin_OctreeSceneManager"), 
		Ogre::String("Octree Scene Manager"))) return false;
	if (!loadPlugin(Ogre::String("Plugin_CgProgramManager"), 
		Ogre::String("Cg Program Manager"))) return false;
	if (!loadPlugin(Ogre::String("ParticleUniverse"), 
		Ogre::String("ParticleUniverse"))) return false;

	// Create the window, initialize openGL
	if (!createWindow()) return false;

	loadResources();

	return true;
}

void OgreSystem::destroyAllAttachedMovableObjects(Ogre::SceneNode* node)
{
   // Destroy all the attached objects
   Ogre::SceneNode::ObjectIterator itObject = node->getAttachedObjectIterator();
   while (itObject.hasMoreElements())
   {
      node->getCreator()->destroyMovableObject(itObject.getNext());
   }

   // Recurse to child SceneNodes
   Ogre::SceneNode::ChildNodeIterator itChild = node->getChildIterator();
   while ( itChild.hasMoreElements() )
   {
      Ogre::SceneNode* pChildNode = static_cast<Ogre::SceneNode*>(itChild.getNext());
      destroyAllAttachedMovableObjects(pChildNode);
   }
}

void OgreSystem::destroySceneNode(Ogre::SceneNode* node)
{
   destroyAllAttachedMovableObjects(node);
   node->removeAndDestroyAllChildren();
   node->getCreator()->destroySceneNode(node);
}
