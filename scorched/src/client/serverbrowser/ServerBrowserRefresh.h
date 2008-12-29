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

#if !defined(__INCLUDE_ServerBrowserRefreshh_INCLUDE__)
#define __INCLUDE_ServerBrowserRefreshh_INCLUDE__

#include <serverbrowser/ServerBrowserServerList.h>
#include <map>
#include <time.h>

struct ServerBrowserRefreshEntry
{
	ServerBrowserRefreshEntry();
	ServerBrowserRefreshEntry(const char *address, int position);

	int retries_;
	int sentTime_;
	int recieved_;
	std::string address_;
	int position_;
};

class ServerBrowserRefresh
{
public:
	ServerBrowserRefresh(ServerBrowserServerList &list);
	virtual ~ServerBrowserRefresh();

	void refreshList();

	void setCancel(bool cancel) { cancel_ = cancel; }

protected:
	bool cancel_;
	ServerBrowserServerList &list_;
	std::list<ServerBrowserRefreshEntry> refreshEntries_;
	std::map<UDPsocket, ServerBrowserRefreshEntry> entryMap_;
	UDPpacket *sendPacketStatus_;
	UDPpacket *sendPacketPlayers_;
	UDPpacket *recvPacket_;

	void sendNextEntry(ServerBrowserRefreshEntry &entry, time_t theTime);
	void processMessages(time_t theTime);
	void processMessage(UDPpacket *packet, ServerBrowserRefreshEntry &entry);
};

#endif
