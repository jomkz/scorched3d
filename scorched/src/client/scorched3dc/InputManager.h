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

#if !defined(__INCLUDE_InputManagerh_INCLUDE__)
#define __INCLUDE_InputManagerh_INCLUDE__

#include <list>

class InputHandlerMouse;
class InputManager :
	public Ogre::FrameListener, 
	public Ogre::WindowEventListener, 
	public OIS::KeyListener, 
	public OIS::MouseListener
{
public:
	InputManager();
	virtual ~InputManager();

	void create(Ogre::Root *ogreRoot, Ogre::RenderWindow* ogreWindow);

	void addMouseHandler(InputHandlerMouse *handler);
	void removeMouseHandler(InputHandlerMouse *handler);

	bool isKeyDown(CEGUI::Key::Scan key);

	// Ogre::FrameListener
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

	// OIS::KeyListener
	virtual bool keyPressed(const OIS::KeyEvent &arg);
	virtual bool keyReleased(const OIS::KeyEvent &arg);

	// OIS::MouseListener
	virtual bool mouseMoved(const OIS::MouseEvent &arg);
	virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	// Ogre::WindowEventListener
	virtual void windowResized(Ogre::RenderWindow* rw);
	virtual void windowClosed(Ogre::RenderWindow* rw);	

protected:
	bool *keyDownState_;

	// OIS Input devices
	OIS::InputManager* inputManager_;
	OIS::Mouse* mouse_;
	OIS::Keyboard* keyboard_;
	
	// Mouse handling
	std::list<InputHandlerMouse *> mouseHandlers_;
	int mouseDownButton_, mouseDownX_, mouseDownY_;
	bool mouseDragging_;
};

#endif // __INCLUDE_InputManagerh_INCLUDE__
