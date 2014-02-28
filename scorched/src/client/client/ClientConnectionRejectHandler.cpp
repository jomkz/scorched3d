////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2013
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

#include <client/ClientConnectionRejectHandler.h>
#include <client/ScorchedClient.h>
#include <lang/LangResource.h>
#include <coms/ComsConnectRejectMessage.h>
#include <common/Logger.h>

ClientConnectionRejectHandler::ClientConnectionRejectHandler(ComsMessageHandler &comsMessageHandler)
{
	comsConnectRejectMessageTypeAdapter_ =
		new ComsMessageHandlerIAdapter<ClientConnectionRejectHandler>(
			this, &ClientConnectionRejectHandler::processMessage,
			ComsConnectRejectMessage::ComsConnectRejectMessageType,
			comsMessageHandler);
}

ClientConnectionRejectHandler::~ClientConnectionRejectHandler()
{
	delete comsConnectRejectMessageTypeAdapter_;
}

bool ClientConnectionRejectHandler::processMessage(
	NetMessage &netNessage,
	NetBufferReader &reader)
{
	ComsConnectRejectMessage message;
	if (!message.readMessage(reader)) return false;

	LangString msg = LANG_RESOURCE_1("DISCONNECTED",
		"Disconnected from server due to : {0}", message.getText());
	Logger::log(LangStringUtil::convertFromLang(msg));
	return true;
}
