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

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <cstdlib>
#include <common/ARGParser.h>
#include <common/Defines.h>
#include <common/OptionsParameters.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

static ARGParser aParser;
char scorched3dAppName[128];

void _no_storage()
{
	printf("Failed to allocate memory!!");
	std::exit(1);
}

void run_main(int argc, char *argv[], OptionsParameters &params)
{
	std::set_new_handler(&_no_storage);

	// Set the path the executable was run with
	S3D::setExeName((const char *) argv[0]);

	// Generate the version
	snprintf(scorched3dAppName, 128, "Scorched3D - Version %s (%s)", 
		S3D::ScorchedVersion.c_str(), S3D::ScorchedProtocolVersion.c_str());

	srand((unsigned)time(0));

	// Parse command line
	// Read options from command line
	params.addToArgParser(aParser);
	if (!aParser.parse(argc, argv)) exit(64);
	S3D::setSettingsDir(params.getSettingsDir());

	// Check we are in the correct directory
	if (!S3D::dirExists(S3D::getDataFile("data")))
	{
		std::string cwd = S3D::getCWD();
		std::string dataDir = S3D::getDataFile("");

		// Perhaps we can get the directory from the executables path name
		std::string path = argv[0];
		S3D::fileDos2Unix(path);
		size_t slashPos = path.rfind('/');
		if (slashPos != std::string::npos)
		{
			path.erase(slashPos);
			S3D::setCWD(path);
		}

		// Now try again for the correct directory
		if (!S3D::dirExists(S3D::getDataFile("data")))
		{	
			S3D::dialogExit(
				scorched3dAppName, S3D::formatStringBuffer(
				"Error: This game requires the Scorched3D data directory to run.\n"
				"Your machine does not appear to have the Scorched3D data directory in\n"
				"the required location.\n\n"
				"Checked path, current working directory : %s\n"
				"Checked path, binary directory : %s\n"
				"Checked path, data directory : %s\n\n"
				"If Scorched3D does not run please re-install Scorched3D.",
				cwd.c_str(), path.c_str(), dataDir.c_str()));
		}
	}

	// Check that the mods are uptodate with the current scorched3d
	// version
	//ModDirs dirs;
	//dirs.loadModDirs();

#ifndef _WIN32
	// Tells Linux not to issue a sig pipe when writting to a closed socket
	// Why does it have to be dificult!
	signal(SIGPIPE, SIG_IGN);
	signal(SIGFPE, SIG_IGN);
#endif

	if (setlocale(LC_ALL, "C") == 0)
	{
		S3D::dialogMessage(
			scorched3dAppName,
			"Warning: Failed to set client locale");
	}
}
