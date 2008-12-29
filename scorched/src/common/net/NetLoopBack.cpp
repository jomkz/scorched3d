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

#include <net/NetLoopBack.h>
#include <net/NetMessagePool.h>
#include <common/Defines.h>
#include <common/Logger.h>

static unsigned int ClientLoopBackID = 100001;
static unsigned int ServerLoopBackID = 200002;

NetLoopBack::NetLoopBack(bool server) 
	: loopback_(0), server_(server)
{
}

NetLoopBack::~NetLoopBack()
{
}

bool NetLoopBack::connect(const char *hostName, int portNo)
{
	{
		NetMessage *message = NetMessagePool::instance()->
			getFromPool(NetMessage::ConnectMessage, ServerLoopBackID, 0);
		messageHandler_.addMessage(message);
	}
	{
		NetMessage *message = NetMessagePool::instance()->
			getFromPool(NetMessage::ConnectMessage, ClientLoopBackID, 0);
		loopback_->messageHandler_.addMessage(message);
	}

	return true;
}

void NetLoopBack::setLoopBack(NetLoopBack *loopback)
{
	loopback_ = loopback;
}

bool NetLoopBack::started()
{
	return true;
}

void NetLoopBack::setMessageHandler(NetMessageHandlerI *handler) 
{ 
	messageHandler_.setMessageHandler(handler); 
}

int NetLoopBack::processMessages()
{ 
	return messageHandler_.processMessages(); 
}

void NetLoopBack::disconnectAllClients()
{
	Logger::log("Cannot disconnect all clients, they are local!");
}

void NetLoopBack::disconnectClient(unsigned int client)
{
	Logger::log(S3D::formatStringBuffer("Cannot disconnect client %i, they are local!", client));
}

void NetLoopBack::sendMessageServer(NetBuffer &buffer, 
	unsigned int flags)
{
	sendMessageDest(buffer, ServerLoopBackID, flags);
}

void NetLoopBack::sendMessageDest(NetBuffer &buffer, 
	unsigned int destination, unsigned int flags)
{
	DIALOG_ASSERT(loopback_);
	DIALOG_ASSERT(
		(server_ && destination == ClientLoopBackID) || 
		(!server_ && destination == ServerLoopBackID));

	NetMessage *message = NetMessagePool::instance()->
		getFromPool(NetMessage::BufferMessage, server_?ServerLoopBackID:ClientLoopBackID, 0, flags);
	message->getBuffer().reset();
	message->getBuffer().addDataToBuffer(buffer.getBuffer(), buffer.getBufferUsed());
	loopback_->messageHandler_.addMessage(message);
}

