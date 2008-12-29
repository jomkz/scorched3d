////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2009
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

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#else
#include <errno.h>
#endif

#include <net/NetServerTCP2.h>
#include <net/NetMessagePool.h>
#include <net/NetOptions.h>
#include <common/Logger.h>
#include <common/Clock.h>
#include <set>

static int SDLNet_TCP_Recv_Wrapper(TCPsocket sock, void *data, int maxlen)
{
#ifdef WIN32
	WSASetLastError(0);
#else
	errno = 0;
#endif

	int result = SDLNet_TCP_Recv(sock, data, maxlen);
	if (result <= 0)
	{
#ifdef WIN32
		int wsacp = WSAGetLastError();
		if (wsacp != WSAECONNRESET)
		{
			Logger::log(S3D::formatStringBuffer(
				"SDLNet_TCP_Recv_Wrapper: WSA Error code %i", wsacp));
		}
#else
		int errnocp = errno;
		if (errnocp != ECONNRESET)
		{
			Logger::log(S3D::formatStringBuffer(
				"SDLNet_TCP_Recv_Wrapper: Error code %i", errnocp));
		}
#endif
	}

	return result;
}

static int SDLNet_TCP_Send_Wrapper(TCPsocket sock, void *datap, int len)
{
#ifdef WIN32
	WSASetLastError(0);
#else
	errno = 0;
#endif

	int result = SDLNet_TCP_Send(sock, datap, len);
	if (result <= 0)
	{
#ifdef WIN32
		int wsacp = WSAGetLastError();
		if (wsacp != WSAECONNRESET)
		{
			Logger::log(S3D::formatStringBuffer(
				"SDLNet_TCP_Send_Wrapper: WSA Error code %i", wsacp));
		}
#else
		int errnocp = errno;
		if (errnocp != ECONNRESET)
		{
			Logger::log(S3D::formatStringBuffer(
				"SDLNet_TCP_Send_Wrapper: Error code %i", errnocp));
		}
#endif
	}

	return result;
}

NetServerTCP2Destination::NetServerTCP2Destination(
	NetMessageHandler *incomingMessageHandler, 
	TCPsocket socket,
	unsigned int destinationId) :
	socket_(socket), socketSet_(0), 
	currentMessage_(0), currentMessageLen_(0),
	currentMessageSentLen_(0),
	destinationId_(destinationId), 
	packetLogging_(false),
	messagesSent_(0), messagesRecieved_(0), 
	bytesIn_(0), bytesOut_(0), 
	incomingMessageHandler_(incomingMessageHandler),
	sendRecvThread_(0),
	stopped_(false), finished_(false)
{
	socketSet_ = SDLNet_AllocSocketSet(1);
	SDLNet_TCP_AddSocket(socketSet_, socket_);

	outgoingMessageHandler_.setMessageHandler(this);
	packetLogging_ = NetOptions::instance()->getPacketLogging();

	sendRecvThread_ = SDL_CreateThread(
		NetServerTCP2Destination::sendRecvThreadFunc, (void *) this);
	if (sendRecvThread_ == 0)
	{
		Logger::log(
			"NetServerTCP2Destination: Failed to create NetServerTCP2Destination thread");
	}
}

NetServerTCP2Destination::~NetServerTCP2Destination()
{
	SDLNet_FreeSocketSet(socketSet_);
	socketSet_ = 0;

	if (socket_) SDLNet_TCP_Close(socket_);
	socket_ = 0;

	if (currentMessage_) NetMessagePool::instance()->addToPool(currentMessage_);
	currentMessage_ = 0;

	std::list<NetMessage *>::iterator itor;
	for (itor = outgoingMessages_.begin();
		itor != outgoingMessages_.end();
		itor++)
	{
		NetMessagePool::instance()->addToPool(*itor);
	}

	int status;
	SDL_WaitThread(sendRecvThread_, &status);
	sendRecvThread_ = 0;

	if (packetLogging_)
	{
		Logger::log(S3D::formatStringBuffer("NetServerTCP2Destination %u: destroyed",
			destinationId_));
	}
}

unsigned int NetServerTCP2Destination::getIpAddress()
{
	return getIpAddressFromSocket(socket_);
}

unsigned int NetServerTCP2Destination::getIpAddressFromSocket(TCPsocket socket)
{
	unsigned int addr = 0;
	IPaddress *address = SDLNet_TCP_GetPeerAddress(socket);
	if (address)
	{
		addr = SDLNet_Read32(&address->host);
	}
	return addr;
}

