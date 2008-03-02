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

#include <net/NetServerUDP.h>
#include <net/NetMessagePool.h>
#include <net/NetOptions.h>
#include <common/Logger.h>
#include <common/Clock.h>
#include <limits.h>

NetServerUDP::NetServerUDP() : 
	serverDestinationId_(UINT_MAX), udpsock_(0), nextDestinationId_(1),
	sendRecvThread_(0)
{
	NetOptions::instance()->readOptionsFromFile();
	NetOptions::instance()->writeOptionsToFile();

	packetVIn_ = SDLNet_AllocPacketV(20, 10000);
	packetVOut_ = SDLNet_AllocPacketV(20, 10000);
}

NetServerUDP::~NetServerUDP()
{
	SDLNet_FreePacketV(packetVIn_);
	SDLNet_FreePacketV(packetVOut_);
}

bool NetServerUDP::started()
{
	// Do we have a valid send/recieve thread
	return (sendRecvThread_ != 0);
}

bool NetServerUDP::connect(const char *hostName, int portNo)
{
	if(SDLNet_Init()==-1)
	{
		return false;
	}

	// Resolve server address
	IPaddress serverAddress;
	if (SDLNet_ResolveHost(&serverAddress, hostName, portNo) != 0)
	{
		Logger::log(S3D::formatStringBuffer("NetServerUDP: Failed to resolve host %s:%i",
			hostName, portNo));
		return false;
	}

	// Stop any previous connections
	stop();

	// Create a new socket with anon port
	udpsock_ = SDLNet_UDP_Open(0);
	if (!udpsock_)
	{
		Logger::log(S3D::formatStringBuffer("NetServerUDP: Failed to open client socket : %s",
			SDLNet_GetError()));
		return false;		
	}
	serverDestinationId_ = 0;

	// Send connect message
	if (!sendConnect(serverAddress, eConnect)) return false;

	// Start sending/recieving on the anon port
	if (!startProcessing()) return false;

	return true;
}

bool NetServerUDP::start(int portNo)
{
	if(SDLNet_Init()==-1)
	{
		return false;
	}

	// Stop any previous connections
	stop();

	// Create a new server listening socket
	udpsock_ = SDLNet_UDP_Open(portNo);
	if (!udpsock_)
	{
		Logger::log(S3D::formatStringBuffer("NetServerUDP: Failed to open server socket %i", portNo));
		return false;
	}

	// Start sending/recieving on this socket
	if (!startProcessing()) return false;

	return true;
}

void NetServerUDP::stop()
{
	if (started())
	{
		disconnectAllClients();
		while (started()) SDL_Delay(100);
	}
}

bool NetServerUDP::sendConnect(IPaddress &address, PacketType type)
{
	// Send a small message with a type
	packetVOut_[0]->len = 1;
	packetVOut_[0]->address.host = address.host;
	packetVOut_[0]->address.port = address.port;
	packetVOut_[0]->data[0] = type;
	packetVOut_[0]->channel = -1;
	if (SDLNet_UDP_SendV(udpsock_, packetVOut_, 1) == 0)
	{
		Logger::log(S3D::formatStringBuffer("NetServerUDP: Failed to send connect packet"));
		return false;
	}
	return true;
}

bool NetServerUDP::startProcessing()
{
	// Check if we are already running
	DIALOG_ASSERT(!sendRecvThread_);

	// We are going to process all incoming message
	outgoingMessageHandler_.setMessageHandler(this);

	// Create the processing thread
	sendRecvThread_ = SDL_CreateThread(
		NetServerUDP::sendRecvThreadFunc, (void *) this);
	if (sendRecvThread_ == 0)
	{
		Logger::log(S3D::formatStringBuffer("NetServerUDP: Failed to create NetServerUDP thread"));
		return false;
	}

	return true;
}

int NetServerUDP::sendRecvThreadFunc(void *c)
{
	// Call a non-static class thread to do the processing in (just for convienience)
	NetServerUDP *th = (NetServerUDP*) c;
	th->actualSendRecvFunc();

	th->sendRecvThread_ = 0;
	Logger::log(S3D::formatStringBuffer("NetServerUDP: shutdown"));
	return 0;
}

void NetServerUDP::actualSendRecvFunc()
{
	Clock netClock;
	while(udpsock_ != 0)
	{
		// Send/recv packets
		if (!checkOutgoing() &&
			!checkIncoming())
		{
			SDL_Delay(10);
		}

		// Check for any new messages we should send and process them
		outgoingMessageHandler_.processMessages();

		// Make sure this loop is progressing as expected
		float timeDiff = netClock.getTimeDifference();
		if (timeDiff > 1.0f)
		{
			Logger::log(S3D::formatStringBuffer(
				"NetServerUDP: coms loop took %.2f seconds", 
				timeDiff));
		}
	}
}

