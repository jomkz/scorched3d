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

#if !defined(__INCLUDE_ComsGiftMoneyMessageh_INCLUDE__)
#define __INCLUDE_ComsGiftMoneyMessageh_INCLUDE__

#include <coms/ComsMessage.h>
#include <string>

class ComsGiftMoneyMessage : public ComsMessage
{
public:
	static ComsMessageType ComsGiftMoneyMessageType;

	ComsGiftMoneyMessage(
		unsigned int fromPlayerId = 0,
		unsigned int toPlayerId = 0,
		int money = 0);
	virtual ~ComsGiftMoneyMessage();

	unsigned int getFromPlayerId() { return fromPlayerId_; }
	unsigned int getToPlayerId() { return toPlayerId_; }
	unsigned int getMoney() { return money_; }

	// Inherited from ComsMessage
    virtual bool writeMessage(NetBuffer &buffer);
    virtual bool readMessage(NetBufferReader &reader);

protected:
	unsigned int fromPlayerId_;
	unsigned int toPlayerId_;
	unsigned int money_;
};

#endif
