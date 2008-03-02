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


#if !defined(__INCLUDE_ServerMessageHandlerh_INCLUDE__)
#define __INCLUDE_ServerMessageHandlerh_INCLUDE__

#include <coms/ComsMessageHandler.h>

class ServerMessageHandler : 
	public ComsMessageConnectionHandlerI
{
public:
	static ServerMessageHandler *instance();

	virtual void clientConnected(NetMessage &message);
	virtual void clientDisconnected(NetMessage &message);
	virtual void clientError(NetMessage &message,
		const char *errorString);

	virtual void messageRecv(unsigned int destinationId);
	virtual void messageSent(unsigned int destinationId);

	void destroyPlayer(unsigned int playerId, const char *reason);
	void destroyTaggedPlayers();

protected:
	static ServerMessageHandler *instance_;
	void actualDestroyPlayer(unsigned int tankId);

private:
	ServerMessageHandler();
	virtual ~ServerMessageHandler();
};


#endif
