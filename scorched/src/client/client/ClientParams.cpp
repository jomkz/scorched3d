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
	OptionsParameters("options", "The command line options for the Scorched3D client"),
	connect_("connect", 
		"The name of the server to connect to, starts a NET/LAN client", 0, ""),
	connectAcceptDefaults_("connectwithdefaults", 
		"Connect to the server without prompting for player name and tank selection", 0, false),
	client_("startclient",
		"Starts a scorched 3d client, requires the name of the client settings file e.g. data/singlecustom.xml", 0, ""),
	startcustom_("startcustom",
		"Starts a scorched 3d client, Uses the last custom game made", 0, false),
	save_("loadsave",
		"Continues a scorched 3d client game, requires the name of the saved game.", 0, ""),
	exittime_("exittime",
		"The time after which the client will exit", 0, 0),
	disconnecttime_("disconnecttime",
		"The time after which the client will disconnect from the server", 0, 0),
	username_("username",
		"The username of the NET/LAN server", 0, ""),
	password_("password",
		"The password of the NET/LAN server", 0, ""),
	nonParam_("launch file", 
		".3dl scorched3d online gaming launching config", 0, "")
{
	addChildXMLEntry(&connect_, &connectAcceptDefaults_);
	addChildXMLEntry(&client_, &startcustom_, &save_);
	addChildXMLEntry(&exittime_, &disconnecttime_);
	addChildXMLEntry(&username_, &password_);
	addChildXMLEntry(&nonParam_);
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
