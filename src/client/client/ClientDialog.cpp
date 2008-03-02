////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
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
//    You should have received a copy of the GNU General Public License
//    along with Scorched3D; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

#include <client/ClientDialog.h>
#include <graph/GLSetup.h>
#include <graph/MainCamera.h>
#include <graph/Main2DCamera.h>
#include <graph/OptionsDisplay.h>
#include <graph/Display.h>
#include <graph/Gamma.h>
#include <client/ClientParams.h>
#include <common/OptionsTransient.h>
#include <common/OptionsScorched.h>
#include <common/Defines.h>
#include <stdio.h>
#include <time.h>

extern char scorched3dAppName[128];

bool createScorchedWindow()
{
	int width = OptionsDisplay::instance()->getScreenWidth();
	int height = OptionsDisplay::instance()->getScreenHeight();
	bool fullscreen = OptionsDisplay::instance()->getFullScreen();

	SDL_WM_SetCaption(scorched3dAppName, "tank2");
	std::string iconFile = S3D::getDataFile("data/windows/tank2.bmp");
	SDL_WM_SetIcon(SDL_LoadBMP(iconFile.c_str()), NULL);

	if (!Display::instance()->changeSettings(width,height,fullscreen)) 
	{
		S3D::dialogMessage("Scorched 3D Display", 
			"ERROR: Failed to set the display mode.\n"
			"Ensure that no other application is exclusively using the graphics hardware.\n"
			"Ensure that the current desktop mode has at least 24 bits colour depth.\n");
		return false;
	}

	int windowWidth = 1024;
	int windowHeight = 768;
	switch (OptionsDisplay::instance()->getDialogSize())
	{
	case 0:
		windowWidth = 1600;
		break;
	case 1:
		windowWidth = 1152;
		break;
	case 3:
		windowWidth = 800;
		break;
	}
	windowHeight = (height * windowWidth) / width;

	MainCamera::instance()->getCamera().setWindowSize(width, height);
	Main2DCamera::instance()->getViewPort().setWindowSize(windowWidth, windowHeight, 
		width, height);

	Gamma::instance()->save();
	Gamma::instance()->set();

	GLSetup::setup();
	/*if (!OptionsDisplay::instance()->getNoCg())
	{
		if (!CGLoader::instance()->init())
		{
			return false;
		}
	}*/

	return true;
}
