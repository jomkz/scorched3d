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

#if !defined(__INCLUDE_UIStatePlayingh_INCLUDE__)
#define __INCLUDE_UIStatePlayingh_INCLUDE__

#include <scorched3dc/UIStateI.h>
#include <SkyX/SkyX.h>
#include <Hydrax/Hydrax.h>
#include <Hydrax/Noise/Perlin/Perlin.h>
#include <Hydrax/Modules/ProjectedGrid/ProjectedGrid.h>

class UIStatePlaying : public UIStateI
{
public:
	UIStatePlaying();
	virtual ~UIStatePlaying();

	virtual void createState();
	virtual void destroyState();

protected:
	SkyX::ColorGradient waterGradient_, sunGradient_, ambientGradient_;
	Ogre::Camera* camera_;
	Ogre::SceneManager* sceneMgr_;
	SkyX::SkyX *skyX_;
	Hydrax::Hydrax *hydraX_;

	void updateLighting();
};

#endif // __INCLUDE_UIStatePlayingh_INCLUDE__
