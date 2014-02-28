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

#include <server/ServerConsoleProgressCounter.h>
#include <server/ServerConsoleLogger.h>
#include <server/ServerCommon.h>
#include <server/ServerParams.h>
#include <server/ScorchedServer.h>
#include <server/ScorchedServerSettings.h>
#include <common/Logger.h>

void consoleServer()
{
	ServerConsoleProgressCounter::instance();
	ServerConsoleLogger serverConsoleLogger;
	ServerCommon::startFileLogger(ServerParams::instance()->getServerFile());

	Logger::log(S3D::formatStringBuffer("Scorched3D - Version %s (%s) - %s",
		S3D::ScorchedVersion.c_str(), 
		S3D::ScorchedProtocolVersion.c_str(), 
		S3D::ScorchedBuildTime.c_str()));

	ScorchedServerSettingsOptions *settings =
		new ScorchedServerSettingsOptions(
		ServerParams::instance()->getServerFile(),
		ServerParams::instance()->getRewriteOptions(),
		ServerParams::instance()->getWriteFullOptions());
	ScorchedServer::instance()->startServer(settings, ServerConsoleProgressCounter::instance()->getProgressCounter(), 0);
	for (;;)
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
	}
}
