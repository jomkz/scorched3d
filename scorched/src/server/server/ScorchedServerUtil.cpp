////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
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

#include <server/ScorchedServerUtil.h>
#include <server/ScorchedServer.h>
#include <server/ServerAuthHandlerForumLogin.h>
#include <server/ServerAuthHandlerPrefered.h>
#include <server/ServerAuthHandlerMinKills.h>
#include <server/ServerAuthHandlerDefault.h>
#include <common/OptionsScorched.h>
#include <common/Logger.h>

ScorchedServerUtil *ScorchedServerUtil::instance_ = 0;

ScorchedServerUtil *ScorchedServerUtil::instance()
{
	if (!instance_) instance_ = new ScorchedServerUtil;
	return instance_;
}

ScorchedServerUtil::ScorchedServerUtil() : authHandler_(0)
{
}

ScorchedServerUtil::~ScorchedServerUtil()
{
}

ServerAuthHandler *ScorchedServerUtil::getAuthHandler()
{
	if (authHandler_) return authHandler_;

	const char *handler = ScorchedServer::instance()->getOptionsGame().getAuthHandler();
	if (0 == strcmp("none", handler))
	{
		authHandler_ = new ServerAuthHandlerDefault;
	} 
	else if (0 == strcmp("prefered", handler))
	{
		authHandler_ = new ServerAuthHandlerPrefered;
	}
#ifdef HAVE_MYSQL
	else if (0 == strcmp("forumlogin", handler))
	{
		authHandler_ = new ServerAuthHandlerForumLogin;
	}
#endif
	else if (0 == strcmp("minkills", handler))
	{
		authHandler_ = new ServerAuthHandlerMinKills;
	}
	else 
	{
		S3D::dialogExit("ServerAuthHandler", 
			S3D::formatStringBuffer("Unknown auth handler \"%s\"", handler));
	}
	
	Logger::log(S3D::formatStringBuffer("Using \"%s\" authentication handler.", handler));
	return authHandler_;
}
