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

#include <server/ServerAdminSessions.h>
#include <server/ScorchedServer.h>
#include <server/ServerCommon.h>
#include <common/OptionsScorched.h>
#include <XML/XMLFile.h>
#include <time.h>

ServerAdminSessions *ServerAdminSessions::instance()
{
	static ServerAdminSessions *instance = 
		new ServerAdminSessions;
	return instance;
}

ServerAdminSessions::ServerAdminSessions()
{
}

ServerAdminSessions::~ServerAdminSessions()
{
}

ServerAdminSessions::SessionParams *ServerAdminSessions::getFirstSession()
{
	if (sessions_.empty()) return 0;
	return &(sessions_.begin()->second);
}

ServerAdminSessions::SessionParams *ServerAdminSessions::getSession(unsigned int sid)
{
	const unsigned int SessionTimeOut = 60 * 15;
	unsigned int currentTime = (unsigned int) time(0);

	// Tidy expired sessions
	std::map <unsigned int, SessionParams>::iterator sitor;
	for (sitor = sessions_.begin();
		sitor != sessions_.end();
		sitor++)
	{
		SessionParams &params = (*sitor).second;
		if (currentTime > params.sessionTime + SessionTimeOut)
		{
			sessions_.erase(sitor);
			break;
		}
	}

	// Check if user has a valid session
	std::map <unsigned int, SessionParams>::iterator findItor =
		sessions_.find(sid);
	if (findItor != sessions_.end())
	{
		SessionParams &params = (*findItor).second;
		if (currentTime < params.sessionTime + SessionTimeOut)
		{
			params.sessionTime = currentTime;
			return &params;
		}
		else
		{
			sessions_.erase(findItor);
		}
	}

	return 0;
}

unsigned int ServerAdminSessions::login(const char *userId, const char *ipAddress)
{
	unsigned int sid = 0;

	// Try to find an existing session for this user
	std::map<unsigned int, SessionParams>::iterator itor;
	for (itor = sessions_.begin();
		itor != sessions_.end();
		itor++)
	{
		SessionParams &params = (*itor).second;
		if (0 == strcmp(params.userName.c_str(), userId))
		{
			// Found one
			sid = (*itor).first;
			break;
		}
	}

	// Generate a sid if we didn't find an existing login
	if (sid == 0)
	{
		// Generate a new unique session id
		do 
		{
			sid = rand();
		} while (sessions_.find(sid) != sessions_.end());
	}

	// Update the session params
	unsigned int currentTime = (unsigned int) time(0);
	SessionParams params;
	params.sessionTime = currentTime;
	params.userName = userId;
	params.ipAddress = ipAddress;
	params.sid = sid;
	sessions_[sid] = params;

	return sid;
}

void ServerAdminSessions::logout(unsigned int sid)
{
	std::map <unsigned int, SessionParams>::iterator sitor =
		sessions_.find(sid);
	if (sitor != sessions_.end())
	{
		sessions_.erase(sitor);
	}
}

bool ServerAdminSessions::authenticate(const char *name, const char *password)
{
#ifndef S3D_SERVER
	{
		return true;
	}
#endif

	std::list<Credential> creds;
	std::list<Credential>::iterator itor;
	if (!getAllCredentials(creds)) return false;

	for (itor = creds.begin();
		itor != creds.end();
		itor++)
	{
		Credential &credential = (*itor);
		if (0 == strcmp(name, credential.username.c_str()) &&
			0 == strcmp(password, credential.password.c_str()))
		{
			return true;
		}
	}
	return false;
}

bool ServerAdminSessions::getAllCredentials(std::list<Credential> &creds)
{
	std::string fileName = 
		S3D::getSettingsFile(S3D::formatStringBuffer("adminpassword-%i.xml",
			ScorchedServer::instance()->getOptionsGame().getPortNo()));

	XMLFile file;
	if (!file.readFile(fileName))
	{
		ServerCommon::serverLog( 
			S3D::formatStringBuffer("Failed to parse \"%s\"\n%s", 
			fileName.c_str(),
			file.getParserError()));
		return false;
	}
	if (!file.getRootNode())
	{
		ServerCommon::serverLog( 
			S3D::formatStringBuffer("Please create file %s to have admin users", 
			fileName.c_str()));
		return false;
	}

	// Itterate all of the users in the file
    std::list<XMLNode *>::iterator childrenItor;
	std::list<XMLNode *> &children = file.getRootNode()->getChildren();
    for (childrenItor = children.begin();
		 childrenItor != children.end();
		 childrenItor++)
    {
        XMLNode *currentNode = (*childrenItor);
		if (strcmp(currentNode->getName(), "user")) return false;

		Credential credential;
		if (!currentNode->getNamedChild("name", credential.username)) return false;
		if (!currentNode->getNamedChild("password", credential.password)) return false;
		if (!currentNode->failChildren()) return false;
		creds.push_back(credential);
	}

	return true;	
}
