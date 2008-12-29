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

#if !defined(__INCLUDE_OptionsMasterListServerh_INCLUDE__)
#define __INCLUDE_OptionsMasterListServerh_INCLUDE__

#include <common/OptionEntry.h>

class OptionsMasterListServer
{
public:
	static OptionsMasterListServer *instance();

	const char * getMasterListServer() { return masterListServer_; }
	const char * getMasterListServerURI() { return masterListServerURI_; }
	const char * getMasterListBackupServer() { return masterListBackupServer_; }
	const char * getMasterListBackupServerURI() { return masterListBackupServerURI_; }
	int getMasterListServerTimeout() { return masterListServerTimeout_; }

protected:
	static OptionsMasterListServer *instance_;

	std::list<OptionEntry *> options_;

	OptionEntryString masterListServer_;
	OptionEntryString masterListServerURI_;
	OptionEntryString masterListBackupServer_;
	OptionEntryString masterListBackupServerURI_;
	OptionEntryInt masterListServerTimeout_;

	bool readOptionsFromFile();
	bool writeOptionsToFile();

private:
	OptionsMasterListServer();
	virtual ~OptionsMasterListServer();
};

#endif // __INCLUDE_OptionsMasterListServerh_INCLUDE__
