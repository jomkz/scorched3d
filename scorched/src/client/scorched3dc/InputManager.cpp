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

#include <scorched3dc/InputManager.h>
#include <scorched3dc/InputHandlerMouse.h>

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

InputManager::InputManager() :
	inputManager_(0), mouse_(0), keyboard_(0),
	mouseDownButton_(-1), mouseDragging_(false)
{
	keyDownState_ = new bool[256];
	memset(keyDownState_, false, sizeof(bool) * 256);
}

InputManager::~InputManager()
{
	windowClosed(0);
	delete [] keyDownState_;
	//Ogre::WindowEventUtilities::removeWindowEventListener(ogreWindow_, this);
}

void InputManager::create(Ogre::Root *ogreRoot, Ogre::RenderWindow* ogreWindow)
{
	Ogre::LogManager::getSingletonPtr()->logMessage("*** Creating Input ***");
	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;
 
	ogreWindow->getCustomAttribute("WINDOW", &windowHnd);
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
	windowResized(ogreWindow);
 
	//Register as a Window listener
	Ogre::WindowEventUtilities::addWindowEventListener(ogreWindow, this);
	ogreRoot->addFrameListener(this);
}

bool InputManager::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	// Need to capture/update each device
	keyboard_->capture();
	mouse_->capture();

	// Allow CEGUI to know about the passing of time
	if (CEGUI::System::getSingletonPtr())
	{
		CEGUI::System::getSingleton().injectTimePulse(evt.timeSinceLastFrame);
		CEGUI::System::getSingleton().getDefaultGUIContext().injectTimePulse(evt.timeSinceLastFrame);
	}

	return true;
}

bool InputManager::keyPressed(const OIS::KeyEvent &arg)
{
	CEGUI::System &sys = CEGUI::System::getSingleton();
	if (!sys.getDefaultGUIContext().injectKeyDown((CEGUI::Key::Scan) arg.key) &&
		!sys.getDefaultGUIContext().injectChar(arg.text))
	{
		if (arg.key < 255) keyDownState_[arg.key] = true;
	}
	return true;
}

bool InputManager::keyReleased(const OIS::KeyEvent &arg)
{
	if (!CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp((CEGUI::Key::Scan) arg.key))
	{
		if (arg.key < 255) keyDownState_[arg.key] = false;
	}
	return true;
}

bool InputManager::isKeyDown(CEGUI::Key::Scan key)
{
	if (key < 255) return keyDownState_[key];
	return false;
}

bool InputManager::mouseMoved(const OIS::MouseEvent &arg)
{
	if (arg.state.Z.rel) 
	{
		CEGUI::System &sys = CEGUI::System::getSingleton();
		if (!sys.getDefaultGUIContext().injectMouseWheelChange(arg.state.Z.rel / 120.0f))
		{
			std::list<InputHandlerMouse *>::iterator itor = mouseHandlers_.begin();
			std::list<InputHandlerMouse *>::iterator endItor = mouseHandlers_.end();
			for (;itor != endItor; ++itor)
			{
				(*itor)->mouseWheel(arg.state.Z.rel);
			}
		}
	}

	if (mouseDownButton_ == -1)
	{
		CEGUI::System &sys = CEGUI::System::getSingleton();
		sys.getDefaultGUIContext().injectMousePosition((float) arg.state.X.abs, (float) arg.state.Y.abs);
	}
	else
	{
		int dx = arg.state.X.abs - mouseDownX_;
		int dy = arg.state.Y.abs - mouseDownY_;
		if (mouseDragging_ || abs(dx) > 5 || abs(dy) > 5)
		{
			mouseDragging_ = true;
			std::list<InputHandlerMouse *>::iterator itor = mouseHandlers_.begin();
			std::list<InputHandlerMouse *>::iterator endItor = mouseHandlers_.end();
			for (;itor != endItor; ++itor)
			{
				(*itor)->mouseDrag(arg.state.X.abs, arg.state.Y.abs, dx, dy, mouseDownButton_);
			}
			mouseDownX_ = arg.state.X.abs;
			mouseDownY_ = arg.state.Y.abs;
		}
	}
	return true;
}

bool InputManager::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mouseDownButton_ == -1)
	{
		CEGUI::System &sys = CEGUI::System::getSingleton();
		if (!sys.getDefaultGUIContext().injectMouseButtonDown(convertButton(id)))
		{
			mouseDownButton_ = (int) id;
			mouseDownX_ = arg.state.X.abs;
			mouseDownY_ = arg.state.Y.abs;
		}
	}
	return true;
}

bool InputManager::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mouseDownButton_ != -1)
	{
		std::list<InputHandlerMouse *>::iterator itor = mouseHandlers_.begin();
		std::list<InputHandlerMouse *>::iterator endItor = mouseHandlers_.end();
		for (;itor != endItor; ++itor)
		{
			(*itor)->mouseClick(arg.state.X.abs, arg.state.Y.abs, (int) id);
		}
	}
	else
	{
		CEGUI::System &sys = CEGUI::System::getSingleton();
		sys.getDefaultGUIContext().injectMouseButtonUp(convertButton(id));
	}
	mouseDownButton_ = -1;
	mouseDragging_ = false;
	return true;
}

void InputManager::windowResized(Ogre::RenderWindow* rw)
{
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState &ms = mouse_->getMouseState();
	ms.width = width;
	ms.height = height;

	if (CEGUI::System::getSingletonPtr()) 
	{
		CEGUI::System::getSingleton().notifyDisplaySizeChanged(CEGUI::Sizef((float) width, (float) height));
	}
}

void InputManager::windowClosed(Ogre::RenderWindow* rw)
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

void InputManager::addMouseHandler(InputHandlerMouse *handler)
{
	mouseHandlers_.push_back(handler);
}

void InputManager::removeMouseHandler(InputHandlerMouse *handler)
{
	mouseHandlers_.remove(handler);
}
