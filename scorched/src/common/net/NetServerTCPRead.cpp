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

#include <net/NetServerTCPRead.h>
#include <net/NetServerTCP.h>
#include <net/NetMessagePool.h>
#include <net/NetServerTCP3.h>
#include <common/Clock.h>
#include <common/Logger.h>
#include <common/Defines.h>
#include <common/ThreadUtils.h>

NetServerTCPRead::NetServerTCPRead(unsigned int id,
							 boost::asio::ip::tcp::socket *socket,
							 NetServerTCPProtocol *protocol,
							 NetMessageHandler *messageHandler,
							 bool *checkDeleted) : 
	id_(id),
	socket_(socket), protocol_(protocol), 
	checkDeleted_(checkDeleted),
	disconnect_(false), messageHandler_(messageHandler),
	sentDisconnect_(false), startCount_(0),
	ctrlThread_(0), recvThread_(0), sendThread_(0)
{
}

NetServerTCPRead::~NetServerTCPRead()
{
	outgoingMessagesMutex_.lock();
	while (!newMessages_.empty())
	{
		NetMessage *message = newMessages_.front();
		newMessages_.pop_front();
		NetMessagePool::instance()->addToPool(message);
	}
	outgoingMessagesMutex_.unlock();
}

unsigned int NetServerTCPRead::getIpAddress()
{
	return NetServerTCP3::getIpAddressFromSocket(socket_);
}

void NetServerTCPRead::start()
{
	// Send the player connected notification
	NetMessage *message = NetMessagePool::instance()->
		getFromPool(NetMessage::ConnectMessage, 
		id_,
		getIpAddress());
	messageHandler_->addMessage(message);

	recvThread_ = new boost::thread(
		NetServerTCPRead::recvThreadFunc, (void *) this);
	ThreadUtils::setThreadName(recvThread_->native_handle(), "NetServerTCPRead::recvThread");
	sendThread_ = new boost::thread(
		NetServerTCPRead::sendThreadFunc, (void *) this);
	ThreadUtils::setThreadName(sendThread_->native_handle(), "NetServerTCPRead::sendThread");
	ctrlThread_ = new boost::thread(
		NetServerTCPRead::ctrlThreadFunc, (void *) this);
	ThreadUtils::setThreadName(ctrlThread_->native_handle(), "NetServerTCPRead::ctrlThread");
}

void NetServerTCPRead::addMessage(NetMessage *message)
{
	if (message->getMessageType() != NetMessage::DisconnectMessage && 
		message->getBuffer().getBuffer() == 0) 
	{ 
		DIALOG_ASSERT(0); 
	}

	outgoingMessagesMutex_.lock();
	newMessages_.push_back(message);
	if (message->getMessageType() == NetMessage::DisconnectMessage &&
		!sentDisconnect_)
	{
		sentDisconnect_ = true;
		NetMessage *message = NetMessagePool::instance()->
			getFromPool(NetMessage::DisconnectMessage, 
				id_,
				getIpAddress());
		messageHandler_->addMessage(message);
	}
	outgoingMessagesMutex_.unlock();
}

bool NetServerTCPRead::getDisconnect()
{ 
	outgoingMessagesMutex_.lock();
	bool result = disconnect_;	
	outgoingMessagesMutex_.unlock();

	if (result)
	{
		delete ctrlThread_;
		ctrlThread_ = 0;
	}
	return result; 
}

int NetServerTCPRead::ctrlThreadFunc(void *netServerTCPRead)
{
	NetServerTCPRead *ns = (NetServerTCPRead *) netServerTCPRead;
	ns->actualCtrlThreadFunc();
	return 0;
}

int NetServerTCPRead::sendThreadFunc(void *netServerTCPRead)
{
	NetServerTCPRead *ns = (NetServerTCPRead *) netServerTCPRead;
	ns->actualSendRecvThreadFunc(true);
	return 0;
}

int NetServerTCPRead::recvThreadFunc(void *netServerTCPRead)
{
	NetServerTCPRead *ns = (NetServerTCPRead *) netServerTCPRead;
	ns->actualSendRecvThreadFunc(false);
	return 0;
}

void NetServerTCPRead::actualCtrlThreadFunc()
{
	// Ensure the other threads have started
	bool done = false;
	while (!done)
	{
		outgoingMessagesMutex_.lock();
		if (startCount_ == 2) done = true;
		outgoingMessagesMutex_.unlock();
		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	}

	// Wait for the other threads to end
	delete recvThread_;
	delete sendThread_;
	recvThread_ = 0;
	sendThread_ = 0;

	// Tidy socket
	delete socket_;
	socket_ = 0;

	// Delete self
	outgoingMessagesMutex_.lock();
	disconnect_ = true;
	*checkDeleted_ = true;
	outgoingMessagesMutex_.unlock();
}

void NetServerTCPRead::actualSendRecvThreadFunc(bool send)
{
	outgoingMessagesMutex_.lock();
	startCount_++;
	outgoingMessagesMutex_.unlock();

	Clock netClock;
	while (!sentDisconnect_)
	{
		netClock.getTimeDifference();

		if (send)
		{	
			if (!pollOutgoing()) break;
		}
		else
		{
			if (!pollIncoming()) break;
		}

		float timeDiff = netClock.getTimeDifference();
		if (timeDiff > 15.0f)
		{
			Logger::log(S3D::formatStringBuffer(
				"Warning: %s net loop took %.2f seconds, client %u", 
				(send?"Send":"Recv"),
				timeDiff, id_));
		}
	}

	outgoingMessagesMutex_.lock();
	if (!sentDisconnect_)
	{
		sentDisconnect_ = true;
		NetMessage *message = NetMessagePool::instance()->
			getFromPool(NetMessage::DisconnectMessage, 
				id_,
				getIpAddress());
		messageHandler_->addMessage(message);
	}
	outgoingMessagesMutex_.unlock();
}

bool NetServerTCPRead::pollIncoming()
{
	boost::system::error_code ec;
	if (socket_->available(ec) > 0)
	{
		NetMessage *message = protocol_->readBuffer(socket_, id_);
		if (!message)
		{
			//Logger::log( "Client socket has been closed.");
			return false;
		}
		else
		{
			if (!sentDisconnect_)
			{
				// We have a buffer containing the message
				messageHandler_->addMessage(message);
			}
			else
			{
				NetMessagePool::instance()->addToPool(message);
			}
		}
	}
	else
	{
		if (ec) return false;
		boost::this_thread::sleep(boost::posix_time::milliseconds(10));
	}

	return true;
}

bool NetServerTCPRead::pollOutgoing()
{
	NetMessage *message = 0;
	outgoingMessagesMutex_.lock();
	if (!newMessages_.empty())
	{
		message = newMessages_.front();
		newMessages_.pop_front();
	}
	outgoingMessagesMutex_.unlock();

	bool result = true;
	if (message)
	{
		if (message->getMessageType() == NetMessage::DisconnectMessage)
		{
			result = false;
		}
		else
		{
			if (!protocol_->sendBuffer(message->getBuffer(), socket_, id_))
			{
				Logger::log( "Failed to send message to client");
				result = false;
			}
		}
		message->setType(NetMessage::SentMessage);
		messageHandler_->addMessage(message);
	}
	else 
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(10));
	}

	return result;
}
