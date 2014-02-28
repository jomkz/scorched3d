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

#if !defined(__INCLUDE_ClientStateInitializeh_INCLUDE__)
#define __INCLUDE_ClientStateInitializeh_INCLUDE__

#include <string>
#include <net/NetBuffer.h>

class NetMessage;
class NetBufferReader;
class NetInterface;
class UniqueIdStore;
class ComsMessageHandler;
class ClientStateInitialize  
{
public:
	ClientStateInitialize(ComsMessageHandler &comsMessageHandler);
	virtual ~ClientStateInitialize();

	// Called when we want to connect
	void enterState();

	// Called by the ClientMessageHandler when we are connected
	void connected();

	UniqueIdStore &getIdStore();
protected:
	NetBuffer recvBuffer_;
	boost::thread *remoteConnectionThread_;
	UniqueIdStore *idStore_;
	unsigned int totalBytes_;

	static void getHost(std::string &host, int &port);
	void tryConnection();
	static int tryRemoteConnection(NetInterface *clientNetInterface);
	void startTryRemoteConnectionServer();
	void startTryRemoteConnection();
	void finishedTryingConnection();
	void finished();
	void connectToServer();
	void sendAuth();
	bool initializeMod();
	bool processConnectAuthMessage(NetMessage &netMessage, NetBufferReader &reader);
	bool processConnectAcceptMessage(NetMessage &netMessage, NetBufferReader &reader);
	bool processFileMessage(NetMessage &netMessage, NetBufferReader &mainreader);
	bool processInitializeModMessage(NetMessage &netMessage, NetBufferReader &reader);
};

#endif
