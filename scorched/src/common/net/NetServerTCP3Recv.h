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

#if !defined(__INCLUDE_NetServerTCP3Recvh_INCLUDE__)
#define __INCLUDE_NetServerTCP3Recvh_INCLUDE__

#include <net/NetInterface.h>
#include <list>

class NetServerTCP3Recv
{
public:
	NetServerTCP3Recv(TCPsocket socket, 
		unsigned int destinationId, unsigned int ipAddress,
		NetMessageHandler *recieveMessageHandler);
	virtual ~NetServerTCP3Recv();

	bool getStopped() { return stopped_; }
	void stop() { running_ = false; }
	void wait();

	unsigned int getMessagesRecieved() { return messagesRecieved_; }
	unsigned int getBytesIn() { return bytesIn_; }

protected:
	bool stopped_, running_;
	unsigned int destinationId_, ipAddress_;
	TCPsocket socket_;
	SDLNet_SocketSet socketSet_;
	SDL_Thread *recvThread_;
	NetMessageHandler *recieveMessageHandler_;
	unsigned int messagesRecieved_, bytesIn_;

	static int recvThreadFunc(void *c);
	bool actualRecvFunc();
};

#endif // __INCLUDE_NetServerTCP3Recvh_INCLUDE__
