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

#include <common/OptionsMasterListServer.h>

OptionsMasterListServer *OptionsMasterListServer::instance_ = 0;

OptionsMasterListServer *OptionsMasterListServer::instance()
{
	if (!instance_) instance_ = new OptionsMasterListServer();
	return instance_;
}

OptionsMasterListServer::OptionsMasterListServer() :
	masterListServer_(options_, "MasterListServer",
		"The master list server for scorched3d", 0, "scorched3d.sourceforge.net"),
	masterListServerURI_(options_, "MasterListServerURI",
		"The URI on the master list server for scorched3d", 0, "/scorched"),
	masterListBackupServer_(options_, "MasterListBackupServer",
		"The backup master list server for scorched3d", 0, "www.scorched3d.co.uk"),
	masterListBackupServerURI_(options_, "MasterListBackupServerURI",
		"The URI on the backup master list server for scorched3d", 0, "/scorched"),
	masterListServerTimeout_(options_, "MasterListServerTimeout",
		"The amount of time to allow for list communications", 0, 10)
{
	readOptionsFromFile();
}

OptionsMasterListServer::~OptionsMasterListServer()
{
}

bool OptionsMasterListServer::writeOptionsToFile()
{
	std::string filePath = S3D::getSettingsFile("masterlistservers.xml");
	if (!OptionEntryHelper::writeToFile(options_, filePath)) return false;
	return true;
}

bool OptionsMasterListServer::readOptionsFromFile()
{
	std::string filePath = S3D::getSettingsFile("masterlistservers.xml");
	if (!OptionEntryHelper::readFromFile(options_, filePath)) return false;
	return true;
}
