#include "StdAfx.h"
#include "Scorched3DC.h"

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

Scorched3DC::Scorched3DC() : 
	ogreRoot_(0), ogreWindow_(0),
	inputManager_(0), mouse_(0), keyboard_(0),
	guiRenderer_(0), sceneMgr_(0), camera_(0),
	quit_(false)
{

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
}

bool Scorched3DC::loadPlugin(const Ogre::String &pluginName, const Ogre::String &requiredName)
{
	Ogre::LogManager::getSingletonPtr()->logMessage("*** Loading Plugins ***");

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
		if ((*k)->getName() == requiredName)
		{
			found = true;
			break;
		}
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
		return false; //No RenderSystem found
	}

	// configure our RenderSystem
	rs->setConfigOption("Full Screen", "No");
	rs->setConfigOption("VSync", "No");
	rs->setConfigOption("Video Mode", "1024 x 768 @ 32-bit");
	ogreRoot_->setRenderSystem(rs);

	ogreWindow_ = ogreRoot_->initialise(true, "Scorched3D");

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
	// setup resources
	// Only add the minimally required resource locations to load up SDKTrays and the Ogre head mesh
    /*
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("I:\\OgreSDK_vc10_v1-8-1\\media\\materials\\scripts", "FileSystem", "General");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("I:\\OgreSDK_vc10_v1-8-1\\media\\materials\\textures", "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("I:\\OgreSDK_vc10_v1-8-1\\media\\materials\\programs", "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("I:\\OgreSDK_vc10_v1-8-1\\media\\models", "FileSystem", "General");
	*/

	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("data/cegui/imagesets", "FileSystem", "Imagesets");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("data/cegui/fonts", "FileSystem", "Fonts");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("data/cegui/schemes", "FileSystem", "Schemes");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("data/cegui/looknfeel", "FileSystem", "LookNFeel");

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

bool Scorched3DC::quit(const CEGUI::EventArgs &e)
{
	quit_ = true;
	return true;
}

void Scorched3DC::createScene()
{
	// Create scene manager
	sceneMgr_ = ogreRoot_->createSceneManager(Ogre::ST_GENERIC);

	// Create the camera
	camera_ = sceneMgr_->createCamera("PlayerCam");

	// Position it at 500 in Z direction
	camera_->setPosition(Ogre::Vector3(0,0,80));
	// Look back along -Z
	camera_->lookAt(Ogre::Vector3(0,0,-300));
	camera_->setNearClipDistance(5);

	// Create one viewport, entire window
	Ogre::Viewport* vp = ogreWindow_->addViewport(camera_);
	vp->setBackgroundColour(Ogre::ColourValue(0,0,0));

	// Alter the camera aspect ratio to match the viewport
	camera_->setAspectRatio(
		Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

	// Create head
	//Ogre::Entity* ogreHead = sceneMgr_->createEntity("Head", "ogrehead.mesh");
    //Ogre::SceneNode* headNode = sceneMgr_->getRootSceneNode()->createChildSceneNode();
    //headNode->attachObject(ogreHead);

    // Set ambient light
    sceneMgr_->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));

    // Create a light
    Ogre::Light* l = sceneMgr_->createLight("MainLight");
    l->setPosition(20,80,50);

	// Window
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window *sheet = wmgr.createWindow("DefaultWindow", "CEGUIDemo/Sheet");

	CEGUI::Window *quit = wmgr.createWindow("OgreTray/Button", "CEGUIDemo/QuitButton");
	quit->setText("Quit");
	quit->setSize(CEGUI::UVector2(CEGUI::UDim(0.15f, 0.0f), CEGUI::UDim(0.05f, 0.0f)));
	quit->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Scorched3DC::quit, this));

	sheet->addChildWindow(quit);
	CEGUI::System::getSingleton().setGUISheet(sheet);
}

bool Scorched3DC::go()
{
	// Create ogre not specifying any configuration (.cfg) files to read
	ogreRoot_ = new Ogre::Root("", "");

	// Load the OpenGL RenderSystem and the SceneManager plugins
	if (!loadPlugin(Ogre::String("RenderSystem_GL"), 
		Ogre::String("GL RenderSystem"))) return false;
	if (!loadPlugin(Ogre::String("Plugin_OctreeSceneManager"), 
		Ogre::String("Octree Scene Manager"))) return false;

	// Create the window, initialize openGL
	if (!createWindow()) return false;

	// Connect to the keyboard and mouse inputs
	if (!createInput()) return false;

	loadResources();

	// Create the CEGUI UI
	if (!createUI()) return false;

	createScene();

	while(!quit_)
	{
		// Pump window messages for nice behaviour
		Ogre::WindowEventUtilities::messagePump();
 
		if(ogreWindow_->isClosed())
		{
			return false;
		}
 
		// Render a frame
		if(!ogreRoot_->renderOneFrame()) 
		{
			return false;
		}
	}

	return true;
}
