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

#if !defined(AFX_ACCESSORYSTORE_H__6A964E56_3784_42FA_A900_2AB4B98F99A8__INCLUDED_)
#define AFX_ACCESSORYSTORE_H__6A964E56_3784_42FA_A900_2AB4B98F99A8__INCLUDED_

#include <list>
#include <vector>
#include <map>
#include <set>
#include <weapons/Weapon.h>
#include <weapons/Accessory.h>
#include <weapons/AccessoryCreateContext.h>
#include <common/ProgressCounter.h>

class XMLNode;
class AccessoryStore
{
public:

	// This is mirrored by accessorySortKeyEnum in client/graph/OptionsDisplay.cpp.

	enum SortKey
	{
		SortNothing = 0,
		SortName = 1,
		SortPrice = 2
	};

	AccessoryStore();
	virtual ~AccessoryStore();

	bool parseFile(ScorchedContext &context, ProgressCounter *counter = 0);
	void clearAccessories();

	Accessory *findByAccessoryId(unsigned int id);
	Accessory *findByPrimaryAccessoryName(const char *name); // **careful there**
	AccessoryPart *findAccessoryPartByAccessoryId(unsigned int id, const char *type);
	AccessoryPart *createAccessoryPart(AccessoryCreateContext &context, 
		Accessory *parent, XMLNode *currentNode);
	AccessoryPart *findByAccessoryPartId(unsigned int id);

	Weapon *getDeathAnimation();
	Weapon *getMuzzelFlash();

	std::list<Accessory *> getAllAccessories(int sortKey=SortNothing);
	std::list<Accessory *> getAllAccessoriesByTabGroup(const char *tabgroup, int sortKey=SortNothing);
	std::map<std::string, XMLNode *> &getParsingNodes() { return parsingNodes_; }
	std::set<std::string> &getTabGroupNames() { return tabGroups_; }

	bool writeWeapon(NetBuffer &buffer, Weapon *weapon);
	bool readWeapon(NetBufferReader &reader, Weapon *&weapon);
	bool writeAccessoryPart(NetBuffer &buffer, AccessoryPart *weapon);
	bool readAccessoryPart(NetBufferReader &reader, AccessoryPart *&part);

	bool writeEconomyToBuffer(NetBuffer &buffer);
	bool readEconomyFromBuffer(NetBufferReader &reader);

	void sortList(std::list<Accessory *> &accList, int sortKey=SortNothing);

protected:
	std::set<std::string> tabGroups_;
	std::list<Accessory *> accessories_;
	std::list<AccessoryPart *> accessoryParts_;
	std::map<std::string, XMLNode *> parsingNodes_;
	Weapon *muzzleFlash_;
	Weapon *deathAnimation_;

};

#endif // !defined(AFX_ACCESSORYSTORE_H__6A964E56_3784_42FA_A900_2AB4B98F99A8__INCLUDED_)
