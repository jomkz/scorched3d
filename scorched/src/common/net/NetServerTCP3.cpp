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

#include <net/NetServerTCP3.h>
#include <net/NetMessagePool.h>
#include <net/NetOptions.h>
#include <common/Logger.h>
#include <common/Clock.h>
#include <limits.h>

NetServerTCP3::NetServerTCP3() : 
	serverDestinationId_(UINT_MAX), nextDestinationId_(1),
	sendRecvThread_(0),
	serverSock_(0), serverSockSet_(0),
	stopped_(false)
{
	NetOptions::instance()->readOptionsFromFile();
	NetOptions::instance()->writeOptionsToFile();

	serverSockSet_ = SDLNet_AllocSocketSet(1);
}

NetServerTCP3::~NetServerTCP3()
{
	SDLNet_FreeSocketSet(serverSockSet_);
	serverSockSet_ = 0;
}

bool NetServerTCP3::started()
{
	// Do we have a valid send/recieve thread
	return (sendRecvThread_ != 0);
}

bool NetServerTCP3::connect(const char *hostName, int portNo)
{
	if(SDLNet_Init()==-1)
	{
		return false;
	}

	// Resolve server address
	IPaddress serverAddress;
	if (SDLNet_ResolveHost(&serverAddress, hostName, portNo) != 0)
	{
		Logger::log(S3D::formatStringBuffer("NetServerTCP3: Failed to resolve host %s:%i",
			hostName, portNo));
		return false;
	}

	// Stop any previous connections
	stop();

	// Create a new socket for the client
	TCPsocket clientSock = SDLNet_TCP_Open(&serverAddress);
	if (!clientSock)
	{
		Logger::log(S3D::formatStringBuffer("NetServerTCP3: Failed to open client socket : %s",
			SDLNet_GetError()));
		return false;
	}

	// Add client socket
	serverDestinationId_ = addDestination(clientSock);

	// Start sending/recieving on the anon port
	if (!startProcessing()) return false;

	return true;
}

bool NetServerTCP3::start(int portNo)
{
	if(SDLNet_Init()==-1)
	{
		return false;
	}

	// Stop any previous connections
	stop();

	// Get the local ip address
	IPaddress localip;
	if(SDLNet_ResolveHost(&localip, NULL, portNo)==-1)
	{
		return false;
	}

	// we seem to be able to open the same port
	// multiple times!!!
	// This is fixed as the server info port catches it
	serverSock_ = SDLNet_TCP_Open(&localip);
	if (!serverSock_)
	{
		Logger::log(S3D::formatStringBuffer("NetServerTCP3: Failed to open server socket %i", portNo));
		return false;
	}
	SDLNet_TCP_AddSocket(serverSockSet_, serverSock_);

	// Start sending/recieving on this socket
	if (!startProcessing()) return false;

	return true;
}

void NetServerTCP3::stop()
{
	if (started())
	{
		disconnectAllClients();
		while (started()) SDL_Delay(100);
	}
}

bool NetServerTCP3::startProcessing()
{
	// Check if we are already running
	DIALOG_ASSERT(!sendRecvThread_);

	// We are going to process all incoming message
	outgoingMessageHandler_.setMessageHandler(this);

	// Create the processing thread
	sendRecvThread_ = SDL_CreateThread(
		NetServerTCP3::sendRecvThreadFunc, (void *) this);
	if (sendRecvThread_ == 0)
	{
		Logger::log(S3D::formatStringBuffer("NetServerTCP3: Failed to create NetServerTCP3 thread"));
		return false;
	}

	return true;
}

int NetServerTCP3::sendRecvThreadFunc(void *c)
{
	// Call a non-static class thread to do the processing in (just for convienience)
	NetServerTCP3 *th = (NetServerTCP3*) c;
	th->actualSendRecvFunc();

	th->sendRecvThread_ = 0;
	Logger::log(S3D::formatStringBuffer("NetServerTCP3: shutdown"));
	return 0;
}

void NetServerTCP3::actualSendRecvFunc()
{
	float timeDiff;
	stopped_ = false;
	Clock netClock;
	while(!stopped_)
	{
		// Send/recv packets
		checkClients();
		timeDiff = netClock.getTimeDifference();
		if (timeDiff > 1.0f)
		{
			Logger::log(S3D::formatStringBuffer(
				"NetServerTCP3: checkClients took %.2f seconds", 
				timeDiff));
		}

		// Check for new connections
		checkNewConnections();
		timeDiff = netClock.getTimeDifference();
		if (timeDiff > 1.0f)
		{
			Logger::log(S3D::formatStringBuffer(
				"NetServerTCP3: checkNewConnections took %.2f seconds", 
				timeDiff));
		}

		// sleep so we don't go into an infinite loop
		SDL_Delay(10);
		netClock.getTimeDifference();

		// Check for any new messages we should send and process them
		outgoingMessageHandler_.processMessages();
		timeDiff = netClock.getTimeDifference();
		if (timeDiff > 1.0f)
		{
			Logger::log(S3D::formatStringBuffer(
				"NetServerTCP3: processMessages took %.2f seconds", 
				timeDiff));
		}
	}

	if (serverSock_) SDLNet_TCP_Close(serverSock_);
	serverSock_ = 0;
}

void NetServerTCP3::checkNewConnections()
{
	if (!serverSock_) return; // Check if we are running a server

	int numready = SDLNet_CheckSockets(serverSockSet_, 10);
	if (numready == -1) return;
	if (numready == 0) return;

	if(SDLNet_SocketReady(serverSock_))
	{
		TCPsocket clientSock = SDLNet_TCP_Accept(serverSock_);
		if (clientSock)
		{
			// add client
			addDestination(clientSock);
		}
	}
}

