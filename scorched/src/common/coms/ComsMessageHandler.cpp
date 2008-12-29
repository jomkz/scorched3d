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

#include <coms/ComsMessageHandler.h>
#include <common/Defines.h>
#include <common/Logger.h>

ComsMessageConnectionHandlerI::~ComsMessageConnectionHandlerI()
{
}

ComsMessageHandlerI::~ComsMessageHandlerI()
{
}

ComsMessageHandlerSentI::~ComsMessageHandlerSentI()
{
}

ComsMessageHandler::ComsMessageHandler(const char *instanceName) : 
	instanceName_(instanceName),
	connectionHandler_(0), comsMessageLogging_(false)
{
}

ComsMessageHandler::~ComsMessageHandler()
{
}

void ComsMessageHandler::setConnectionHandler(
		ComsMessageConnectionHandlerI *handler)
{
	connectionHandler_ = handler;
}

void ComsMessageHandler::addHandler(const char *messageType,
		ComsMessageHandlerI *handler)
{
	std::map<std::string, ComsMessageHandlerI *>::iterator itor =
		recvHandlerMap_.find(messageType);
	DIALOG_ASSERT(itor == recvHandlerMap_.end());

	recvHandlerMap_[messageType] = handler;
}

void ComsMessageHandler::addSentHandler(const char *messageType,
		ComsMessageHandlerSentI *handler)
{
	std::map<std::string, ComsMessageHandlerSentI *>::iterator itor =
		sentHandlerMap_.find(messageType);
	DIALOG_ASSERT(itor == sentHandlerMap_.end());

	sentHandlerMap_[messageType] = handler;
}

void ComsMessageHandler::processMessage(NetMessage &message)
{
	switch(message.getMessageType())
	{
		case NetMessage::BufferMessage:
			if (connectionHandler_) 
				connectionHandler_->messageRecv(message.getDestinationId());
			processReceiveMessage(message);
			break;
		case NetMessage::SentMessage:
			if (connectionHandler_) 
				connectionHandler_->messageSent(message.getDestinationId());
			processSentMessage(message);
			break;
		case NetMessage::DisconnectMessage:
			if (comsMessageLogging_)
			{
				Logger::log(S3D::formatStringBuffer("ComsMessage::Disconnected(%i, %i)",
					message.getDestinationId(), message.getFlags()));
			}

			if (connectionHandler_) 
				connectionHandler_->clientDisconnected(message);
			break;
		case NetMessage::ConnectMessage:
			if (comsMessageLogging_)
			{
				Logger::log(S3D::formatStringBuffer("ComsMessage::Connected(%i)",
					message.getDestinationId()));
			}

			if (connectionHandler_)
				connectionHandler_->clientConnected(message);
			break;
		default:
			if (connectionHandler_)
				connectionHandler_->clientError(message,
					"Failed to recognise message type");
			break;
	}
}

void ComsMessageHandler::processReceiveMessage(NetMessage &message)
{
	unsigned int bufferUsed = message.getBuffer().getBufferUsed();
	if (!message.getBuffer().uncompressBuffer())
	{
		if (connectionHandler_)
			connectionHandler_->clientError(message,
				"Failed to uncompress RECV message type");
	}
	NetBufferReader reader(message.getBuffer());

	std::string messageType;
	if (!reader.getFromBuffer(messageType))
	{
		if (connectionHandler_)
			connectionHandler_->clientError(message,
				"Failed to decode RECV message type");
		return;
	}

	if (comsMessageLogging_)
	{
		Logger::log(S3D::formatStringBuffer("%s::process(%s, %i, %u)",
			instanceName_.c_str(),
			messageType.c_str(), message.getDestinationId(),
			bufferUsed));
	}

	std::map<std::string, ComsMessageHandlerI *>::iterator itor =
		recvHandlerMap_.find(messageType);
	if (itor == recvHandlerMap_.end())
	{
		char buffer[1024];
		snprintf(buffer, 1024, "Failed to find RECV message type handler \"%s\"",
			messageType.c_str());

		if (connectionHandler_)
			connectionHandler_->clientError(message,
				buffer);
		return;
	}

	ComsMessageHandlerI *handler = (*itor).second;
	const char *messageTypeStr = messageType.c_str();
	if (!handler->processMessage(
		message, messageTypeStr, reader))
	{
		char buffer[1024];
		snprintf(buffer, 1024, "Failed to handle RECV message type \"%s\"",
			messageType.c_str());

		if (connectionHandler_)
			connectionHandler_->clientError(message,
				buffer);
		return;
	}

	if (comsMessageLogging_)
	{
		Logger::log(S3D::formatStringBuffer("%s::processFinished(%s, %i)",
			instanceName_.c_str(),
			messageType.c_str(), message.getDestinationId()));
	}
}

void ComsMessageHandler::processSentMessage(NetMessage &message)
{
	if (sentHandlerMap_.empty()) return;

	message.getBuffer().uncompressBuffer();
	NetBufferReader reader(message.getBuffer());

	std::string messageType;
	if (!reader.getFromBuffer(messageType))
	{
		if (connectionHandler_)
			connectionHandler_->clientError(message,
				"Failed to decode SENT message type");
		return;
	}

	if (comsMessageLogging_)
	{
		Logger::log(S3D::formatStringBuffer("%s::processSentMessage(%s, %i)",
			instanceName_.c_str(),
			messageType.c_str(), message.getDestinationId()));
	}

	std::map<std::string, ComsMessageHandlerSentI *>::iterator itor =
		sentHandlerMap_.find(messageType);
	if (itor == sentHandlerMap_.end()) return;

	ComsMessageHandlerSentI *handler = (*itor).second;
	const char *messageTypeStr = messageType.c_str();
	if (!handler->processSentMessage(message.getDestinationId(), 
		messageTypeStr, reader))
	{
		char buffer[1024];
		snprintf(buffer, 1024, "Failed to handle SENT message type \"%s\"",
			messageType.c_str());

		if (connectionHandler_)
			connectionHandler_->clientError(message,
				buffer);
		return;
	}
}
