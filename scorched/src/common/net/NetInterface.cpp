////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2013
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

#include <net/NetInterface.h>
#include <common/Defines.h>

unsigned int NetInterface::bytesIn_ = 0;
unsigned int NetInterface::bytesOut_ = 0;
unsigned int NetInterface::pings_ = 0;
unsigned int NetInterface::connects_ = 0;

NetInterface::NetInterface()
{
}

NetInterface::~NetInterface()
{
}

const char *NetInterface::getIpName(unsigned int ipAddress)
{
	static char result[128];
	snprintf(result, 128, "%u.%u.%u.%u", 
		(ipAddress & 0xFF000000) >> 24,
		(ipAddress & 0x00FF0000) >> 16,
		(ipAddress & 0x0000FF00) >> 8,
		(ipAddress & 0x000000FF) >> 0);
	return result;
}

unsigned int NetInterface::getIpAddressFromName(const char *name)
{
	boost::asio::io_service io_service;
	boost::asio::ip::tcp::resolver resolver(io_service);

	boost::asio::ip::tcp::resolver::query query(
	boost::asio::ip::tcp::v4(), name, "27270");
		boost::asio::ip::tcp::resolver::iterator end, endpoint_iterator = resolver.resolve(query);

	if (endpoint_iterator != end)
	{
		boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
		unsigned int result = endpoint.address().to_v4().to_ulong();
		return result;
	}
	return 0;
}

unsigned int NetInterface::getIpAddressFromSocket(boost::asio::ip::tcp::socket *socket)
{
	boost::asio::ip::tcp::endpoint endpoint = socket->remote_endpoint();
	unsigned int addr = endpoint.address().to_v4().to_ulong();
	return addr;
}
