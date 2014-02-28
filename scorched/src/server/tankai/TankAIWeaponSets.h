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

#if !defined(__INCLUDE_TankAIWeaponSetsh_INCLUDE__)
#define __INCLUDE_TankAIWeaponSetsh_INCLUDE__

#include <tanket/TanketAccessories.h>
#include <XML/XMLEntrySimpleTypes.h>
#include <XML/XMLEntryRoot.h>

class XMLNode;
class TankAIWeaponSets : public XMLEntryRoot<XMLEntryContainer>
{
public:
	TankAIWeaponSets();
	virtual ~TankAIWeaponSets();

	bool parseConfig();

	class WeaponSetAccessories 
	{
	public:
		WeaponSetAccessories(Tanket *tanket);

		int tankMoney;
		unsigned int tankId;
		TanketAccessories tankAccessories;
	};

	class WeaponSetEntry : public XMLEntryContainer
	{
	public:
		WeaponSetEntry();
		virtual ~WeaponSetEntry();

		XMLEntryString accessoryName;
		XMLEntryInt buymin, buymax;
		XMLEntryInt moneymin, moneymax;
		XMLEntryInt prioritybuy, priorityuse;
		XMLEntryStringEnum type;
		Accessory *accessory;

		virtual bool readXML(XMLNode *node, void *xmlData);
		
		bool weaponValid(WeaponSetAccessories &tankAccessories, bool lastRound);
	};

	class WeaponSetEntryList : public XMLEntryList<WeaponSetEntry>
	{
	public:
		WeaponSetEntryList();
		virtual ~WeaponSetEntryList();

		virtual WeaponSetEntry *createXMLEntry(void *xmlData);
	};

	class WeaponSet : public XMLEntryContainer
	{
	public:
		WeaponSet();
		virtual ~WeaponSet();

		XMLEntryString name;
		WeaponSetEntryList weapons;

		void buyWeapons(WeaponSetAccessories &tankAccessories, bool lastRound);
		Accessory *getTankAccessoryByType(Tanket *tanket, const char *type);
	};

	class WeaponSetList : public XMLEntryList<WeaponSet>
	{
	public:
		WeaponSetList();
		virtual ~WeaponSetList();

		virtual WeaponSet *createXMLEntry(void *xmlData);
	};

	WeaponSet *getWeaponSet(const char *name);

protected:
	WeaponSetList weaponSets_;
};

#endif
