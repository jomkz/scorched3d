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


#include <coms/ComsDefenseMessage.h>
#include <weapons/AccessoryStore.h>

ComsMessageType ComsDefenseMessage::ComsDefenseMessageType("ComsDefenseMessageType");

ComsDefenseMessage::ComsDefenseMessage() :
	ComsMessage(ComsDefenseMessageType),
	playerId_(0),
	change_(eNoChange),
	infoId_(0)
{
}

ComsDefenseMessage::ComsDefenseMessage(unsigned int playerId,
									   DefenseChange change,
									   unsigned int infoId) :
	ComsMessage(ComsDefenseMessageType),
	playerId_(playerId),
	change_(change), 
	infoId_(infoId)
{
}

ComsDefenseMessage::~ComsDefenseMessage()
{
}

bool ComsDefenseMessage::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(playerId_);
	buffer.addToBuffer((int) change_);
	buffer.addToBuffer(infoId_);
	return true;
}

bool ComsDefenseMessage::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(playerId_)) return false;
	int c = 0;
	if (!reader.getFromBuffer(c)) return false;
	change_ = (DefenseChange) c;
	if (!reader.getFromBuffer(infoId_)) return false;
	return true;
}
