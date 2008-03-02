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
#include <tank/TankDeadContainer.h>
#include <tank/TankContainer.h>
#include <tankai/TankAIStore.h>
#include <landscapedef/LandscapeDefinitions.h>

#ifndef S3D_SERVER
#include <client/ClientParams.h>
#include <client/ScorchedClient.h>
#endif

ScorchedServer *ScorchedServer::instance_ = 0;

ScorchedServer *ScorchedServer::instance()
{
	if (!instance_)
	{
		instance_ = new ScorchedServer;
	}

#ifndef S3D_SERVER
	if (ClientParams::instance()->getConnectedToServer() &&
		ScorchedClient::instance()->getTankContainer().getCurrentDestinationId() != 0)
	{
		DIALOG_ASSERT(0);
	}
#endif

	return instance_;
}

ScorchedServer::ScorchedServer()  : context_("Server")
{
	deadContainer_ = new TankDeadContainer;
	context_.serverMode = true;
	tankAIStore_ = new TankAIStore;
}

ScorchedServer::~ScorchedServer()
{
	delete deadContainer_;
}

