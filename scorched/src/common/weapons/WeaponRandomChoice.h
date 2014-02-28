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

#if !defined(__INCLUDE_WeaponRandomChoiceh_INCLUDE__)
#define __INCLUDE_WeaponRandomChoiceh_INCLUDE__

#include <weapons/Weapon.h>
#include <XML/XMLEntryComplexTypes.h>

class WeaponRandomChoiceItem : public XMLEntryContainer
{
public:
	WeaponRandomChoiceItem();
	virtual ~WeaponRandomChoiceItem();

	XMLEntryWeaponChoice weapon_;
	XMLEntryInt weight_;
};

class WeaponRandomChoiceItemList : public XMLEntryList<WeaponRandomChoiceItem>
{
public:
	WeaponRandomChoiceItemList();
	virtual ~WeaponRandomChoiceItemList();

	virtual WeaponRandomChoiceItem *createXMLEntry();
};

class WeaponRandomChoice : public Weapon
{
public:
	WeaponRandomChoice();
	virtual ~WeaponRandomChoice();

	// Inherited from Weapon
	virtual bool readXML(XMLNode *node, void *xmlData);
	virtual void fireWeapon(ScorchedContext &context,
		WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity);

	REGISTER_ACCESSORY_HEADER(WeaponRandomChoice, AccessoryPart::AccessoryWeapon);

protected:
	int totalWeight_;
	WeaponRandomChoiceItemList itemList_;
};

#endif // __INCLUDE_WeaponRandomChoiceh_INCLUDE__
