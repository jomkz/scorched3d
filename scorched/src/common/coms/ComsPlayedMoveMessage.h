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

#if !defined(__INCLUDE_ComsPlayedMoveMessageh_INCLUDE__)
#define __INCLUDE_ComsPlayedMoveMessageh_INCLUDE__

#include <coms/ComsMessage.h>

class ComsPlayedMoveMessage : public ComsMessage
{
public:
	enum MoveType
	{
		eNone,
		eShot,
		eResign,
		eSkip,
		eFinishedBuy
	};

	ComsPlayedMoveMessage(unsigned int playerId = 0,
		MoveType type = eNone);
	virtual ~ComsPlayedMoveMessage();

	void setShot(unsigned int weaponId,
		fixed rotationXY,
		fixed rotationYZ,
		fixed power,
		int positionX, 
		int positionY);

	unsigned int getPlayerId() { return playerId_; }
	unsigned int getWeaponId() { return weaponId_; }
	fixed getRotationXY() { return rotationXY_; }
	fixed getRotationYZ() { return rotationYZ_; }
	int getSelectPositionX() { return selectPositionX_; }
	int getSelectPositionY() { return selectPositionY_; }
	fixed getPower() { return power_; }
	MoveType getType() { return moveType_; }

	// Inherited from ComsMessage
    virtual bool writeMessage(NetBuffer &buffer);
    virtual bool readMessage(NetBufferReader &reader);

protected:
	unsigned int playerId_;
	unsigned int weaponId_;
	MoveType moveType_;
	fixed rotationXY_;
	fixed rotationYZ_;
	fixed power_;
	int selectPositionX_;
	int selectPositionY_;

private:
	ComsPlayedMoveMessage(const ComsPlayedMoveMessage &);
	const ComsPlayedMoveMessage & operator=(const ComsPlayedMoveMessage &);
};

#endif
