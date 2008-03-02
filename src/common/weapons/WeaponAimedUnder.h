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

#if !defined(AFX_WeaponAimedUnder_H__24F2D834_712D_4355_AC74_3571E2F4B14D__INCLUDED_)
#define AFX_WeaponAimedUnder_H__24F2D834_712D_4355_AC74_3571E2F4B14D__INCLUDED_

#include <weapons/Weapon.h>

class WeaponAimedUnder : public Weapon
{
public:
	WeaponAimedUnder();
	virtual ~WeaponAimedUnder();

	virtual bool parseXML(AccessoryCreateContext &context,
		XMLNode *accessoryNode);

	// Inherited from Weapon
	void fireWeapon(ScorchedContext &context, 
		WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity);

	REGISTER_ACCESSORY_HEADER(WeaponAimedUnder, AccessoryPart::AccessoryWeapon);

protected:
	int warHeads_;
	Weapon *aimedWeapon_;
	NumberParser maxAimedDistance_;
	NumberParser percentageMissChance_;
	NumberParser maxInacuracy_;
	bool moveUnderground_;

};

#endif // !defined(AFX_WeaponAimedUnder_H__24F2D834_712D_4355_AC74_3571E2F4B14D__INCLUDED_)
