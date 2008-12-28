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

#include <server/ScorchedServer.h>
#include <server/ServerMain.h>
#include <server/ServerParams.h>
#include <engine/ModDirs.h>
#include <common/OptionsGame.h>
#include <common/ARGParser.h>
#include <common/Defines.h>
#include <common/OptionsTransient.h>
#include <locale.h>
#include <math.h>
#include <signal.h>
#include <float.h>
#include <time.h>
#include <common/main.h>

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "C");

	// From main.h
	run_main(argc, argv, *ServerParams::instance());

	// Start SDL
	unsigned int initFlags = 0;
	if (ServerParams::instance()->getAllowExceptions()) initFlags |= SDL_INIT_NOPARACHUTE;
	if (SDL_Init(initFlags) < 0)
	{
		S3D::dialogExit(
			scorched3dAppName,
			"The SDL library failed to initialize.");
	}

	// Load the server settings file
	if (ServerParams::instance()->getServerCustom())
	{
		ServerParams::instance()->setServerFile(
			S3D::getSettingsFile("server.xml"));
	}
	if (!ServerParams::instance()->getServerFile()[0])
	{
		aParser.showArgs();
		exit(64);
	}
	if (!S3D::fileExists(ServerParams::instance()->getServerFile()))
	{
		S3D::dialogExit(scorched3dAppName, S3D::formatStringBuffer(
			"Server file \"%s\" does not exist.",
			ServerParams::instance()->getServerFile()));
	}
	ScorchedServer::instance()->getOptionsGame().getMainOptions().readOptionsFromFile(
		(char *) ServerParams::instance()->getServerFile());

#ifdef WIN32
	if (ServerParams::instance()->getHideWindow())
	{
		char titleBuffer[MAX_PATH];
		GetConsoleTitle(titleBuffer, MAX_PATH);
		HWND window = FindWindow(NULL, titleBuffer);
		if (window != NULL) 
		{
			ShowWindow(window, SW_HIDE);
			strcat(titleBuffer, " (HIDDEN)");
			SetWindowText(window, titleBuffer);
		}
	}
#endif

	// Run Server
	consoleServer();

	SDL_Quit();
	return 0; // exit(0)
}
