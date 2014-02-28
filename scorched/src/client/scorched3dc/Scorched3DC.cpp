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

#include <scorched3dc/Scorched3DC.h>
#include <scorched3dc/UIState.h>

#include <common/Logger.h>
#include <common/FileLogger.h>
#include <lang/Lang.h>

static CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID)
{
	switch (buttonID)
	{
		case OIS::MB_Left: return CEGUI::LeftButton;
		case OIS::MB_Right: return CEGUI::RightButton;
		case OIS::MB_Middle: return CEGUI::MiddleButton;
		default: return CEGUI::LeftButton;
	}
}

Scorched3DC *Scorched3DC::instance_(0);

Scorched3DC *Scorched3DC::instance()
{
	return instance_;
}

Scorched3DC::Scorched3DC() : 
	ogreRoot_(0), ogreWindow_(0),
	inputManager_(0), mouse_(0), keyboard_(0),
	guiRenderer_(0),
	quit_(false)
{
	instance_ = this;
}

Scorched3DC::~Scorched3DC()
{
	if (ogreWindow_)
	{
		Ogre::WindowEventUtilities::removeWindowEventListener(ogreWindow_, this);
	}
	windowClosed(ogreWindow_);
	delete ogreRoot_;
	ogreRoot_ = 0;
	instance_ = 0;
}

bool Scorched3DC::loadPlugin(const Ogre::String &pluginName, const Ogre::String &requiredName)
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

bool Scorched3DC::createWindow()
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

bool Scorched3DC::createInput()
{
	Ogre::LogManager::getSingletonPtr()->logMessage("*** Creating Input ***");
	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;
 
	ogreWindow_->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
 
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
	pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
	pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
	pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
	pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
	pl.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
	pl.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
#endif
	inputManager_ = OIS::InputManager::createInputSystem( pl );
 
	keyboard_ = static_cast<OIS::Keyboard*>(inputManager_->createInputObject( OIS::OISKeyboard, true ));
	mouse_ = static_cast<OIS::Mouse*>(inputManager_->createInputObject( OIS::OISMouse, true ));
 
	keyboard_->setEventCallback(this);
	mouse_->setEventCallback(this);
 
	//Set initial mouse clipping size
	windowResized(ogreWindow_);
 
	//Register as a Window listener
	Ogre::WindowEventUtilities::addWindowEventListener(ogreWindow_, this);
 
	ogreRoot_->addFrameListener(this);
	return true;
}

bool Scorched3DC::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if(ogreWindow_->isClosed()) return false;

	// Need to capture/update each device
	keyboard_->capture();
	mouse_->capture();

	return true;
}

bool Scorched3DC::keyPressed( const OIS::KeyEvent &arg )
{
	CEGUI::System &sys = CEGUI::System::getSingleton();
	sys.injectKeyDown(arg.key);
	sys.injectChar(arg.text);
	return true;
}

bool Scorched3DC::keyReleased( const OIS::KeyEvent &arg )
{
	CEGUI::System::getSingleton().injectKeyUp(arg.key);
	return true;
}

bool Scorched3DC::mouseMoved( const OIS::MouseEvent &arg )
{
	CEGUI::System &sys = CEGUI::System::getSingleton();
	sys.injectMousePosition((float) arg.state.X.abs, (float) arg.state.Y.abs);
	// Scroll wheel.
	if (arg.state.Z.rel) sys.injectMouseWheelChange(arg.state.Z.rel / 120.0f);
	return true;
}

bool Scorched3DC::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	CEGUI::System::getSingleton().injectMouseButtonDown(convertButton(id));
	return true;
}

bool Scorched3DC::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	CEGUI::System::getSingleton().injectMouseButtonUp(convertButton(id));
	return true;
}

void Scorched3DC::windowResized(Ogre::RenderWindow* rw)
{
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState &ms = mouse_->getMouseState();
	ms.width = width;
	ms.height = height;

	if (guiRenderer_) 
	{
		CEGUI::System::getSingleton().notifyDisplaySizeChanged(CEGUI::Size((float) width, (float) height));
	}
}

void Scorched3DC::windowClosed(Ogre::RenderWindow* rw)
{
    if(inputManager_)
    {
        if (mouse_) inputManager_->destroyInputObject(mouse_);
        if (keyboard_) inputManager_->destroyInputObject(keyboard_);

        OIS::InputManager::destroyInputSystem(inputManager_);
        inputManager_ = 0;
		mouse_ = 0;
		keyboard_ = 0;
    }
}

void Scorched3DC::loadResources()
{
	// HydraX
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("data/hydrax", "FileSystem", "Hydrax");

	// CEGUI
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("data/cegui/imagesets", "FileSystem", "Imagesets");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("data/cegui/fonts", "FileSystem", "Fonts");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("data/cegui/schemes", "FileSystem", "Schemes");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("data/cegui/looknfeel", "FileSystem", "LookNFeel");

	// SkyX
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("data/skyx", "FileSystem", "SkyX");

	// load resources
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

bool Scorched3DC::createUI()
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

bool Scorched3DC::go()
{
	// Setup S3D logger
	Logger::addLogger(new FileLogger("Scorched3D.log", ".", false), false);
	Logger::log(S3D::formatStringBuffer("Scorched3D - Version %s (%s) - %s",
		S3D::ScorchedVersion.c_str(), 
		S3D::ScorchedProtocolVersion.c_str(), 
		S3D::ScorchedBuildTime.c_str()));

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

	// Connect to the keyboard and mouse inputs
	if (!createInput()) return false;

	loadResources();

	// Create the CEGUI UI
	if (!createUI()) return false;

	// Create the first scene
	UIState::instance()->setState(UIState::StatePlaying);//UIState::StateMainMenu);

	while(!quit_)
	{
		// Pump window messages for nice behaviour
		Ogre::WindowEventUtilities::messagePump();
 
		if(ogreWindow_->isClosed())
		{
			return false;
		}

		// Update UI
		UIState::instance()->getUIThreadCallback().processCallbacks();
 
		// Render a frame
		if(!ogreRoot_->renderOneFrame()) 
		{
			return false;
		}
	}

	Lang::instance()->saveUndefined();

	return true;
}
