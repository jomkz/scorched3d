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

#ifndef _comsMessageSender_h
#define _comsMessageSender_h

#include <list>
#include <coms/ComsMessage.h>
#include <net/NetInterfaceFlags.h>

class ComsMessageSender
{
public:
	static bool sendToServer(
		ComsMessage &message, unsigned int flags = 0);
	static bool sendToSingleClient(
		ComsMessage &message, unsigned int destination, unsigned int flags = 0);
	static bool sendToMultipleClients(
		ComsMessage &message, std::list<unsigned int> destinations, unsigned int flags = 0);
	static bool sendToAllPlayingClients(
		ComsMessage &message, unsigned int flags = 0);
	static bool sendToAllConnectedClients(
		ComsMessage &message, unsigned int flags = 0);

protected:

	static bool formMessage(
		ComsMessage &message);

private:
	ComsMessageSender();
	virtual ~ComsMessageSender();

};

#endif // _comsMessageSender_h

