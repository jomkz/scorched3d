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

#if !defined(__INCLUDE_ComsMessageHandlerh_INCLUDE__)
#define __INCLUDE_ComsMessageHandlerh_INCLUDE__

#include <net/NetMessageHandler.h>
#include <coms/ComsMessage.h>
#include <vector>

class ComsMessageConnectionHandlerI
{
public:
	virtual ~ComsMessageConnectionHandlerI();

	virtual void clientConnected(NetMessage &message) = 0;
	virtual void clientDisconnected(NetMessage &message) = 0;
	virtual void clientError(NetMessage &message,
		const std::string &errorString) = 0;

	virtual void messageRecv(unsigned int destinationId) = 0;
	virtual void messageSent(unsigned int destinationId) = 0;
};

class ComsMessageHandlerI
{
public:
	virtual ~ComsMessageHandlerI();

	virtual bool processMessage(
		NetMessage &message,
		const char *messageType,
		NetBufferReader &reader) = 0;
};

class ComsMessageHandler : public NetMessageHandlerI
{
public:
	ComsMessageHandler(const char *instanceName = "");
	virtual ~ComsMessageHandler();

	// Used to add a handler for a specific message type
	void setConnectionHandler(
		ComsMessageConnectionHandlerI *handler);
	void addHandler(ComsMessageType &comsMessageType,
		ComsMessageHandlerI *handler);
	void addSentHandler(ComsMessageType &comsMessageType,
		ComsMessageHandlerI *handler);

	// Inherited from NetMessageHandlerI
	virtual void processMessage(NetMessage &message);

	bool &getMessageLogging() { return comsMessageLogging_; }

protected:
	std::string instanceName_;
	std::vector<ComsMessageHandlerI *> recvHandlers_;
	std::vector<ComsMessageHandlerI *> sentHandlers_;
	ComsMessageConnectionHandlerI *connectionHandler_;

	bool comsMessageLogging_;
	void processReceiveMessage(NetMessage &message);
	void processSentMessage(NetMessage &message);
	void processMessage(NetMessage &message,
		std::vector<ComsMessageHandlerI *> &handlers,
		const char *sendRecv);
};

// Used as in REGISTER_HANDLER(
//	"MyComsMessage", 
//	ComsMessageHandlerIRegistration::eClient, 
//	new MyComsMessageHandler)
#define REGISTER_HANDLER(name, type, y, z) \
	struct HANDLER_##name { HANDLER_##name() { ComsMessageHandlerIRegistration::addHandler(y, type, z); } }; \
	static HANDLER_##name HANDLER_IMPL_##name;

class ComsMessageHandlerIRegistration
{
public:
	enum HandlerType
	{
		eClient,
		eServer
	};
	struct HandlerInfo
	{
		HandlerType type;
		ComsMessageType *messageType;
		ComsMessageHandlerI *handler;
	};

	static void addHandler(HandlerType type,
			ComsMessageType &messageType,
			ComsMessageHandlerI *handler);
	static std::list<HandlerInfo> *handlerList;
	static void registerHandlers(HandlerType type, ComsMessageHandler &handler);
};

#endif
