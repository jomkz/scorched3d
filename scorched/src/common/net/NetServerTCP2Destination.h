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

#if !defined(__INCLUDE_NetServerTCP2Destinationh_INCLUDE__)
#define __INCLUDE_NetServerTCP2Destinationh_INCLUDE__

#include <net/NetInterface.h>
#include <list>
#include <map>

class NetServerTCP2;
class NetServerTCP2Destination : public NetMessageHandlerI
{
public:
	NetServerTCP2Destination(NetMessageHandler *incomingMessageHandler,
		TCPsocket socket, unsigned int destinationId);
	virtual ~NetServerTCP2Destination();

	void printStats(unsigned int destinationId);
	void stop() { stopped_ = true; }
	bool finished() { return finished_; }

	unsigned int getIpAddress();
	static unsigned int getIpAddressFromSocket(TCPsocket socket);

	void addMessage(NetMessage *message) 
	{
		outgoingMessageHandler_.addMessage(message);
	}

	// NetMessageHandlerI
	virtual void processMessage(NetMessage &message);

protected:
	enum HeaderType
	{
		TypeNone = 0,
		TypeMessage = 1,
		TypeLast = 4
	};

	bool packetLogging_;
	unsigned int destinationId_;
	SDL_Thread *sendRecvThread_;
	TCPsocket socket_;
	SDLNet_SocketSet socketSet_;
	NetBuffer currentMessagePart_;
	NetMessage *currentMessage_;
	int currentMessageLen_;
	char currentMessageType_;
	int currentMessageSentLen_;
	unsigned int messagesSent_, messagesRecieved_;
	unsigned int bytesIn_, bytesOut_;
	bool stopped_, finished_;
	NetMessageHandler outgoingMessageHandler_;
	NetMessageHandler *incomingMessageHandler_;
	std::list<NetMessage *> outgoingMessages_;

	enum SocketResult
	{
		SocketActivity,
		SocketEmpty,
		SocketClosed
	};
	SocketResult checkIncoming();
	SocketResult checkOutgoing();

	static int sendRecvThreadFunc(void *c);
	void actualSendRecvFunc();
	bool sendHeader(char headerType, int len);
};

#endif // __INCLUDE_NetServerTCP2Destinationh_INCLUDE__
