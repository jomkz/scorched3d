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

#if !defined(__INCLUDE_Scorched3DCh_INCLUDE__)
#define __INCLUDE_Scorched3DCh_INCLUDE__

class Scorched3DC : 
	public Ogre::FrameListener, 
	public Ogre::WindowEventListener, 
	public OIS::KeyListener, 
	public OIS::MouseListener
{
public:
	Scorched3DC();
	virtual ~Scorched3DC();

	static Scorched3DC *instance();

	bool go();

	Ogre::Root *getOgreRoot() { return ogreRoot_; }
	Ogre::RenderWindow *getOgreRenderWindow() { return ogreWindow_; }
	OIS::Keyboard* getKeyboard() { return keyboard_; }

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
	static Scorched3DC *instance_;
	bool quit_;
	Ogre::Root *ogreRoot_;
	Ogre::RenderWindow* ogreWindow_;

	// CEGUI
	CEGUI::OgreRenderer* guiRenderer_;

	// OIS Input devices
	OIS::InputManager* inputManager_;
	OIS::Mouse* mouse_;
	OIS::Keyboard* keyboard_;

	bool loadPlugin(const Ogre::String &pluginName, const Ogre::String &requiredName);
	bool createWindow();
	bool createInput();
	bool createUI();
	void loadResources();
};

#endif // __INCLUDE_Scorched3DCh_INCLUDE__