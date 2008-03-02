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

#if !defined(__INCLUDE_NetServerUDPh_INCLUDE__)
#define __INCLUDE_NetServerUDPh_INCLUDE__

#include <net/NetServerUDPDestination.h>
#include <map>

class NetServerUDP : 
	public NetInterface,
	public NetMessageHandlerI
{
public:
	NetServerUDP();
	virtual ~NetServerUDP();

	// NetInterface
	virtual bool started();
	virtual bool connect(const char *hostName, int portNo);
	virtual bool start(int portNo);
	virtual void stop();

	virtual int processMessages();
	virtual void setMessageHandler(NetMessageHandlerI *handler);

	virtual void disconnectAllClients();
	virtual void disconnectClient(unsigned int client);
	virtual void sendMessageServer(NetBuffer &buffer, 
		unsigned int flags = 0);
	virtual void sendMessageDest(NetBuffer &buffer, 
		unsigned int destination, unsigned int flags = 0);

	// NetMessageHandlerI
	virtual void processMessage(NetMessage &message);

protected:
	friend class NetServerUDPDestination;

	enum PacketType
	{
		eConnect = 1,
		eConnectAck = 2,
		eDisconnect = 3,
		eData = 4,
		eDataFin = 5,
		eDataAck = 6
	};

	NetMessageHandler outgoingMessageHandler_;
	NetMessageHandler incomingMessageHandler_;
	UDPsocket udpsock_;
	UDPpacket **packetVIn_;
	UDPpacket **packetVOut_;
	SDL_Thread *sendRecvThread_;
	std::map<unsigned int, NetServerUDPDestination *> destinations_;
	unsigned int serverDestinationId_;
	unsigned int nextDestinationId_;

	bool checkIncoming();
	bool checkOutgoing();
	bool startProcessing();
	void actualSendRecvFunc();
	bool sendConnect(IPaddress &address, PacketType type);
	static int sendRecvThreadFunc(void *);

	void destroyDestination(unsigned int destinationId, 
		NetMessage::DisconnectFlags type);
	unsigned int getDestination(IPaddress &address);
	unsigned int addDestination(IPaddress &address);
};

#endif // __INCLUDE_NetServerUDPh_INCLUDE__
