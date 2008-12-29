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

#include <coms/ComsNewGameMessage.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <weapons/AccessoryStore.h>
#ifndef S3D_SERVER
#include <client/ScorchedClient.h>
#endif
#include <server/ScorchedServer.h>

ComsNewGameMessage::ComsNewGameMessage() :
	ComsMessage("ComsNewGameMessage"),
	gameStateEnclosed_(false)
{

}

ComsNewGameMessage::~ComsNewGameMessage()
{

}

void ComsNewGameMessage::addGameState()
{
	gameStateEnclosed_ = true;
}

bool ComsNewGameMessage::writeMessage(NetBuffer &buffer)
{
	if (!levelMessage_.writeMessage(buffer)) return false;
	buffer.addToBuffer(gameStateEnclosed_);
	if (gameStateEnclosed_)
	{
		if (!ScorchedServer::instance()->
			getOptionsGame().getMainOptions().writeToBuffer(buffer, false, false)) return false;
	}
	if (!ScorchedServer::instance()->
		getOptionsTransient().writeToBuffer(buffer)) return false;

	ComsPlayerStateMessage playerState(false, true);
	if (!playerState.writeMessage(playerStateMessage_)) return false;
	buffer.addToBuffer(playerStateMessage_);

	if (!ScorchedServer::instance()->getAccessoryStore().
		writeEconomyToBuffer(buffer)) return false;

	return true;
}

bool ComsNewGameMessage::readMessage(NetBufferReader &reader)
{
#ifndef S3D_SERVER
	if (!levelMessage_.readMessage(reader)) return false;
	if (!reader.getFromBuffer(gameStateEnclosed_)) return false;
	if (gameStateEnclosed_)
	{
		if (!ScorchedClient::instance()->
			getOptionsGame().getMainOptions().readFromBuffer(reader, false, false)) return false;
	}
	ScorchedClient::instance()->getOptionsGame().updateLevelOptions(
		ScorchedClient::instance()->getContext(), levelMessage_.getGroundMapsDefn());
	if (!ScorchedClient::instance()->
		getOptionsTransient().readFromBuffer(reader)) return false;
	if (!reader.getFromBuffer(playerStateMessage_)) return false;
	if (!ScorchedClient::instance()->getAccessoryStore().
		readEconomyFromBuffer(reader)) return false;
#endif

	return true;
}

bool ComsNewGameMessage::parsePlayerStateMessage()
{
	NetBufferReader reader(playerStateMessage_);
	ComsPlayerStateMessage playerState(false, true);
	if (!playerState.readMessage(reader))
	{
		return false;
	}
	return true;
}