void NetServerUDP::processMessage(NetMessage &message)
{
	// We have been told to send a message to a client

	// Check if we have been told to disconect all clients
	if (message.getMessageType() == NetMessage::DisconnectAllMessage)
	{
		// Foreach client
		while (!destinations_.empty())
		{
			// Get the first client
			std::map<unsigned int, NetServerUDPDestination *>::iterator itor =
				destinations_.begin();
			unsigned int destinationId = (*itor).first;
			NetServerUDPDestination *destination = (*itor).second;

			// Tell the client they are kicked
			sendConnect(destination->getAddress(), eDisconnect);

			// This is a message telling us to kick the client, do so
			//Logger::log(S3D::formatStringBuffer("Disconnected %u - kicked", destinationId));
			destroyDestination(destinationId, NetMessage::KickDisconnect);
		}

		// This can occur if we have not recieved the connection accept from 
		// the server yet
		if (udpsock_)
		{
			SDLNet_UDP_Close(udpsock_);
			udpsock_ = 0;
		}

		return;
	}

	// Look up this destination in the list of current
	std::map<unsigned int, NetServerUDPDestination *>::iterator itor = 
		destinations_.find(message.getDestinationId());
	if (itor == destinations_.end())
	{
		Logger::log(S3D::formatStringBuffer(
			"NetServerUDP: Invalid send destination %u", message.getDestinationId()));
		return;
	}

	NetServerUDPDestination *destination = (*itor).second;
	if (message.getMessageType() == NetMessage::DisconnectMessage)
	{
		// Tell the client they are kicked
		sendConnect(destination->getAddress(), eDisconnect);

		// This is a message telling us to kick the client, do so
		//Logger::log(S3D::formatStringBuffer("Disconnected %u - kicked", message.getDestinationId()));
		destroyDestination(message.getDestinationId(), NetMessage::KickDisconnect);
	}
	else
	{
		NetInterface::getBytesOut() += message.getBuffer().getBufferUsed();

		// Add this buffer to the list of items to be sent
		destination->addMessage(message);
	}
}

bool NetServerUDP::checkOutgoing()
{
	bool sent = false;

	// Each client checks to see if they have any parts to send
	std::map<unsigned int, NetServerUDPDestination *>::iterator itor;
	for (itor = destinations_.begin();
		itor != destinations_.end();
		itor ++)
	{
		unsigned int destinationId = (*itor).first;
		NetServerUDPDestination *destination = (*itor).second;

		NetServerUDPDestination::OutgoingResult outResult = destination->checkOutgoing();
		switch (outResult)
		{
		case NetServerUDPDestination::OutgoingTimeout:

			// Client timedout
			//Logger::log(S3D::formatStringBuffer("Disconnected %u - timedout", destinationId));
			destroyDestination(destinationId, NetMessage::TimeoutDisconnect);
			return true; // Because we are in iterator
			break;
		case NetServerUDPDestination::OutgoingSent:
			sent = true;
			break;
		}
	}

	return sent;
}

bool NetServerUDP::checkIncoming()
{
	// Check socket is still valid
	if (!udpsock_) return false;

	// Get the next set of packets from the socket
	int numrecv = SDLNet_UDP_RecvV(udpsock_, packetVIn_);
	if(numrecv <=0) return false;
	
	// For each packet
	for(int i=0; i<numrecv; i++) 
	{
		// Get the packet details
		int packetLen = packetVIn_[i]->len;
		unsigned char *packetData = packetVIn_[i]->data;
		if (packetLen < 1)
		{
			Logger::log(S3D::formatStringBuffer("NetServerUDP: Invalid incoming packet size %i", packetLen));
			continue;
		}
		NetInterface::getBytesIn() += packetLen;

		// Check what type of packet we have recieved
		unsigned char packetType = packetData[0];
		unsigned int destinationId = getDestination(packetVIn_[i]->address);
		switch (packetType)
		{
		case eConnect:
			// A connect request
			if (destinationId != 0)
			{
				destroyDestination(destinationId, NetMessage::TimeoutDisconnect);
			}

			addDestination(packetVIn_[i]->address);
			sendConnect(packetVIn_[i]->address, eConnectAck);
			break;
		case eConnectAck:
			// A connect accept
			if (destinationId == 0)
			{
				addDestination(packetVIn_[i]->address);
			}
			break;
		case eDisconnect:
			// A disconnect request
			if (destinationId != 0)
			{
				//Logger::log(S3D::formatStringBuffer("Disconnected %u - user", destinationId));
				destroyDestination(destinationId, NetMessage::UserDisconnect);
			}
			break;
		case eData:
		case eDataFin:
			// A data packet
			if (destinationId != 0) 
			{
				NetServerUDPDestination *destination = destinations_[destinationId];
				destination->processData(
					destinationId, packetLen, packetData, (packetType == eDataFin));
			}
			break;
		case eDataAck:
			// A data packet ack
			if (destinationId != 0)
			{
				NetServerUDPDestination *destination = destinations_[destinationId];
				destination->processDataAck(
					destinationId, packetLen, packetData);
			}
			break;
		}
	}

	return true;
}

