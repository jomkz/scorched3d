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

#include <client/ClientParams.h>

ClientParams *ClientParams::instance_ = 0;

ClientParams *ClientParams::instance()
{
	if (!instance_)
	{
		instance_ = new ClientParams;
	}

	return instance_;
}

ClientParams::ClientParams() :
	OptionsParameters("ClientParams", "The command line options for the Scorched3D client"),
	connect_("The name of the server to connect to, starts a NET/LAN client", 0, ""),
	connectAcceptDefaults_("Connect to the server without prompting for player name and tank selection", 0, false),
	client_("Starts a Scorched3D client, requires the name of the client settings file e.g. data/singlecustom.xml", 0, ""),
	startcustom_("Starts a Scorched3D client, Uses the last custom game made", 0, false),
	save_("Continues a Scorched3D client game, requires the name of the saved game.", 0, ""),
	exittime_("The time after which the client will exit", 0, 0),
	disconnecttime_("The time after which the client will disconnect from the server", 0, 0),
	username_("The username of the NET/LAN server", 0, ""),
	password_("The password of the NET/LAN server", 0, ""),
	nonParam_(".3dl Scorched3D online gaming launching config", 0, ""),
	documentationLocation_("Generate the Scorched3D documentation into the given directory location")
{
	addChildXMLEntry("connect", &connect_, "connectwithdefaults", &connectAcceptDefaults_);
	addChildXMLEntry("startclient", &client_, "startcustom", &startcustom_, "loadsave", &save_);
	addChildXMLEntry("exittime", &exittime_, "disconnecttime", &disconnecttime_);
	addChildXMLEntry("username", &username_, "password", &password_);
	addChildXMLEntry("launchfile", &nonParam_);
	addChildXMLEntry("generatedocumentation", &documentationLocation_);
}

ClientParams::~ClientParams()
{
	
}

void ClientParams::reset()
{
	connectAcceptDefaults_.resetDefaultValue();
	connect_.resetDefaultValue();
	client_.resetDefaultValue();
	startcustom_.resetDefaultValue();;
	save_.resetDefaultValue();
	username_.resetDefaultValue();
	password_.resetDefaultValue();
	exittime_.resetDefaultValue();
}
