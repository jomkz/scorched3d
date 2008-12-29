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

#if !defined(__INCLUDE_ServerBrowserCollecth_INCLUDE__)
#define __INCLUDE_ServerBrowserCollecth_INCLUDE__

#include <net/NetServerTCP.h>
#include <serverbrowser/ServerBrowserServerList.h>
#include <set>

class ServerBrowserCollect : public NetMessageHandlerI
{
public:
	ServerBrowserCollect(ServerBrowserServerList &list);
	virtual ~ServerBrowserCollect();

	// Fetch the new list from the server
	bool fetchServerList(
		const char *masterListServer,
		const char *masterListServerURI);
	bool fetchLANList();
	bool fetchFavoritesList();

	std::set<std::string> getFavourites();
	void setFavourites(std::set<std::string> &favs);

	void setCancel(bool cancel) { cancel_ = cancel; }

	// Inherited from NetMessageHandlerI
	virtual void processMessage(NetMessage &message);

protected:
	ServerBrowserServerList &list_;
	bool cancel_;
	bool complete_;
	UDPpacket *sendPacket_;
	UDPpacket *recvPacket_;
	NetServerTCP netServer_;
	NetBuffer sendNetBuffer_;
};

#endif // __INCLUDE_ServerBrowserCollecth_INCLUDE__
