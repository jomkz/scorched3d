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

#include <coms/ComsAddPlayerMessage.h>

ComsAddPlayerMessage::ComsAddPlayerMessage() :
	ComsMessage("ComsAddPlayerMessage")
{
}

ComsAddPlayerMessage::ComsAddPlayerMessage(
		unsigned int playerId,
		const LangString &playerName,
		Vector playerColor,
		const char *modelName,
		const char *typeName,
		unsigned int destinationId,
		unsigned int playerTeam,
		const char *playerType) :
	ComsMessage("ComsAddPlayerMessage"),
	playerId_(playerId),
	playerName_(playerName),
	playerType_(playerType),
	playerColor_(playerColor),
	modelName_(modelName),
	typeName_(typeName),
	destinationId_(destinationId),
	playerTeam_(playerTeam)
{
}

ComsAddPlayerMessage::~ComsAddPlayerMessage()
{
}

bool ComsAddPlayerMessage::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(playerName_);
	buffer.addToBuffer(playerType_);
	buffer.addToBuffer(modelName_);
	buffer.addToBuffer(typeName_);
	buffer.addToBuffer(playerId_);
	buffer.addToBuffer(destinationId_);
	buffer.addToBuffer(playerTeam_);
	buffer.addToBuffer(playerColor_);
	buffer.addToBuffer(playerIcon_.getBufferUsed());
	if (playerIcon_.getBufferUsed() > 0)
	{
		buffer.addToBuffer(playerIconName_);
		buffer.addDataToBuffer(playerIcon_.getBuffer(), 
			playerIcon_.getBufferUsed());
	}
	return true;
}

bool ComsAddPlayerMessage::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(playerName_)) return false;
	if (!reader.getFromBuffer(playerType_)) return false;
	if (!reader.getFromBuffer(modelName_)) return false;
	if (!reader.getFromBuffer(typeName_)) return false;
	if (!reader.getFromBuffer(playerId_)) return false;
	if (!reader.getFromBuffer(destinationId_)) return false;
	if (!reader.getFromBuffer(playerTeam_)) return false;
	if (!reader.getFromBuffer(playerColor_)) return false;
	unsigned int used = 0;
	if (!reader.getFromBuffer(used)) return false;
	if (used > 0)
	{
		if (!reader.getFromBuffer(playerIconName_)) return false;
		playerIcon_.allocate(used);
		playerIcon_.setBufferUsed(used);
		if (!reader.getDataFromBuffer(playerIcon_.getBuffer(), 
			used)) return false;
	}
	return true;
}
