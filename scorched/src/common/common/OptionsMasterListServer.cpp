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

#include <common/OptionsMasterListServer.h>

OptionsMasterListServer *OptionsMasterListServer::instance_ = 0;

OptionsMasterListServer *OptionsMasterListServer::instance()
{
	if (!instance_) instance_ = new OptionsMasterListServer();
	return instance_;
}

OptionsMasterListServer::OptionsMasterListServer() :
	XMLEntrySimpleGroup("options", 
		"The options that define how the Scorched3D client fetches online data"), 
	masterListServer_("MasterListServer",
		"The master list server for scorched3d", 0, "scorched3d.sourceforge.net"),
	masterListServerURI_("MasterListServerURI",
		"The URI on the master list server for scorched3d", 0, "/scorched"),
	masterListBackupServer_("MasterListBackupServer",
		"The backup master list server for scorched3d", 0, "www.scorched3d.co.uk"),
	masterListBackupServerURI_("MasterListBackupServerURI",
		"The URI on the backup master list server for scorched3d", 0, "/scorched"),
	masterListServerTimeout_("MasterListServerTimeout",
		"The amount of time to allow for list communications", 0, 10),
	chatServer_("ChatServer",
		"The chat server for scorched3d", 0, "www.scorched3d.co.uk"),
	chatServerURI_("ChatServerURI",
		"The URI on the chat server for scorched3d", 0, "/phpBB3/chatmessages.php")
{
	readFromFile(S3D::getSettingsFile("masterlistservers.xml"));
}

OptionsMasterListServer::~OptionsMasterListServer()
{
}

