////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011
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
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
////////////////////////////////////////////////////////////////////////////////

#if !defined(__INCLUDE_NetServerTCPReadh_INCLUDE__)
#define __INCLUDE_NetServerTCPReadh_INCLUDE__

#include <net/NetInterface.h>
#include <net/NetServerTCPProtocol.h>

class NetServerTCPRead
{
public:
	NetServerTCPRead(unsigned int id,
		boost::asio::ip::tcp::socket *socket,
		NetServerTCPProtocol *protocol,
		NetMessageHandler *messageHandler,
		bool *checkDeleted);
	virtual ~NetServerTCPRead();

	void start();
	bool getDisconnect();
	void addMessage(NetMessage *message);
	unsigned int getIpAddress();

protected:
	unsigned int id_;
	bool *checkDeleted_;
	bool disconnect_, sentDisconnect_;
	boost::asio::ip::tcp::socket *socket_;
	NetServerTCPProtocol *protocol_;
	NetMessageHandler *messageHandler_;
	boost::mutex outgoingMessagesMutex_;
	boost::thread *sendThread_;
	boost::thread *recvThread_;
	boost::thread *ctrlThread_;
	std::list<NetMessage *> newMessages_;
	unsigned int startCount_;

	void actualCtrlThreadFunc();
	void actualSendRecvThreadFunc(bool send);
	bool pollOutgoing();
	bool pollIncoming();
	static int ctrlThreadFunc(void *);
	static int sendThreadFunc(void *);
	static int recvThreadFunc(void *);
};

#endif
