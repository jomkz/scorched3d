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
	XMLEntryRoot<XMLEntrySimpleContainer>(S3D::eSettingsLocation, "masterlistservers.xml",
		"OptionsMasterListServer", "OptionsMasterListServer", 
		"The options that define how the Scorched3D client fetches online data", false), 
	masterListServer_("The master list server for scorched3d", 0, "scorched3d.sourceforge.net"),
	masterListServerURI_("The URI on the master list server for scorched3d", 0, "/scorched"),
	masterListBackupServer_("The backup master list server for scorched3d", 0, "www.scorched3d.co.uk"),
	masterListBackupServerURI_("The URI on the backup master list server for scorched3d", 0, "/scorched"),
	masterListServerTimeout_("The amount of time to allow for list communications", 0, 10),
	chatServer_("The chat server for scorched3d", 0, "www.scorched3d.co.uk"),
	chatServerURI_("The URI on the chat server for scorched3d", 0, "/phpBB3/chatmessages.php")
{
	addChildXMLEntry("MasterListServer", &masterListServer_);
	addChildXMLEntry("MasterListServerURI", &masterListServerURI_);
	addChildXMLEntry("MasterListBackupServer", &masterListBackupServer_);
	addChildXMLEntry("MasterListBackupServerURI", &masterListBackupServerURI_);
	addChildXMLEntry("MasterListServerTimeout", &masterListServerTimeout_);
	addChildXMLEntry("ChatServer", &chatServer_);
	addChildXMLEntry("ChatServerURI", &chatServerURI_);

	loadFile(false, 0);
}

OptionsMasterListServer::~OptionsMasterListServer()
{
}

