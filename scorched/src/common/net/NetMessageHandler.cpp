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


#include <net/NetMessageHandler.h>
#include <net/NetMessagePool.h>

NetMessageHandlerI::~NetMessageHandlerI()
{
}

NetMessageHandler::NetMessageHandler() : 
	handler_(0), messagesWaiting_(false)
{
}

NetMessageHandler::~NetMessageHandler()
{
	messagesMutex_.lock();
	while (!messages_.empty())
	{
		NetMessage *message = messages_.front();
		messages_.pop_front();
		NetMessagePool::instance()->addToPool(message);
	}	
	messagesMutex_.unlock();
}

void NetMessageHandler::addMessage(NetMessage *message)
{
	messagesMutex_.lock();
	messages_.push_back(message);
	messagesWaiting_ = true;
	messagesMutex_.unlock();
}

int NetMessageHandler::processMessages()
{
	int result = 0;
	while (processSingleMessage())
	{
		result++;
	}
	return result;
}

bool NetMessageHandler::processSingleMessage()
{
	if (!messagesWaiting_) return false;

	// Get the list of messages that should be processed
	// Process one at a time, incase this method is called re-entrantly
	NetMessage *message = 0;
	messagesMutex_.lock();
	if (!messages_.empty())
	{
		message = messages_.front();
		messages_.pop_front();
	}
	messagesWaiting_ = !messages_.empty();
	messagesMutex_.unlock();

	if (message)
	{
		// Call user to process message
		if (handler_) handler_->processMessage(*message);
		
		// Delete message
		NetMessagePool::instance()->addToPool(message);
	}
	return (message != 0);
}

void NetMessageHandler::setMessageHandler(NetMessageHandlerI *handler)
{
	handler_ = handler;
}
