////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
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

#if !defined(__INCLUDE_WeaponLaserh_INCLUDE__)
#define __INCLUDE_WeaponLaserh_INCLUDE__

#include <weapons/Weapon.h>

class WeaponLaser  : public Weapon
{
public:
	WeaponLaser();
	virtual ~WeaponLaser();

	virtual bool parseXML(AccessoryCreateContext &context,
		XMLNode *accessoryNode);

	fixed getMinimumHurt(ScorchedContext &context) { return minimumHurt_.getValue(context); }
	fixed getMaximumHurt(ScorchedContext &context) { return maximumHurt_.getValue(context); }
	fixed getMinimumDistance(ScorchedContext &context) { return minimumDistance_.getValue(context); }
	fixed getMaximumDistance(ScorchedContext &context) { return maximumDistance_.getValue(context); }
	fixed getHurtRadius(ScorchedContext &context) { return hurtRadius_.getValue(context); }
	fixed getTotalTime(ScorchedContext &context) { return totalTime_.getValue(context); }
	bool getHurtFirer() { return hurtFirer_; }
	Vector &getColor() { return color_; }

	// Inherited from Weapon
	void fireWeapon(ScorchedContext &context,
		WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity);

	REGISTER_ACCESSORY_HEADER(WeaponLaser, AccessoryPart::AccessoryWeapon);

protected:
	Vector color_;
	bool hurtFirer_;
	// Use the following to hold the NumberParser fixed expression
	// convert to values in actions/Laser
	NumberParser minimumHurt_, maximumHurt_;
	NumberParser minimumDistance_, maximumDistance_;
	NumberParser hurtRadius_;
	NumberParser totalTime_;


};

#endif // __INCLUDE_WeaponLaserh_INCLUDE__
