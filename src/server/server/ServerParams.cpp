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

#include <server/ServerParams.h>

ServerParams *ServerParams::instance_ = 0;

ServerParams *ServerParams::instance()
{
	if (!instance_)
	{
		instance_ = new ServerParams;
	}

	return instance_;
}

ServerParams::ServerParams() :
	server_(options_, "startserver",
		"Starts a scorched 3d server, requires the name of the server settings file e.g. data/server.xml", 0, ""),
	hideWindow_(options_, "hidewindow",
		"Hides the scorched3d console window (windows)", 0, false)
{

}

ServerParams::~ServerParams()
{
	
}
