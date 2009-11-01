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

#ifndef _ClientLoadLevelHandler_h
#define _ClientLoadLevelHandler_h

#include <coms/ComsMessageHandler.h>

class ClientLoadLevelHandler : public ComsMessageHandlerI
{
public:
	static ClientLoadLevelHandler* instance();

	virtual bool processMessage(
		NetMessage &message,
		const char *messageType,
		NetBufferReader &reader);

protected:
	static ClientLoadLevelHandler* instance_;

	bool actualProcessMessage(
		NetMessage &netMessage,
		const char *messageType,
		NetBufferReader &reader);

private:
	ClientLoadLevelHandler();
	virtual ~ClientLoadLevelHandler();

	ClientLoadLevelHandler(const ClientLoadLevelHandler &);
	const ClientLoadLevelHandler & operator=(const ClientLoadLevelHandler &);

};

#endif // _ClientLoadLevelHandler_h

