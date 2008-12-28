////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
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

#if !defined(AFX_WEAPON_H__65439E20_84A6_406A_8FD0_045A3E7555D3__INCLUDED_)
#define AFX_WEAPON_H__65439E20_84A6_406A_8FD0_045A3E7555D3__INCLUDED_

#include <weapons/AccessoryPart.h>
#include <net/NetBuffer.h>
#include <common/FixedVector.h>
#include <engine/ScorchedContext.h>
#include <common/NumberParser.h>

class WeaponFireContext
{
public:
	WeaponFireContext(unsigned int playerId, unsigned int data);
	WeaponFireContext(WeaponFireContext &other);
	virtual ~WeaponFireContext();

	unsigned int getPlayerId() { return playerId_; }
	void setPlayerId(unsigned int playerId) { playerId_ = playerId; }
	unsigned int getData() { return data_; }
	int getIncLabelCount(unsigned int label);

protected:
	unsigned int playerId_;
	unsigned int data_;
	std::map<unsigned int, int> labelCount_;

private:
	WeaponFireContext &operator=(WeaponFireContext &other);
};

class Action;
class Weapon : public AccessoryPart
{
public:
	enum DataEnum
	{
		eDataDeathAnimation = 1
	};

	Weapon();
	virtual ~Weapon();

	virtual bool parseXML(AccessoryCreateContext &context,
		XMLNode *accessoryNode);
	
	virtual void fireWeapon(ScorchedContext &context,
		WeaponFireContext &weaponContext,
		FixedVector &position, FixedVector &velocity) = 0;

	int getArmsLevel();

protected:
	int armsLevel_;

};

#endif // !defined(AFX_WEAPON_H__65439E20_84A6_406A_8FD0_045A3E7555D3__INCLUDED_)
