////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
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

#if !defined(__INCLUDE_ClientKeepAliveSenderh_INCLUDE__)
#define __INCLUDE_ClientKeepAliveSenderh_INCLUDE__

#include <coms/ComsMessageHandler.h>

class ClientKeepAliveSender
{
public:
	static ClientKeepAliveSender *instance();

	void sendKeepAlive();

protected:
	static ClientKeepAliveSender *instance_;
	static int sendThreadFunc(void *);
	unsigned int lastSendTime_;
	SDL_Thread *sendThread_;
	bool recvMessage_;
	NetBuffer buffer_;

	void send();

private:
	ClientKeepAliveSender();
	virtual ~ClientKeepAliveSender();
};

#endif // __INCLUDE_ClientKeepAliveSenderh_INCLUDE__
