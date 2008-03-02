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

#if !defined(__INCLUDE_NetServerUDPDestinationh_INCLUDE__)
#define __INCLUDE_NetServerUDPDestinationh_INCLUDE__

#include <net/NetInterface.h>
#include <list>
#include <map>

class NetServerUDP;
class NetServerUDPDestination
{
public:
	NetServerUDPDestination(NetServerUDP *server, IPaddress &address);
	virtual ~NetServerUDPDestination();

	IPaddress &getAddress() { return address_; }
	unsigned int getIpAddress();

	void processData(unsigned int destinationId, int len, unsigned char *data, bool fin);
	void processDataAck(unsigned int destinationId, int len, unsigned char *data);
	void addMessage(NetMessage &message);
	void printStats(unsigned int destinationId);

	enum OutgoingResult
	{
		OutgoingSent,
		OutgoingEmpty,
		OutgoingTimeout
	};

	OutgoingResult checkOutgoing();

protected:
	struct MessagePart
	{
		unsigned int seq;
		unsigned int offset;
		unsigned int length;
		unsigned int sendtime;
		unsigned int retries;
		bool end;
	};
	struct OutgoingMessage
	{
		NetMessage *message_;
		std::list<MessagePart> pendingParts_;
		std::map<unsigned int, MessagePart> sentParts_;
	};

	unsigned int droppedPackets_; // Packets droped
	unsigned int packetsWaiting_; // Packets waiting to be sent
	unsigned int packetsSent_; // Packets sent
	unsigned int packetsRecieved_; // Packets recieved
	unsigned int packetsAsyncRecieved_;
	unsigned int packetsOutOfSequenceRecieved_;
	unsigned int packetsSequenceRecieved_;
	unsigned int packetsDuplicateRecieved_;
	unsigned int packetTime_; // Time for last packet
	unsigned int packetsAcked_; // The number of sent packets that have been acked
	unsigned int messagesSent_;
	unsigned int messagesRecieved_;
	bool packetLogging_;
	NetServerUDP *server_;
	IPaddress address_;
	unsigned int sendSeq_;
	unsigned int recvSeq_;
	NetMessage *recvMessage_;
	std::map<unsigned int, NetMessage *> incomingMessages_;
	std::list<OutgoingMessage *> outgoingMessages_;
	std::list<OutgoingMessage *> outgoingAsyncMessages_;

	void processAck(unsigned int seq);
	void addData(unsigned int destinationId, int len, unsigned char *data, bool fin);
	bool sendPart(MessagePart &part, NetMessage &message);
	OutgoingResult checkOutgoingSync();
	void checkOutgoingAsync();
};

#endif // __INCLUDE_NetServerUDPDestinationh_INCLUDE__
