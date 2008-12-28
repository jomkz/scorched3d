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

#ifndef _ComsAddPlayerMessage_h
#define _ComsAddPlayerMessage_h

#include <coms/ComsMessage.h>
#include <common/Vector.h>

class ComsAddPlayerMessage : public ComsMessage
{
public:
	ComsAddPlayerMessage();
	ComsAddPlayerMessage(
		unsigned int playerId,
		const LangString &playerName,
		Vector playerColor,
		const char *modelName,
		const char *typeName,
		unsigned int destinationId,
		unsigned int playerTeam,
		const char *playerType);
	virtual ~ComsAddPlayerMessage();

	const LangString &getPlayerName() { return playerName_; }
	const char *getPlayerType() { return playerType_.c_str(); }
	const char *getModelName() { return modelName_.c_str(); }
	const char *getTankType() { return typeName_.c_str(); }
	unsigned int getPlayerId() { return playerId_; }
	Vector &getPlayerColor() { return playerColor_; }
	unsigned int getDestinationId() { return destinationId_; }
	unsigned int getPlayerTeam() { return playerTeam_; }

	void setPlayerIconName(const char *name) { playerIconName_ = name; }
	const char *getPlayerIconName() { return playerIconName_.c_str(); }
	NetBuffer &getPlayerIcon() { return playerIcon_; }

	// Inherited from ComsMessage
	virtual bool writeMessage(NetBuffer &buffer);
	virtual bool readMessage(NetBufferReader &reader);

protected:
	unsigned int playerId_;
	unsigned int destinationId_;
	unsigned int playerTeam_;
	LangString playerName_;
	std::string playerType_;
	std::string modelName_;
	std::string typeName_;
	std::string playerIconName_;
	Vector playerColor_;
	NetBuffer playerIcon_;

private:
	ComsAddPlayerMessage(const ComsAddPlayerMessage &);
	const ComsAddPlayerMessage & operator=(const ComsAddPlayerMessage &);

};

#endif // _ComsAddPlayerMessage_h

