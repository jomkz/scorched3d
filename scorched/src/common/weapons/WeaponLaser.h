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

#if !defined(__INCLUDE_WeaponLaserh_INCLUDE__)
#define __INCLUDE_WeaponLaserh_INCLUDE__

#include <weapons/Weapon.h>
#include <XML/XMLEntryComplexTypes.h>

class WeaponLaser  : public Weapon
{
public:
	WeaponLaser();
	virtual ~WeaponLaser();

	fixed getMinimumHurt(ScorchedContext &context) { return minimumHurt_.getValue(context); }
	fixed getMaximumHurt(ScorchedContext &context) { return maximumHurt_.getValue(context); }
	fixed getMinimumDistance(ScorchedContext &context) { return minimumDistance_.getValue(context); }
	fixed getMaximumDistance(ScorchedContext &context) { return maximumDistance_.getValue(context); }
	fixed getHurtRadius(ScorchedContext &context) { return hurtRadius_.getValue(context); }
	bool getHurtFirer() { return hurtFirer_.getValue(); }

	// Inherited from Weapon
	void fireWeapon(ScorchedContext &context,
		WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity);

	REGISTER_ACCESSORY_HEADER(WeaponLaser, AccessoryPart::AccessoryWeapon);

protected:
	XMLEntryNumberParser minimumHurt_, maximumHurt_;
	XMLEntryNumberParser minimumDistance_, maximumDistance_;
	XMLEntryNumberParser hurtRadius_;
	XMLEntryBool hurtFirer_;
};

#endif // __INCLUDE_WeaponLaserh_INCLUDE__
