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

#include <weapons/AccessoryPart.h>
#include <weapons/AccessoryCreateContext.h>
#include <stdlib.h>

unsigned int AccessoryPart::nextAccessoryPartId_ = 100000;

AccessoryPart::AccessoryPart(const char *typeName, const char *description) :
	XMLEntryContainer(typeName, description),
	accessoryPartId_(++nextAccessoryPartId_),
	parent_(0)
{
}

AccessoryPart::~AccessoryPart()
{

}

std::map<std::string, AccessoryPart *> *AccessoryMetaRegistration::accessoryMap = 0;
std::map<std::string, AccessoryPart *> *AccessoryMetaRegistration::weaponMap = 0;

void AccessoryMetaRegistration::addMap(const char *name, AccessoryPart *accessory)
{
	if (!accessoryMap) accessoryMap = new std::map<std::string, AccessoryPart *>;
	if (!weaponMap) weaponMap = new std::map<std::string, AccessoryPart *>;

	std::map<std::string, AccessoryPart *>::iterator itor = 
		accessoryMap->find(name);
	DIALOG_ASSERT(itor == accessoryMap->end());

	(*accessoryMap)[name] = accessory;
	if (accessory->getType() == AccessoryPart::AccessoryWeapon)
	{
		(*weaponMap)[name] = accessory;
	}
}

AccessoryPart *AccessoryMetaRegistration::getNewAccessory(const char *name)
{
	std::map<std::string, AccessoryPart *>::iterator itor = 
		accessoryMap->find(name);
	if (itor == accessoryMap->end()) return 0;

	AccessoryPart *newAccessory = (*itor).second->getAccessoryCopy();
	return newAccessory;
}

XMLEntryAccessoryPartChoice::XMLEntryAccessoryPartChoice() :
	XMLEntryTypeChoice<AccessoryPart>("AccessoryPartChoice", "The choice of an accessory part")
{
}

XMLEntryAccessoryPartChoice::~XMLEntryAccessoryPartChoice()
{
}

AccessoryPart *XMLEntryAccessoryPartChoice::createXMLEntry(const std::string &type, void *xmlData)
{
	AccessoryPart *result = AccessoryMetaRegistration::getNewAccessory(type.c_str());
	AccessoryCreateContext *context = (AccessoryCreateContext *) xmlData;
	if (context && result)
	{
		result->setParent(context->getAccessory());
	}
	return result;
}

void XMLEntryAccessoryPartChoice::getAllTypes(std::set<std::string> &allTypes)
{
	std::map<std::string, AccessoryPart *>::iterator 
		itor = AccessoryMetaRegistration::accessoryMap->begin(),
		end = AccessoryMetaRegistration::accessoryMap->end();
	for (;itor!=end;++itor)
	{
		allTypes.insert(itor->first);
	}
}
