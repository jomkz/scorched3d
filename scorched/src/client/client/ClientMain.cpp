////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011
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

#include <client/ClientMain.h>
#include <client/ClientState.h>
#include <client/ScorchedClient.h>
#include <client/ClientOptions.h>
#include <net/NetInterface.h>
#include <common/ProgressCounter.h>
#include <common/Logger.h>
#include <common/Defines.h>
#include <lang/LangResource.h>


bool ClientMain::clientMain()
{
	Logger::log(S3D::formatStringBuffer("Scorched3D - Version %s (%s) - %s",
		S3D::ScorchedVersion.c_str(), 
		S3D::ScorchedProtocolVersion.c_str(), 
		S3D::ScorchedBuildTime.c_str()));

	ProgressCounter progressCounter;
	
	// Start the initial windows
	/*
	ClientState::setupGameState();
	
	
	ProgressDialog::instance()->changeTip();
	progressCounter.setUser(ProgressDialogSync::events_instance());
	progressCounter.setNewPercentage(0.0f);

	if (!initWindows(&progressCounter)) return false;
	

	OptionsDisplayConsole::instance()->addDisplayToConsole();
	*/


	// Run the main loop
	ScorchedClient::instance()->getClientState().clientMainLoop();

	// When we get here we have exited the loop and are now shutting down
	if (ScorchedClient::instance()->getContext().getNetInterfaceValid())
	{
		ScorchedClient::instance()->getNetInterface().disconnectAllClients();
	}
	Lang::instance()->saveUndefined();

	return true;
}

