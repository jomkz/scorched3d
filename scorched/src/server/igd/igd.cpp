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

#include <igd/igd.h>
#include <common/Logger.h>
#include <XML/XMLStringBuffer.h>

static const char *UPNP_MCAST_ADDR = "239.255.255.250";
static unsigned int PORT = 1900;

// Used to auto free packets
class Packet
{
public:
	Packet()
	{
		packet = SDLNet_AllocPacket(10000);
	}
	~Packet()
	{
		SDLNet_FreePacket(packet);
	}

	UDPpacket *packet;
};

igd::igd()
{
}

igd::~igd()
{
}

void igd::sendInitialRequest()
{
	UDPsocket udpsock = SDLNet_UDP_Open(0);
	if (!udpsock)
	{
		Logger::log("igd::Failed to create client UDP socket");
	}

	// Perform SSDP discovery
	std::list<Location> locations;
	sendInitialRequest(udpsock);
	recvInitialRequest(udpsock, locations);
	SDLNet_UDP_Close(udpsock);

	// For each device discovered try to get the service definition
	std::list<Location>::iterator itor;
	for (itor = locations.begin(); itor!= locations.end(); itor++)
	{
		Location &location = *itor;
		sendServiceRequest(location);
		if (!location.serviceData.empty())
		{
			XMLStringBuffer xmlDocument;
			if (xmlDocument.create(location.serviceData.c_str(), location.serviceData.size()))
			{
				Logger::log("OK");
			}
		}
	}
}

void igd::sendInitialRequest(UDPsocket udpsock)
{
	std::string buffer = S3D::formatStringBuffer(
		"M-SEARCH * HTTP/1.1\r\n"
		"HOST: %s:%i\r\n"
		"ST: %s\r\n"
		"MAN: \"ssdp:discover\"\r\n"
		"MX: 2\r\n" // Seconds to delay response from device
		"\r\n",
		UPNP_MCAST_ADDR, PORT, 
		"urn:schemas-upnp-org:device:InternetGatewayDevice:1");

	int len = buffer.size() + 1;
	Packet packet;
	memcpy(packet.packet->data, buffer.c_str(), len);
	packet.packet->len = len;
	packet.packet->channel = -1;
	SDLNet_ResolveHost(&packet.packet->address, UPNP_MCAST_ADDR, PORT);

	if (SDLNet_UDP_Send(udpsock, -1, packet.packet) == 0)
	{
		Logger::log("igd::Failed to send IGD ssdp discover");
	}
}

static bool parseInitialItem(const char *data, const char *name, std::string &result)
{
	const char *pos = strstr(data, name);
	if (!pos) return false;
	pos += strlen(name);

	const char *end = strchr(pos, '\n');
	if (!end) return false;

	result = std::string(pos, end - pos);
	return true;
}

static bool parseUrl(const char *data, std::string &host, int &port, std::string &path)
{
	const char *http = "http://";
	const char *hostPos = strstr(data, http);
	if (!hostPos) return false;
	hostPos += strlen(http);

	const char *portPos = strchr(hostPos, ':');
	if (!portPos) return false;
	((char *)portPos)[0] = '\0';
	portPos++;
	const char *urlPos = strchr(portPos, '/');
	if (!urlPos) return false;
	const char *endPos = strchr(urlPos, '\r');
	if (!endPos) return false;
	((char *)endPos)[0] = '\0';

	host = hostPos;
	path = urlPos;

	((char *)urlPos)[0] = '\0';
	port = atoi(portPos);
	return true;
}

void igd::recvInitialRequest(UDPsocket udpsock, std::list<Location> &locations)
{
	Packet packet;
	for (int i=0; i<20; i++)
	{
		SDL_Delay(100);
		if (SDLNet_UDP_Recv(udpsock, packet.packet) == 1)
		{
			const char *data = (const char *) packet.packet->data;
			Location location;
			if (parseInitialItem(data, "\nLOCATION: ", location.location) &&
				parseInitialItem(data, "\nST: ", location.st) &&
				parseUrl(location.location.c_str(), location.host, location.port, location.path))
			{
				locations.push_back(location);
			}
		}
	}
}

void igd::sendServiceRequest(Location &location)
{
	if (SDLNet_ResolveHost(&location.ipAddress, location.host.c_str(), location.port) != 0)
	{
		Logger::log(S3D::formatStringBuffer("igd::Failed to resolve host and port, %s:%i", 
			location.host.c_str(), location.port));
		return;		
	}

	TCPsocket tcpsock = SDLNet_TCP_Open(&location.ipAddress);
	if (!tcpsock) 
	{
		Logger::log(S3D::formatStringBuffer("igd::Failed to connect to host and port, %s:%i", 
			location.host.c_str(), location.port));
		return;		
	}

	std::string request = S3D::formatStringBuffer(
		"GET %s HTTP/1.0\r\n"
		"Host: %s:%i\r\n"
		"Connection: Close\r\n"
		"User-Agent: OS/version, UPnP/1.0, MiniUPnPc/1.4\r\n"
		"\r\n",
		location.path.c_str(),
		location.host.c_str(), location.port);
	if (SDLNet_TCP_Send(tcpsock, request.c_str(), request.size()) != request.size())
	{
		Logger::log(S3D::formatStringBuffer("igd::Failed to send request to host and port, %s:%i", 
			location.host.c_str(), location.port));
		return;	
	}

	std::string result;
	char buffer[256];
	for (;;)
	{
		int bytesRead = SDLNet_TCP_Recv(tcpsock, buffer, sizeof(buffer));
		if (bytesRead <=0) break;
		result.append(std::string(buffer, bytesRead));
	}

	if (result.find("200 OK") != -1)
	{
		const char *data = strstr(result.c_str(), "\r\n\r\n");
		if (data) 
		{
			data+=4;
			location.serviceData = data;
		} 
		else
		{
			Logger::log(S3D::formatStringBuffer("igd::Failed to get data from host and port, %s:%i, %s", 
				location.host.c_str(), location.port, result.c_str()));
		}
	}
	else
	{
		Logger::log(S3D::formatStringBuffer("igd::Failed to get response from host and port, %s:%i, %s", 
			location.host.c_str(), location.port, result.c_str()));
	}

	SDLNet_TCP_Close(tcpsock);
}
