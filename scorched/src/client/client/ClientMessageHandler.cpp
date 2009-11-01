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

#include <client/ClientMessageHandler.h>
#include <client/ClientState.h>
#include <client/ScorchedClient.h>
#include <dialogs/ConnectDialog.h>
#include <tank/TankContainer.h>
#include <common/Logger.h>
#include <net/NetInterface.h>

ClientMessageHandler *ClientMessageHandler::instance_ = 0;

ClientMessageHandler *ClientMessageHandler::instance()
{
	if (!instance_)
	{
		instance_ = new ClientMessageHandler;
	}
	return instance_;
}

ClientMessageHandler::ClientMessageHandler()
{
}

ClientMessageHandler::~ClientMessageHandler()
{
}

void ClientMessageHandler::messageRecv(unsigned int destinationId)
{
}

void ClientMessageHandler::messageSent(unsigned int destinationId)
{
}

void ClientMessageHandler::clientConnected(NetMessage &message)
{
	ConnectDialog::instance()->connected();
}

void ClientMessageHandler::clientDisconnected(NetMessage &message)
{
	const char *type = "";
	if (message.getFlags() == NetMessage::TimeoutDisconnect)
	{
		type = "Timeout";
	}
	else if (message.getFlags() == NetMessage::UserDisconnect)
	{
		type = "User";
	}

	Logger::log(S3D::formatStringBuffer("Disconnected %s", type));
	ScorchedClient::instance()->getGameState().stimulate(ClientState::StimDisconnected);
	ScorchedClient::instance()->getTankContainer().setCurrentDestinationId(0);
}

void ClientMessageHandler::clientError(NetMessage &message,
	const std::string &errorString)
{
	Logger::log(S3D::formatStringBuffer("***Client Error*** \"%s\"", errorString.c_str()));
	ScorchedClient::instance()->getNetInterface().disconnectAllClients();
}
