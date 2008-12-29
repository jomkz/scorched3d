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

#include <coms/ComsPlayMovesMessage.h>

ComsPlayMovesMessage::ComsPlayMovesMessage() :
	ComsMessage("ComsPlayMovesMessage"),
	playerState_(true, true)
{
}

ComsPlayMovesMessage::~ComsPlayMovesMessage()
{
}

bool ComsPlayMovesMessage::writeMessage(NetBuffer &buffer)
{
	if (!playerState_.writeMessage(buffer)) return false;

	int size = (int) moves_.size();
	buffer.addToBuffer(size);

	std::map<unsigned int, ComsPlayedMoveMessage *>::iterator itor;
	for (itor = moves_.begin();
		itor != moves_.end();
		itor++)
	{
		unsigned int playerId = (*itor).first;
		ComsPlayedMoveMessage *message = (*itor).second;

		buffer.addToBuffer(playerId);
		if (!message->writeMessage(buffer)) return false;
	}
	buffer.addToBuffer(seed_);
	return true;
}

bool ComsPlayMovesMessage::readMessage(NetBufferReader &reader)
{
	if (!playerState_.readMessage(reader)) return false;

	int size = 0;
	if (!reader.getFromBuffer(size)) return false;

	for (int s=0; s<size; s++)
	{
		ComsPlayedMoveMessage *message = new ComsPlayedMoveMessage();
		unsigned int playerId = 0;

		if (!reader.getFromBuffer(playerId)) return false;
		if (!message->readMessage(reader)) return false;

		moves_[playerId] = message;
	}
	if (!reader.getFromBuffer(seed_)) return false;
	return true;
}
