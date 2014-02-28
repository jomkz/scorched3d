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

#include <net/NetServerTCP3.h>
#include <net/NetMessagePool.h>
#include <common/Logger.h>
#include <common/Clock.h>
#include <common/ThreadUtils.h>
#include <limits.h>

NetServerTCP3::NetServerTCP3() : 
	serverDestinationId_(UINT_MAX), nextDestinationId_(1),
	sendRecvThread_(0),
	serverSock_(0),
	stopped_(false)
{

}

NetServerTCP3::~NetServerTCP3()
{
	delete sendRecvThread_;
	sendRecvThread_ = 0;
	delete serverSock_;
	serverSock_ = 0;
}

bool NetServerTCP3::started()
{
	// Do we have a valid send/recieve thread
	return (sendRecvThread_ != 0);
}

bool NetServerTCP3::connect(const char *hostName, int portNo)
{
	// Stop any previous connections
	stop();

	// Resolve server address
	std::string portStr = S3D::formatStringBuffer("%i", portNo);
	boost::asio::ip::tcp::resolver resolver(io_service_);
    boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), hostName, portStr.c_str());
	boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);
	
	// Create a new socket for the client
    boost::asio::ip::tcp::socket *clientSock = new boost::asio::ip::tcp::socket(io_service_);
	boost::system::error_code ec;
    clientSock->connect(*iterator, ec);
	if (ec) 
	{
		Logger::log(S3D::formatStringBuffer("NetServerTCP3: Failed to resolve host %s:%i %s",
			hostName, portNo, ec.message().c_str()));
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
	// Stop any previous connections
	stop();

	// Get the local ip address
	serverSock_ = new boost::asio::ip::tcp::acceptor(io_service_, 
		boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), portNo));
	if (!serverSock_->is_open()) return false;	

	// Set non-blocking accepts
	boost::system::error_code ec;
	serverSock_->non_blocking(true, ec);
	if (ec) return false;

	// Start sending/recieving on this socket
	if (!startProcessing()) return false;

	return true;
}

void NetServerTCP3::stop()
{
	if (started())
	{
		boost::thread *localSendRecvThread = sendRecvThread_;
		disconnectAllClients();
		localSendRecvThread->join();
	}
}

bool NetServerTCP3::startProcessing()
{
	// Check if we are already running
	DIALOG_ASSERT(!sendRecvThread_);

	// We are going to process all incoming message
	outgoingMessageHandler_.setMessageHandler(this);

	// Create the processing thread
	sendRecvThread_ = new boost::thread(
		NetServerTCP3::sendRecvThreadFunc, (void *) this);
	ThreadUtils::setThreadName(sendRecvThread_->native_handle(), "NetServerTCP3::sendRecvThread");
	return true;
}

int NetServerTCP3::sendRecvThreadFunc(void *c)
{
	// Call a non-static class thread to do the processing in (just for convienience)
	NetServerTCP3 *th = (NetServerTCP3*) c;
	th->actualSendRecvFunc();

	Logger::log(S3D::formatStringBuffer("NetServerTCP3: shutdown"));

	delete th->sendRecvThread_;
	th->sendRecvThread_ = 0;
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
		boost::this_thread::sleep(boost::posix_time::milliseconds(10));
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

	// Tidy any outstanding connections before we stop
	while (!finishedDestinations_.empty()) 
	{
		NetServerTCP3Destination *destination = finishedDestinations_.front();
		if (destination->allFinished())
		{
			delete destination;
			finishedDestinations_.pop_front();
		}
		boost::this_thread::sleep(boost::posix_time::milliseconds(10));
	}

	if (serverSock_) 
	{
		serverSock_->close();
		delete serverSock_;
	}
	serverSock_ = 0;
}

void NetServerTCP3::checkNewConnections()
{
	if (!serverSock_) return; // Check if we are running a server

	boost::asio::ip::tcp::socket *clientSock = new boost::asio::ip::tcp::socket(io_service_);
	boost::system::error_code ec;
	serverSock_->accept(*clientSock, ec);
	if (ec) 
	{
		delete clientSock;
		boost::this_thread::sleep(boost::posix_time::milliseconds(10));
		return;
	}

	// add client
	addDestination(clientSock);
}

