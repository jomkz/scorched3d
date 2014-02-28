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

#if !defined(__INCLUDE_OptionsMasterListServerh_INCLUDE__)
#define __INCLUDE_OptionsMasterListServerh_INCLUDE__

#include <XML/XMLEntrySimpleTypes.h>
#include <XML/XMLEntryRoot.h>

class OptionsMasterListServer : public XMLEntryRoot<XMLEntrySimpleContainer>
{
public:
	static OptionsMasterListServer *instance();

	const char *getMasterListServer() { return masterListServer_.getValue().c_str(); }
	const char *getMasterListServerURI() { return masterListServerURI_.getValue().c_str(); }
	const char *getMasterListBackupServer() { return masterListBackupServer_.getValue().c_str(); }
	const char *getMasterListBackupServerURI() { return masterListBackupServerURI_.getValue().c_str(); }
	const char *getChatServer() { return chatServer_.getValue().c_str(); }
	const char *getChatServerURI() { return chatServerURI_.getValue().c_str(); }
	int getMasterListServerTimeout() { return masterListServerTimeout_.getValue(); }

protected:
	static OptionsMasterListServer *instance_;

	XMLEntryString masterListServer_;
	XMLEntryString masterListServerURI_;
	XMLEntryString masterListBackupServer_;
	XMLEntryString masterListBackupServerURI_;
	XMLEntryString chatServer_;
	XMLEntryString chatServerURI_;
	XMLEntryInt masterListServerTimeout_;

private:
	OptionsMasterListServer();
	virtual ~OptionsMasterListServer();
};

#endif // __INCLUDE_OptionsMasterListServerh_INCLUDE__
