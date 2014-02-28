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

#include <client/UniqueIdStore.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <client/ClientOptions.h>
#include <net/NetInterface.h>
#include <stdlib.h>

UniqueIdStore::Entry::Entry() :
	XMLEntryContainer("UniqueIdStoreEntry", 
		"Used to store the ids that uniquely identify players on remote servers", 
		false),
	id("The unique id that should be used for this server"),
	published("The published IP address of the server that will be used for validation of authenticity")
{
	addChildXMLEntry("id", &id, "published", &published);
}

UniqueIdStore::Entry::~Entry()
{
}

bool UniqueIdStore::Entry::readXML(XMLNode *node, void *xmlData)
{
	if (!XMLEntryContainer::readXML(node, xmlData)) return false;
	ip = NetInterface::getIpAddressFromName((char *) published.getValue().c_str());
	return true;
}

UniqueIdStore::EntryList::EntryList() :
	XMLEntryList<Entry>("The set of ids that uniquely identify players on remote servers")
{
}

UniqueIdStore::EntryList::~EntryList()
{
}

UniqueIdStore::Entry *UniqueIdStore::EntryList::createXMLEntry(void *)
{
	return new UniqueIdStore::Entry();
}

UniqueIdStore::EntryRoot::EntryRoot() :
	XMLEntryRoot(S3D::eSettingsLocation, "ids.xml", "ids",
		"UniqueIdStore", "Used to store the ids that uniquely identify players on remote servers, "
		"ids are unique for each server to avoid spoofing")
{
	addChildXMLEntry("id", &list_);
}

UniqueIdStore::EntryRoot::~EntryRoot()
{
}

UniqueIdStore::UniqueIdStore()
{
}

UniqueIdStore::~UniqueIdStore()
{
}

bool UniqueIdStore::loadStore()
{
	ids_.list_.clear();
	if (!ids_.loadFile(false)) return false;
	return true;
}

bool UniqueIdStore::saveStore()
{
	return ids_.saveFile();
}

const char *UniqueIdStore::getUniqueId(unsigned int ip)
{
	std::list<Entry *>::iterator itor;
	for (itor = ids_.list_.getChildren().begin();
		itor != ids_.list_.getChildren().end();
		++itor)
	{
		Entry &entry = *(*itor);
		if (entry.ip == ip)
		{
			return entry.id.getValue().c_str();
		}
	}

	static char buffer[128];
	buffer[0] = '\0';
	return buffer;
}

bool UniqueIdStore::saveUniqueId(unsigned int ip, const char *id,
	const char *published)
{
	// No unique id
	if (0 == id[0]) return true;

	// AutoDetect server no unique id saved
	if (0 == strcmp(published, "AutoDetect")) return true;

	// Check the published ip matches the actual server ip
	unsigned int ipAddress = NetInterface::getIpAddressFromName(published);
	if (ipAddress != ip) 
	{
		std::string actualIp = NetInterface::getIpName(ip);
		std::string pubIp = NetInterface::getIpName(ipAddress);
		Logger::log(S3D::formatStringBuffer("Server ip does not match published ip\n%s != %s (%s)",
			actualIp.c_str(), published, pubIp.c_str()));

		if (ClientOptions::instance()->getValidateServerIp())
		{
			return false;
		}
	}

	// If it does, store this id against the published name
	bool found = false;
	std::list<Entry *>::iterator itor;
	for (itor = ids_.list_.getChildren().begin();
		itor != ids_.list_.getChildren().end();
		++itor)
	{
		Entry &entry = *(*itor);
		if (0 == strcmp(entry.published.getValue().c_str(), published))
		{
			if (0 != strcmp(entry.id.getValue().c_str(), id))
			{
				Logger::log( "Warning: Updating to new uniqueid.");

				// Update an old id
				found = true;
				entry.id = id;
				break;
			}
			else
			{
				// Id already here and the same
				return true;
			}
		}
	}

	if (!found)
	{
		// A new id
		Entry *entry = new Entry();
		entry->id.setValue(id);
		entry->ip = ip;
		entry->published.setValue(published);
		ids_.list_.getChildren().push_back(entry);
	}

	// Save this id
	saveStore();
	return true;
}
