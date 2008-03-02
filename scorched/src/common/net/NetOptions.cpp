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

#include <net/NetOptions.h>

NetOptions *NetOptions::instance_ = 0;

NetOptions *NetOptions::instance()
{
	if (!instance_)
	{
		instance_ = new NetOptions;
	}

	return instance_;
}

NetOptions::NetOptions() :
	numberRetries_(options_, "NumberRetries",
		"The number of retries before deeming a connection terminated", 0, 40, 5, 100, 5),
	outstandingPackets_(options_, "OutstandingPackets",
		"The number of packets to send in a block", 0, 10, 1, 100, 10),
	packetSize_(options_, "PacketSize",
		"The size of packets to send", 0, 800, 100, 1500, 100),
	packetLogging_(options_, "PacketLogging",
		"Log in depth packet sends/receives (performance affecting)", 0, false)
{
}

NetOptions::~NetOptions()
{	
}

bool NetOptions::writeOptionsToFile()
{
	std::string path = S3D::getSettingsFile("netoptions.xml");
	if (!OptionEntryHelper::writeToFile(options_, path)) return false;
	return true;
}

bool NetOptions::readOptionsFromFile()
{
	std::string path = S3D::getSettingsFile("netoptions.xml");
	OptionEntryHelper::readFromFile(options_, path);
	return true;
}
