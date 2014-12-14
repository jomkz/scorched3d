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

#if !defined(__INCLUDE_MiniLandscapeh_INCLUDE__)
#define __INCLUDE_MiniLandscapeh_INCLUDE__

#include <uistate/UIStateI.h>
#include <client/ClientUISync.h>

class MiniLandscape
{
public:
	static MiniLandscape *instance();

	void create();
	void update();
	CEGUI::BasicImage *createGUITexture();

	Ogre::RenderTarget *getTexture() { return renderTexture_; }

protected:
	int x_size, z_size;
	Ogre::Real half_x_size, half_z_size;
	Ogre::SceneManager* landscapeSceneManager_;
	Ogre::Camera *landscapeCamera_;
	Ogre::RenderTarget *renderTexture_;
	Ogre::ManualObject *miniLandscape_;
	Ogre::SceneNode *waterNode_;
	Ogre::TexturePtr rtt_texture_; 

private: 
	MiniLandscape();
	virtual ~MiniLandscape();
};

#endif // __INCLUDE_MiniLandscapeh_INCLUDE__
