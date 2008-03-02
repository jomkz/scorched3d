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

#if !defined(__INCLUDE_ServerAuthHandlerPreferedh_INCLUDE__)
#define __INCLUDE_ServerAuthHandlerPreferedh_INCLUDE__

#include <server/ServerAuthHandler.h>
#include <list>

class ServerAuthHandlerPrefered : public ServerAuthHandler
{
public:
	struct UserEntry
	{
		std::string name;
		std::string uniqueid;
	};

	ServerAuthHandlerPrefered();
	virtual ~ServerAuthHandlerPrefered();

	virtual void createAuthentication(ComsConnectAuthMessage &authMessage);
	virtual bool authenticateUser(ComsConnectAuthMessage &authMessage, 
		std::string &message);
	virtual bool authenticateUserName(const char *uniqueId, 
		const char *playername);
	virtual void banUser(const char *uniqueId);

protected:
	std::list<UserEntry> entries_;
	unsigned int lastReadTime_;

	UserEntry *getUserByName(const char *name);
	UserEntry *getUserById(const char *uniqueId);
	bool load();
};

#endif // __INCLUDE_ServerAuthHandlerPreferedh_INCLUDE__