void NetServerUDP::disconnectAllClients()
{
	// Get a new buffer from the pool (with the discconect type set)
	NetMessage *message = NetMessagePool::instance()->
		getFromPool(NetMessage::DisconnectAllMessage, 0, 0);

	// Send Mesage
	outgoingMessageHandler_.addMessage(message);
}

void NetServerUDP::disconnectClient(unsigned int destination)
{
	// Get a new buffer from the pool (with the discconect type set)
	NetMessage *message = NetMessagePool::instance()->
		getFromPool(NetMessage::DisconnectMessage, destination, 0);

	// Send Mesage
	outgoingMessageHandler_.addMessage(message);
}

void NetServerUDP::sendMessageServer(NetBuffer &buffer, 
	unsigned int flags)
{
	// Send a message to the server
	sendMessageDest(buffer, serverDestinationId_, flags);
}

void NetServerUDP::sendMessageDest(NetBuffer &buffer, 
	unsigned int destination, unsigned int flags)
{
	// Get a new buffer from the pool
	NetMessage *message = NetMessagePool::instance()->
		getFromPool(NetMessage::BufferMessage, 
				destination, 0, flags);

	// Add message to new buffer
	message->getBuffer().allocate(buffer.getBufferUsed());
	memcpy(message->getBuffer().getBuffer(), 
		buffer.getBuffer(), buffer.getBufferUsed());
	message->getBuffer().setBufferUsed(buffer.getBufferUsed());

	// Send Mesage
	outgoingMessageHandler_.addMessage(message);
}

int NetServerUDP::processMessages()
{
	// Process any messages that we have recieved
	return incomingMessageHandler_.processMessages();
}

void NetServerUDP::setMessageHandler(NetMessageHandlerI *handler)
{
	// Set the message handler to process any messages that we recieve
	incomingMessageHandler_.setMessageHandler(handler);
}

void NetServerUDP::destroyDestination(unsigned int destinationId,
	NetMessage::DisconnectFlags type)
{
	// Destroy this destination
	std::map<unsigned int, NetServerUDPDestination *>::iterator itor =
		destinations_.find(destinationId);
	if (itor == destinations_.end())
	{
		return;
	}

	if (serverDestinationId_ == destinationId)
	{
		SDLNet_UDP_Close(udpsock_);
		udpsock_ = 0;
		serverDestinationId_ = 0;
	}

	NetServerUDPDestination *destination = (*itor).second;

	// Get a new buffer from the pool (with the discconect type set)
	NetMessage *message = NetMessagePool::instance()->
		getFromPool(NetMessage::DisconnectMessage, 
			destinationId, destination->getIpAddress());
	message->setFlags((unsigned int) type);

	destinations_.erase(itor);
	destination->printStats(destinationId);
	delete destination;

	// Add to outgoing message pool
	incomingMessageHandler_.addMessage(message);
}

unsigned int NetServerUDP::getDestination(IPaddress &address)
{
	// Search all destinations to find those that match
	// port and ip address
	std::map<unsigned int, NetServerUDPDestination *>::iterator itor;
	for (itor = destinations_.begin();
		itor != destinations_.end();
		itor ++)
	{
		unsigned int destinationId = (*itor).first;
		NetServerUDPDestination *destination = (*itor).second;
		if (destination->getAddress().host == address.host &&
			destination->getAddress().port == address.port)
		{
			return destinationId;
		}
	}
	return 0;
}

unsigned int NetServerUDP::addDestination(IPaddress &address)
{
	NetInterface::getConnects() ++;

	// Allocate new destination id
	do {
		nextDestinationId_++;
	} while (nextDestinationId_ == 0 || nextDestinationId_ == UINT_MAX);
	unsigned int destinationId = nextDestinationId_;

	// Create new destination 
	NetServerUDPDestination *destination = 
		new NetServerUDPDestination(this, address);
	destinations_[destinationId] = destination;

	// Get a new buffer from the pool (with the connect type set)
	NetMessage *message = NetMessagePool::instance()->
		getFromPool(NetMessage::ConnectMessage, 
			destinationId, destination->getIpAddress());
	incomingMessageHandler_.addMessage(message);

	// Make the server destination id the first destination
	if (serverDestinationId_ == 0) serverDestinationId_ = destinationId;

	// Return new id
	return destinationId;
}
