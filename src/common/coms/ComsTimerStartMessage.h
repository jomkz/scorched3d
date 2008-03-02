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

#ifndef _comsTimerStartMessage_h
#define _comsTimerStartMessage_h

#include <coms/ComsMessage.h>

class ComsTimerStartMessage : public ComsMessage
{
public:
	ComsTimerStartMessage(int timerValue = 0);
	virtual ~ComsTimerStartMessage();

	int getTimerValue() { return timerValue_; }

	// Inherited from ComsMessage
	virtual bool writeMessage(NetBuffer &buffer);
	virtual bool readMessage(NetBufferReader &reader);

protected:
	int timerValue_;

private:
	ComsTimerStartMessage(const ComsTimerStartMessage &);
	const ComsTimerStartMessage & operator=(const ComsTimerStartMessage &);

};

#endif // _comsTimerStartMessage_h

