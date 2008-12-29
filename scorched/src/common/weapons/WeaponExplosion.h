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

#if !defined(AFX_WEAPONEXPLOSION_H__70119A64_2064_4066_8EE5_FD6A3E24D5FC__INCLUDED_)
#define AFX_WEAPONPEXPLOSION_H__70119A64_2064_4066_8EE5_FD6A3E24D5FC__INCLUDED_

#include <weapons/Weapon.h>
#include <actions/ExplosionParams.h>

class WeaponExplosion : public Weapon
{
public:
	WeaponExplosion();
	virtual ~WeaponExplosion();

	virtual bool parseXML(AccessoryCreateContext &context,
		XMLNode *accessoryNode);

	// Inherited from Weapon
	virtual void fireWeapon(ScorchedContext &context,
		WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity);

	REGISTER_ACCESSORY_HEADER(WeaponExplosion, AccessoryPart::AccessoryWeapon);

protected:
	ExplosionParams params_;
	NumberParser sizeExp_;
	NumberParser shakeExp_;
	NumberParser minLifeExp_, maxLifeExp_;
	NumberParser createMushroomAmountExp_;
	NumberParser hurtAmountExp_;

};

#endif // !defined(AFX_WEAPONPEXPLOSION_H__70119A64_2064_4066_8EE5_FD6A3E24D5FC__INCLUDED_)

