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

#include <common/StatsLoggerMySQL.h>
#include <common/StatsLoggerPGSQL.h>
#include <common/StatsLoggerFile.h>
#include <common/OptionsScorched.h>
#include <common/Logger.h>
#include <server/ScorchedServer.h>
#include <stdlib.h>

StatsLogger *StatsLogger::instance_ = 0;

StatsLogger *StatsLogger::instance()
{
#ifndef S3D_SERVER
	if (!instance_) instance_ = new StatsLoggerNone;
#endif

	if (!instance_)
	{
		const char *statsLogger =
			ScorchedServer::instance()->getOptionsGame().getStatsLogger();
		if (strcmp(statsLogger, "mysql") == 0)
		{
#ifdef HAVE_MYSQL
			instance_ = new StatsLoggerMySQL();
			Logger::log( "Created mysql stats logger.");
#else
			S3D::dialogExit("StatsLogger",
				"Atempted to create mysql stats logger\n"
				"but mysql support has not been compiled into this\n"
				"scorched3d binary.");
#endif
			if (strcmp(ScorchedServer::instance()->getOptionsGame().getPublishAddress(),
						"AutoDetect") == 0)
			{
				S3D::dialogExit("StatsLogger",
					"Stats logging enabled but AutoDetect used for server address");
			}
		}
                else if (strcmp(statsLogger, "pgsql") == 0)
		{
#ifdef HAVE_PGSQL
			instance_ = new StatsLoggerPGSQL;
			Logger::log( "Created pgsql stats logger.");
#else
			S3D::dialogExit("StatsLogger",
				"Atempted to create pgsql stats logger\n"
				"but pgsql support has not been compiled into this\n"
				"scorched3d binary.");
#endif
			if (strcmp(ScorchedServer::instance()->getOptionsGame().getPublishAddress(),
						"AutoDetect") == 0)
			{
				S3D::dialogExit("StatsLogger",
					"Stats logging enabled but AutoDetect used for server address");
			}
		}
		else if (strcmp(statsLogger, "file") == 0)
		{
			Logger::log( "Created file stats logger.");
			instance_ = new StatsLoggerFile;
			
			if (strcmp(ScorchedServer::instance()->getOptionsGame().getPublishAddress(),
						"AutoDetect") == 0)
			{
				S3D::dialogExit("StatsLogger",
					"Stats logging enabled but AutoDetect used for server address");
			}
		}
		else
		{
			Logger::log( "Created null stats logger.");
			instance_ = new StatsLoggerNone;
		}
	}
	return instance_;
}

StatsLogger::StatsLogger()
{

}

StatsLogger::~StatsLogger()
{
}