int NetServerTCP2Destination::sendRecvThreadFunc(void *c)
{
	// Call a non-static class thread to do the processing in (just for convienience)
	NetServerTCP2Destination *th = (NetServerTCP2Destination*) c;
	th->actualSendRecvFunc();
	th->finished_ = true;
	return 0;
}

void NetServerTCP2Destination::actualSendRecvFunc()
{
	Clock netClock;
	while (!stopped_)
	{
		{
			// Reset clock
			netClock.getTimeDifference();
		}

		{
			// Update messages
			outgoingMessageHandler_.processMessages();
			float timeDiff = netClock.getTimeDifference();
			if (timeDiff > 5.0f)
			{
				Logger::log(S3D::formatStringBuffer(
					"NetServerTCP2Destination %u: messages took %.2f seconds", 
					destinationId_,
					timeDiff));
			}
		}

		SocketResult sendResult;
		{
			// Send
			sendResult = checkOutgoing();
			float timeDiff = netClock.getTimeDifference();
			if (timeDiff > 5.0f)
			{
				Logger::log(S3D::formatStringBuffer(
					"NetServerTCP2Destination %u: outgoing took %.2f seconds", 
					destinationId_,
					timeDiff));
			}
			if (sendResult == SocketClosed) break;
		}

		SocketResult recvResult;
		{
			// Recv
			recvResult = checkIncoming();
			float timeDiff = netClock.getTimeDifference();
			if (timeDiff > 5.0f)
			{
				Logger::log(S3D::formatStringBuffer(
					"NetServerTCP2Destination %u: incoming took %.2f seconds", 
					destinationId_,
					timeDiff));
			}
			if (recvResult == SocketClosed) break;
		}

		if (sendResult == SocketEmpty &&
			recvResult == SocketEmpty)
		{
			SDL_Delay(10);
		}
	}

	if (packetLogging_)
	{
		Logger::log(S3D::formatStringBuffer("NetServerTCP2Destination %u: shutdown",
			destinationId_));
	}
}

void NetServerTCP2Destination::processMessage(NetMessage &oldmessage)
{
	// Get a new buffer from the pool
	NetMessage *message = NetMessagePool::instance()->
		getFromPool(NetMessage::SentMessage, 
			oldmessage.getDestinationId(), getIpAddress());

	// Copy old buffer into new buffer
	NetBuffer &buffer = oldmessage.getBuffer();
	message->getBuffer().allocate(buffer.getBufferUsed());
	memcpy(message->getBuffer().getBuffer(), 
		buffer.getBuffer(), buffer.getBufferUsed());
	message->getBuffer().setBufferUsed(buffer.getBufferUsed());

	// Add to list of outgoing
	outgoingMessages_.push_back(message);

	if (packetLogging_)
	{
		Logger::log(S3D::formatStringBuffer(
			"NetServerTCP2Destination %u: Adding a new message, %u now waiting", 
			destinationId_,
			outgoingMessages_.size()));
	}
}

NetServerTCP2Destination::SocketResult NetServerTCP2Destination::checkIncoming()
{
	bool activity = false;
	for (int i=0; i<2000; i++)
	{
		// Check if the socket is ready to give us data
		int numready = SDLNet_CheckSockets(socketSet_, 0);
		if (numready == -1) 
		{
			//if (packetLogging_)
			{
				Logger::log(S3D::formatStringBuffer(
					"NetServerTCP2Destination %u: CheckSockets returned an error %i", 
					destinationId_,
					numready));
			}
			return SocketClosed;
		}
		if (numready == 0) return (i==0?SocketEmpty:SocketActivity);
		if (!SDLNet_SocketReady(socket_)) return (activity?SocketActivity:SocketEmpty);

		// Get data from socket
		char buffer[1];
		int recv = SDLNet_TCP_Recv_Wrapper(socket_, &buffer, 1);
		if (recv <= 0)
		{
			//if (packetLogging_)
			{
				Logger::log(S3D::formatStringBuffer(
					"NetServerTCP2Destination %u: Recv returned an error %i", 
					destinationId_,
					recv));
			}
			return SocketClosed;
		}
		currentMessagePart_.addDataToBuffer(buffer, 1);

		// Update stats
		NetInterface::getBytesIn()++;
		bytesIn_++;
		activity = true;
		
		// Get messagetype and len
		if (currentMessagePart_.getBufferUsed() == 5)
		{
			NetBufferReader reader(currentMessagePart_);
			reader.getFromBuffer(currentMessageType_);
			reader.getFromBuffer(currentMessageLen_);
		}

		// Check if a full message has been recieved
		if (currentMessagePart_.getBufferUsed() >= 5 &&
			currentMessagePart_.getBufferUsed() == currentMessageLen_)
		{
			if (currentMessageType_ & TypeMessage)
			{
				if (!currentMessage_)
				{
					currentMessage_ = NetMessagePool::instance()->
						getFromPool(NetMessage::BufferMessage, 
							destinationId_, getIpAddress());
				}

				// A full message part has been received
				currentMessage_->getBuffer().addDataToBuffer(
					currentMessagePart_.getBuffer() + 5,
					currentMessagePart_.getBufferUsed() - 5);
				if (currentMessageType_ & TypeLast)
				{
					// A finished message has been recieved
					messagesRecieved_++;
					incomingMessageHandler_->addMessage(currentMessage_);
					currentMessage_ = 0;
				}

				// Log
				if (packetLogging_)
				{
					Logger::log(S3D::formatStringBuffer(
						"NetServerTCP2Destination %u: Recieved a message part, %s", 
						destinationId_,
						(currentMessageType_ & TypeLast?"last":"not last")));
				}
			}
			else 
			{
				Logger::log(S3D::formatStringBuffer(
					"NetServerTCP2Destination %u: Unknown message type %i received", 
					destinationId_,
					currentMessageType_));
				return SocketClosed;
			}

			currentMessageLen_ = 0;
			currentMessagePart_.reset();
		}
	}	

	return (activity?SocketActivity:SocketEmpty);
}

