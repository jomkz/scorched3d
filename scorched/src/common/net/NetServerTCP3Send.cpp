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

#include <net/NetServerTCP3Send.h>
#include <net/NetMessagePool.h>
#include <common/Logger.h>

NetServerTCP3Send::NetServerTCP3Send(
	boost::asio::ip::tcp::socket *socket, 
	unsigned int destinationId, unsigned int ipAddress,
	NetMessageHandler *recieveMessageHandler) :
	socket_(socket),  sendThread_(0),
	destinationId_(destinationId), ipAddress_(ipAddress),
	recieveMessageHandler_(recieveMessageHandler),
	messagesSent_(0), bytesOut_(0),
	stopped_(false), running_(true)
{
	sendMessageHandler_.setMessageHandler(this);
	sendThread_ = new boost::thread(
		NetServerTCP3Send::sendThreadFunc, (void *) this);
	if (sendThread_ == 0)
	{
		Logger::log(
			"NetServerTCP3Send: Failed to create send thread");
	}
}

NetServerTCP3Send::~NetServerTCP3Send()
{
	delete sendThread_;
	sendThread_ = 0;
	std::list<NetMessage *>::iterator itor;
	for (itor = outgoingMessages_.begin();
		itor != outgoingMessages_.end();
		++itor)
	{
		NetMessagePool::instance()->addToPool(*itor);
	}
}

void NetServerTCP3Send::sendMessage(NetMessage *message) 
{
	sendMessageHandler_.addMessage(message);
}

int NetServerTCP3Send::sendThreadFunc(void *c)
{
	// Call a non-static class thread to do the processing in (just for convienience)
	NetServerTCP3Send *th = (NetServerTCP3Send*) c;
	while (true)
	{
		if (!th->actualSendFunc()) break;
	}
	th->stopped_ = true;
	return 0;
}

bool NetServerTCP3Send::actualSendFunc()
{
	// Get any new messages
	sendMessageHandler_.processMessages();
	if (outgoingMessages_.empty()) 
	{
		if (!running_) return false;

		boost::this_thread::sleep(boost::posix_time::milliseconds(10));
		return true;
	}

	// Send first message
	NetMessage *message = outgoingMessages_.front();

	// Send the length of the string
	uint32_t len = message->getBuffer().getBufferUsed();
	uint32_t netlen = 0;
	netlen = htonl(len);

	int result = boost::asio::write(*socket_, boost::asio::buffer(&netlen, sizeof(netlen)));
	if(result < (int) sizeof(netlen))
	{
		Logger::log(S3D::formatStringBuffer(
			"NetServerTCP3Send: Failed to send buffer length. Sent %i of %i.",
			result, sizeof(netlen)));
		return false;
	}
	
	// Send the buffer
	result = boost::asio::write(*socket_, boost::asio::buffer(message->getBuffer().getBuffer(), sizeof(len)));
	if(result<int(len))
	{
		Logger::log(S3D::formatStringBuffer(
			"NetServerTCP3Send: Failed to send buffer. Sent %i of %i.",
			result, int(len)));
		return false;
	}
	
	// Notify that this message has been sent
	outgoingMessages_.pop_front();
	if (message->getMessageType() == NetMessage::DisconnectMessage)
	{
		NetMessagePool::instance()->addToPool(message);
	}
	else
	{
		recieveMessageHandler_->addMessage(message);
	}
	NetInterface::getBytesOut() += len;
	bytesOut_ += len;
	messagesSent_++;

	return true;
}

void NetServerTCP3Send::processMessage(NetMessage &oldmessage)
{
	// Check if we are being disconnected
	if (oldmessage.getMessageType() == NetMessage::DisconnectMessage)
	{
		running_ = false;

		while (!outgoingMessages_.empty())
		{

			NetMessagePool::instance()->addToPool(outgoingMessages_.back());
			outgoingMessages_.pop_back();
		}
		if (oldmessage.getBuffer().getBufferUsed() == 0)
		{
			return;
		}
	}

	// Get a new buffer from the pool
	NetMessage *message = NetMessagePool::instance()->
		getFromPool(NetMessage::SentMessage, 
			oldmessage.getDestinationId(), 
			ipAddress_);

	// Copy old buffer into new buffer
	NetBuffer &buffer = oldmessage.getBuffer();
	message->getBuffer().allocate(buffer.getBufferUsed());
	memcpy(message->getBuffer().getBuffer(), 
		buffer.getBuffer(), buffer.getBufferUsed());
	message->getBuffer().setBufferUsed(buffer.getBufferUsed());

	// Add to list of outgoing
	outgoingMessages_.push_back(message);
}

void NetServerTCP3Send::wait()
{
	// Should do nothing
	sendThread_->join();	
}
