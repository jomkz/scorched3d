////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
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


#include <net/NetMessageHandler.h>
#include <net/NetMessagePool.h>

NetMessageHandlerI::~NetMessageHandlerI()
{
}

NetMessageHandler::NetMessageHandler() : 
	handler_(0), messagesMutex_(0), messagesWaiting_(false)
{
	messagesMutex_ = SDL_CreateMutex();
}

NetMessageHandler::~NetMessageHandler()
{
	SDL_LockMutex(messagesMutex_);
	while (!messages_.empty())
	{
		NetMessage *message = messages_.front();
		messages_.pop_front();
		NetMessagePool::instance()->addToPool(message);
	}	
	SDL_UnlockMutex(messagesMutex_);

	SDL_DestroyMutex(messagesMutex_);
}

void NetMessageHandler::addMessage(NetMessage *message)
{
	SDL_LockMutex(messagesMutex_);
	messages_.push_back(message);
	messagesWaiting_ = true;
	SDL_UnlockMutex(messagesMutex_);
}

int NetMessageHandler::processMessages()
{
	if (!messagesWaiting_) return 0;

	// Get the list of messages that should be processed
	// Process one at a time, incase this method is called re-entrantly
	NetMessage *message = 0;
	SDL_LockMutex(messagesMutex_);
	if (!messages_.empty())
	{
		message = messages_.front();
		messages_.pop_front();
	}
	messagesWaiting_ = !messages_.empty();
	SDL_UnlockMutex(messagesMutex_);

	int result = 0;
	if (message)
	{
		result ++;

		// Call user to process message
		if (handler_) handler_->processMessage(*message);
		
		// Delete message
		NetMessagePool::instance()->addToPool(message);
	}
	return result;
}

void NetMessageHandler::setMessageHandler(NetMessageHandlerI *handler)
{
	handler_ = handler;
}
