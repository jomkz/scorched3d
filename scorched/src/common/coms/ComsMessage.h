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


#ifndef _comsMessage_h
#define _comsMessage_h

#include <string>
#include <net/NetMessage.h>

class ComsMessage
{
public:
	ComsMessage(const char *type);
	virtual ~ComsMessage();

	virtual bool writeMessage(NetBuffer &buffer) = 0;
	virtual bool readMessage(NetBufferReader &reader) = 0;

	// The string that defines the type of coms message
	bool writeTypeMessage(NetBuffer &buffer);
	bool readTypeMessage(NetBufferReader &reader);
	const char *getMessageType() { return type_.c_str(); }

protected:
	std::string type_;

private:
	ComsMessage(const ComsMessage &);
	const ComsMessage & operator=(const ComsMessage &);

};

#endif // _comsMessage_h

