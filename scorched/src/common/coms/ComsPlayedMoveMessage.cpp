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

#include <coms/ComsPlayedMoveMessage.h>

ComsMessageType ComsPlayedMoveMessage::ComsPlayedMoveMessageType("ComsPlayedMoveMessageType");

ComsPlayedMoveMessage::ComsPlayedMoveMessage(unsigned int playerId, MoveType type) :
	ComsMessage(ComsPlayedMoveMessageType),
	moveType_(type),
	weaponId_(0),
	rotationXY_(0), rotationYZ_(0), power_(0), playerId_(playerId),
	selectPositionX_(0), selectPositionY_(0)
{
}

ComsPlayedMoveMessage::~ComsPlayedMoveMessage()
{
}

void ComsPlayedMoveMessage::setShot(unsigned int weaponId,
	fixed rotationXY,
	fixed rotationYZ,
	fixed power,
	int selectPositionX, 
	int selectPositionY)
{
	weaponId_ = weaponId;
	rotationXY_ = rotationXY;
	rotationYZ_ = rotationYZ;
	power_ = power;
	selectPositionX_ = selectPositionX;
	selectPositionY_ = selectPositionY;
}

bool ComsPlayedMoveMessage::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(playerId_);
	buffer.addToBuffer((int) moveType_);
	if (moveType_ == eShot)
	{
		buffer.addToBuffer(weaponId_);
		buffer.addToBuffer(rotationXY_);
		buffer.addToBuffer(rotationYZ_);
		buffer.addToBuffer(power_);
		buffer.addToBuffer(selectPositionX_);
		buffer.addToBuffer(selectPositionY_);
	}
	return true;
}

bool ComsPlayedMoveMessage::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(playerId_)) return false;
	int mt;
	if (!reader.getFromBuffer(mt)) return false;
	moveType_ = (MoveType) mt;
	if (moveType_ == eShot)
	{
		if (!reader.getFromBuffer(weaponId_)) return false;
		if (!reader.getFromBuffer(rotationXY_)) return false;
		if (!reader.getFromBuffer(rotationYZ_)) return false;
		if (!reader.getFromBuffer(power_)) return false;
		if (!reader.getFromBuffer(selectPositionX_)) return false;
		if (!reader.getFromBuffer(selectPositionY_)) return false;
	}
	return true;
}

