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

#include <net/NetServerTCP3Recv.h>
#include <net/NetMessagePool.h>
#include <common/Logger.h>

NetServerTCP3Recv::NetServerTCP3Recv(
	boost::asio::ip::tcp::socket *socket, 
	unsigned int destinationId, unsigned int ipAddress,
	NetMessageHandler *recieveMessageHandler) :
	socket_(socket), 
	destinationId_(destinationId), ipAddress_(ipAddress),
	recieveMessageHandler_(recieveMessageHandler),
	messagesRecieved_(0), bytesIn_(0),
	stopped_(false), running_(true)
{
	recvThread_ = new boost::thread(
		NetServerTCP3Recv::recvThreadFunc, (void *) this);
	if (recvThread_ == 0)
	{
		Logger::log(
			"NetServerTCP3Recv: Failed to create recv thread");
	}
}

NetServerTCP3Recv::~NetServerTCP3Recv()
{
	delete recvThread_;
	recvThread_ = 0;
}

int NetServerTCP3Recv::recvThreadFunc(void *c)
{
	// Call a non-static class thread to do the processing in (just for convienience)
	NetServerTCP3Recv *th = (NetServerTCP3Recv*) c;
	while (th->running_)
	{
		if (!th->actualRecvFunc()) break;
	}
	th->stopped_ = true;
	return 0;
}

bool NetServerTCP3Recv::actualRecvFunc()
{
	// Receive the length of the string message
	char lenbuf[4];
	if (boost::asio::read(*socket_, boost::asio::buffer(lenbuf, 4)) != 4)
	{
		return false;
	}
	uint32_t netlen = 0;
	memcpy(&netlen, lenbuf, sizeof(netlen));
	uint32_t len = ntohl(netlen);
	
	// Cannot recieve a message larger than 
	if (len > 15000000 || len == 0)
	{
		Logger::log(S3D::formatStringBuffer(
			"NetServerTCP3Recv: Buffer was too large to recieve.  Size %i.",
			len));
		return false;
	}

	// allocate the buffer memory
	unsigned int recvTime = 0;
	NetMessage *buffer = NetMessagePool::instance()->
		getFromPool(NetMessage::BufferMessage, 
		destinationId_, ipAddress_, 0, recvTime);
	buffer->getBuffer().allocate(len);
	buffer->getBuffer().setBufferUsed(len);

	// get the string buffer over the socket
	if (boost::asio::read(*socket_, boost::asio::buffer(buffer->getBuffer().getBuffer(), len)) != len)
	{
		Logger::log(S3D::formatStringBuffer(
			"NetServerTCP3Recv: Read failed for buffer"));
		NetMessagePool::instance()->addToPool(buffer);
		return false;
	}

	// Notify that this message has been recieved
	NetInterface::getBytesIn() += len;
	bytesIn_ += len;
	messagesRecieved_ ++;
	recieveMessageHandler_->addMessage(buffer);

	return true;
}

void NetServerTCP3Recv::wait()
{
	// Should do nothing
	recvThread_->join();	
}
