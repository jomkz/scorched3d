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

#if !defined(__INCLUDE_ServerAdminSessionsh_INCLUDE__)
#define __INCLUDE_ServerAdminSessionsh_INCLUDE__

#include <map>
#include <list>
#include <string>

class ServerAdminSessions
{
public:
	static ServerAdminSessions *instance();

	// Creds
	struct Credential
	{
		std::string username;
		std::string password;
	};

	bool authenticate(const char *name, const char *password);
	bool getAllCredentials(std::list<Credential> &creds);

	// Sessions
	struct SessionParams
	{
		unsigned int sessionTime;
		unsigned int sid;
		std::string ipAddress;
		std::string userName;
	};

	std::map<unsigned int, SessionParams> &getAllSessions() { return sessions_; }

	SessionParams *getFirstSession();
	SessionParams *getSession(unsigned int sid);

	unsigned int login(const char *userId, const char *ipAddress);
	void logout(unsigned int sid);

protected:
	std::map<unsigned int, SessionParams> sessions_;

private:
	ServerAdminSessions();
	virtual ~ServerAdminSessions();

};

#endif
