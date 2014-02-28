#pragma once

#include "StdAfx.h"
#include <engine/ThreadCallback.h>

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

	ThreadCallback &getUIThreadCallback() { return uiThreadCallback_; }

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
	ThreadCallback uiThreadCallback_;
	bool quit_;
	Ogre::Root *ogreRoot_;
	Ogre::RenderWindow* ogreWindow_;
	Ogre::Camera* camera_;
	Ogre::SceneManager* sceneMgr_;

	// CEGUI
	CEGUI::OgreRenderer* guiRenderer_;

	// OIS Input devices
	OIS::InputManager* inputManager_;
	OIS::Mouse* mouse_;
	OIS::Keyboard* keyboard_;

	bool quit(const CEGUI::EventArgs &e);
	bool start(const CEGUI::EventArgs &e);
	bool join(const CEGUI::EventArgs &e);
	bool loadPlugin(const Ogre::String &pluginName, const Ogre::String &requiredName);
	bool createWindow();
	bool createInput();
	bool createUI();
	void loadResources();
	void createScene();
};