void NetServerTCP3::checkClients()
{
	// Check each destination to see if it has closed
	std::map<unsigned int, NetServerTCP3Destination *>::iterator itor;
	for (itor = destinations_.begin();
		itor != destinations_.end();
		++itor)
	{
		NetServerTCP3Destination *destination = itor->second;
		if (destination->anyFinished())
		{
			NetBuffer buffer;
			destroyDestination(buffer, itor->first, NetMessage::UserDisconnect);
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

			// This is a message telling us to kick the client, do so
			destroyDestination(message.getBuffer(), destinationId, NetMessage::KickDisconnect);
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
		destroyDestination(message.getBuffer(), message.getDestinationId(), NetMessage::KickDisconnect);
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
	NetBuffer buffer;
	sendMessageTypeDest(buffer, 0, 0, NetMessage::DisconnectAllMessage);
}

void NetServerTCP3::disconnectClient(unsigned int destination)
{
	NetBuffer buffer;
	disconnectClient(buffer, destination);
}

void NetServerTCP3::disconnectClient(NetBuffer &buffer, 
	unsigned int destination)
{
	sendMessageTypeDest(buffer, destination, 0, NetMessage::DisconnectMessage);
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
	// Send a message to the client
	sendMessageTypeDest(buffer, destination, flags, NetMessage::BufferMessage);
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

void NetServerTCP3::sendMessageTypeDest(NetBuffer &buffer, 
	unsigned int destination, unsigned int flags, 
	NetMessage::MessageType type)
{
	// Get a new buffer from the pool
	NetMessage *message = NetMessagePool::instance()->
		getFromPool(type, 
				destination, 0, flags);

	// Add message to new buffer
	message->getBuffer().allocate(buffer.getBufferUsed());
	memcpy(message->getBuffer().getBuffer(), 
		buffer.getBuffer(), buffer.getBufferUsed());
	message->getBuffer().setBufferUsed(buffer.getBufferUsed());

	// Send Mesage
	outgoingMessageHandler_.addMessage(message);
}

void NetServerTCP3::destroyDestination(NetBuffer &disconectMessage,
	unsigned int destinationId,
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

	// Get the destination and remove it from the set
	NetServerTCP3Destination *destination = (*itor).second;
	destinations_.erase(itor);
	destination->printStats();

	// Send a message to the destination telling it to shut
	// and optionaly containing the disconnect reason
	NetMessage *message = NetMessagePool::instance()->
		getFromPool(NetMessage::DisconnectMessage, 
			destinationId, destination->getIpAddress(), 
			(unsigned int) type);
	if (disconectMessage.getBuffer() && disconectMessage.getBufferUsed())
	{
		message->getBuffer().allocate(disconectMessage.getBufferUsed());
		memcpy(message->getBuffer().getBuffer(), 
			disconectMessage.getBuffer(), disconectMessage.getBufferUsed());
		message->getBuffer().setBufferUsed(disconectMessage.getBufferUsed());
	}
	destination->close(message);

	// Add this destination to the list of destintions that need deleted
	finishedDestinations_.push_back(destination);

	// Get a new buffer from the pool (with the discconect type set)
	// Add to outgoing message pool
	message = NetMessagePool::instance()->
		getFromPool(NetMessage::DisconnectMessage, 
			destinationId, destination->getIpAddress(), 
			(unsigned int) type);
	incomingMessageHandler_.addMessage(message);
}

unsigned int NetServerTCP3::addDestination(boost::asio::ip::tcp::socket *socket)
{
	NetInterface::getConnects() ++;

	// Allocate new destination id
	do {
		nextDestinationId_++;
	} while (nextDestinationId_ == 0 || nextDestinationId_ == UINT_MAX);
	unsigned int destinationId = nextDestinationId_;

	// Create new destination 
	unsigned int ipAddress = NetInterface::getIpAddressFromSocket(socket);
	NetServerTCP3Destination *destination = 
		new NetServerTCP3Destination(
			&incomingMessageHandler_, socket, 
			destinationId, ipAddress);
	destinations_[destinationId] = destination;

	// Get a new buffer from the pool (with the connect type set)
	NetMessage *message = NetMessagePool::instance()->
		getFromPool(NetMessage::ConnectMessage, 
			destinationId, destination->getIpAddress());
	incomingMessageHandler_.addMessage(message);

	// Return new id
	return destinationId;
}