NetServerTCP2Destination::SocketResult NetServerTCP2Destination::checkOutgoing()
{
	// See if we have any messages to send
	if (outgoingMessages_.empty()) return SocketEmpty;

	// Check to see how much to send
	NetMessage *message = outgoingMessages_.front();
	int sendAmount = message->getBuffer().getBufferUsed() - currentMessageSentLen_;
	if (sendAmount > 100) sendAmount = 100;

	if (currentMessageSentLen_ == 0)
	{
		// Send Len
		if(!sendHeader(TypeMessage | TypeLast, 
			5 + message->getBuffer().getBufferUsed()))
		{
			return SocketClosed;
		}
	}

	// Send data
	int result = SDLNet_TCP_Send_Wrapper(socket_, 
		(void *) &message->getBuffer().getBuffer()[currentMessageSentLen_], 
		sendAmount);
	if(result < sendAmount) // Socket Closed
	{
		Logger::log(S3D::formatStringBuffer(
			"NetServerTCP2Destination %u: Failed to send buffer %i of %i. Socket closed",
			destinationId_, result, sendAmount));
		return SocketClosed;
	}

	NetInterface::getBytesOut() += result;
	bytesOut_ += result;

	// Check if we have finished sending this message
	currentMessageSentLen_ += sendAmount;
	if (currentMessageSentLen_ == message->getBuffer().getBufferUsed())
	{
		outgoingMessages_.pop_front();
		currentMessageSentLen_ = 0;
		incomingMessageHandler_->addMessage(message);

		messagesSent_++;
		if (packetLogging_)
		{
			Logger::log(S3D::formatStringBuffer(
				"NetServerTCP2Destination %u: Sent a message part, %s", 
				destinationId_,
				"last"));
		}
	}

	return SocketActivity;
}

bool NetServerTCP2Destination::sendHeader(char headerType, int len)
{
	NetMessage *ackMessage = NetMessagePool::instance()->
		getFromPool(NetMessage::BufferMessage, 0, 0);

	// Form Header
	ackMessage->getBuffer().addToBuffer(headerType);
	ackMessage->getBuffer().addToBuffer(len);

	// Send Header
	int result = SDLNet_TCP_Send_Wrapper(socket_, 
		(void *) ackMessage->getBuffer().getBuffer(), 
		ackMessage->getBuffer().getBufferUsed());

	NetInterface::getBytesOut() += result;
	bytesOut_ += result;

	NetMessagePool::instance()->addToPool(ackMessage);

	if (result != ackMessage->getBuffer().getBufferUsed())
	{
		Logger::log(S3D::formatStringBuffer(
			"NetServerTCP2Destination %u: Failed to send header %i of %i. Socket closed",
			destinationId_, result, ackMessage->getBuffer().getBufferUsed()));
	}

	return (result == ackMessage->getBuffer().getBufferUsed());
}

void NetServerTCP2Destination::printStats(unsigned int destination)
{
	Logger::log(S3D::formatStringBuffer("TCP2 Destination %u net stats:", destination));
	Logger::log(S3D::formatStringBuffer("  %u messages sent, %u messages recieved",
		messagesSent_, messagesRecieved_));
	Logger::log(S3D::formatStringBuffer("  %u bytes in, %u bytes out",
		bytesIn_, bytesOut_));
}