void NetServerTCP3::checkClients()
{
	// Check each destination to see if it has closed
	std::map<unsigned int, NetServerTCP3Destination *>::iterator itor;
	for (itor = destinations_.begin();
		itor != destinations_.end();
		itor++)
	{
		NetServerTCP3Destination *destination = itor->second;
		if (destination->anyFinished())
		{
			destroyDestination(itor->first, NetMessage::UserDisconnect);
			break;
		}
	}

	// Check each destination that is closing to see if it has finished
	if (finishedDestinations_.empty()) return;

	NetServerTCP3Destination *destination = finishedDestinations_.front();
	if (destination->allFinished())
	{
		delete destination;
		finishedDestinations_.pop_front();
	}
}

void NetServerTCP3::processMessage(NetMessage &message)
{
	// We have been told to send a message to a client

	// Check if we have been told to disconect all clients
	if (message.getMessageType() == NetMessage::DisconnectAllMessage)
	{
		// Foreach client
		while (!destinations_.empty())
		{
			// Get the first client
			std::map<unsigned int, NetServerTCP3Destination *>::iterator itor =
				destinations_.begin();
			unsigned int destinationId = (*itor).first;
			NetServerTCP3Destination *destination = (*itor).second;

			// This is a message telling us to kick the client, do so
			destroyDestination(destinationId, NetMessage::KickDisconnect);
		}

		stopped_ = true;
		return;
	}

	// Look up this destination in the list of current
	std::map<unsigned int, NetServerTCP3Destination *>::iterator itor = 
		destinations_.find(message.getDestinationId());
	if (itor == destinations_.end())
	{
		return;
	}

	NetServerTCP3Destination *destination = (*itor).second;
	if (message.getMessageType() == NetMessage::DisconnectMessage)
	{
		// This is a message telling us to kick the client, do so
		destroyDestination(message.getDestinationId(), NetMessage::KickDisconnect);
	}
	else
	{
		// Add this buffer to the list of items to be sent
		NetMessage *newMessage = NetMessagePool::instance()->getFromPool(
			message.getMessageType(), message.getDestinationId(),
			message.getIpAddress(), message.getFlags());
		newMessage->getBuffer().allocate(message.getBuffer().getBufferUsed());
		memcpy(newMessage->getBuffer().getBuffer(), 
			message.getBuffer().getBuffer(),
			message.getBuffer().getBufferUsed());
		newMessage->getBuffer().setBufferUsed(message.getBuffer().getBufferUsed());

		destination->sendMessage(newMessage);
	}
}

void NetServerTCP3::disconnectAllClients()
{
	// Get a new buffer from the pool (with the discconect type set)
	NetMessage *message = NetMessagePool::instance()->
		getFromPool(NetMessage::DisconnectAllMessage, 0, 0);

	// Send Mesage
	outgoingMessageHandler_.addMessage(message);
}

void NetServerTCP3::disconnectClient(unsigned int destination)
{
	// Get a new buffer from the pool (with the discconect type set)
	NetMessage *message = NetMessagePool::instance()->
		getFromPool(NetMessage::DisconnectMessage, destination, 0);

	// Send Mesage
	outgoingMessageHandler_.addMessage(message);
}

void NetServerTCP3::sendMessageServer(NetBuffer &buffer, 
	unsigned int flags)
{
	// Send a message to the server
	sendMessageDest(buffer, serverDestinationId_, flags);
}

void NetServerTCP3::sendMessageDest(NetBuffer &buffer, 
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

int NetServerTCP3::processMessages()
{
	// Process any messages that we have recieved
	return incomingMessageHandler_.processMessages();
}

void NetServerTCP3::setMessageHandler(NetMessageHandlerI *handler)
{
	// Set the message handler to process any messages that we recieve
	incomingMessageHandler_.setMessageHandler(handler);
}

void NetServerTCP3::destroyDestination(unsigned int destinationId,
	NetMessage::DisconnectFlags type)
{
	// Destroy this destination
	std::map<unsigned int, NetServerTCP3Destination *>::iterator itor =
		destinations_.find(destinationId);
	if (itor == destinations_.end())
	{
		return;
	}

	if (serverDestinationId_ == destinationId)
	{
		stopped_ = true;
		serverDestinationId_ = UINT_MAX;
	}

	NetServerTCP3Destination *destination = (*itor).second;

	// Get a new buffer from the pool (with the discconect type set)
	NetMessage *message = NetMessagePool::instance()->
		getFromPool(NetMessage::DisconnectMessage, 
			destinationId, destination->getIpAddress());
	message->setFlags((unsigned int) type);

	destinations_.erase(itor);
	destination->printStats();
	destination->close();
	finishedDestinations_.push_back(destination);

	// Add to outgoing message pool
	incomingMessageHandler_.addMessage(message);
}

unsigned int NetServerTCP3::addDestination(TCPsocket &socket)
{
	NetInterface::getConnects() ++;

	// Allocate new destination id
	do {
		nextDestinationId_++;
	} while (nextDestinationId_ == 0 || nextDestinationId_ == UINT_MAX);
	unsigned int destinationId = nextDestinationId_;

	// Create new destination 
	NetServerTCP3Destination *destination = 
		new NetServerTCP3Destination(
			&incomingMessageHandler_, socket, destinationId);
	destinations_[destinationId] = destination;

	// Get a new buffer from the pool (with the connect type set)
	NetMessage *message = NetMessagePool::instance()->
		getFromPool(NetMessage::ConnectMessage, 
			destinationId, destination->getIpAddress());
	incomingMessageHandler_.addMessage(message);

	// Return new id
	return destinationId;
}
