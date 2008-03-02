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

#include <coms/ComsPlayerStatusMessage.h>

ComsPlayerStatusMessage::ComsPlayerStatusMessage() :
	ComsMessage("ComsPlayerStatusMessage")
{

}

ComsPlayerStatusMessage::~ComsPlayerStatusMessage()
{

}

bool ComsPlayerStatusMessage::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer((unsigned int) waitingPlayers_.size());
	std::list<unsigned int>::iterator itor;
	for (itor = waitingPlayers_.begin();
		 itor != waitingPlayers_.end();
		 itor++)
	{
		unsigned int player = *itor;
		buffer.addToBuffer(player);
	}
	return true;
}

bool ComsPlayerStatusMessage::readMessage(NetBufferReader &reader)
{
	waitingPlayers_.clear();
	unsigned int size = 0;
	if (!reader.getFromBuffer(size)) return false;
	for (unsigned int i=0; i<size; i++)
	{
		unsigned int player = 0;
		if (!reader.getFromBuffer(player)) return false;
		waitingPlayers_.push_back(player);
	}

	return true;
}

bool ComsPlayerStatusMessage::playerWaiting(unsigned int playerId)
{
	std::list<unsigned int>::iterator itor;
	for (itor = waitingPlayers_.begin();
		 itor != waitingPlayers_.end();
		 itor++)
	{
		unsigned int player = *itor;
		if (player == playerId) return true;
	}
	return false;
}
