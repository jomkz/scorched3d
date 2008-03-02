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

#include <webserver/ServerWebServerI.h>
#include <net/NetMessagePool.h>

ServerWebServerIRequest::ServerWebServerIRequest(const char *url,
		std::map<std::string, std::string> &fields,
		std::map<std::string, NetMessage *> &parts) :
	url_(url),
	fields_(fields),
	parts_(parts),
	session_(0)
{
	parts.clear();
}

ServerWebServerIRequest::~ServerWebServerIRequest()
{
	// Add any message parts back to the pool
	std::map<std::string, NetMessage *>::iterator partitor;
	for (partitor = parts_.begin();
		partitor != parts_.end();
		partitor++)
	{
		NetMessage *newMessage = (*partitor).second;
		NetMessagePool::instance()->addToPool(newMessage);
	}
}

ServerWebServerI::~ServerWebServerI()
{
}
