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


#if !defined(__INCLUDE_WeaponNapalmh_INCLUDE__)
#define __INCLUDE_WeaponNapalmh_INCLUDE__

#include <weapons/Weapon.h>
#include <actions/NapalmParams.h>

class WeaponNapalm : public Weapon
{
public:
	WeaponNapalm();
	virtual ~WeaponNapalm();

	virtual bool parseXML(AccessoryCreateContext &context,
		XMLNode *accessoryNode);

	// Inherited from Weapon
	virtual void fireWeapon(ScorchedContext &context,
		WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity);

	REGISTER_ACCESSORY_HEADER(WeaponNapalm, AccessoryPart::AccessoryWeapon);

protected:
	NapalmParams params_;
	std::string napalmSound_;
	int numberStreams_;			// The number of napalm paths
	NumberParser napalmTime_;   // The time to generate napalm
	NumberParser napalmHeight_; // The height of a napalm point
	NumberParser stepTime_;     // Add/rm napalm every StepTime secs
	NumberParser hurtStepTime_; // Calculate damage every HurtStepTime secs
	NumberParser hurtPerSecond_;// Damage per second

	void addNapalm(ScorchedContext &context, WeaponFireContext &weaponContext,
		int x, int y);
};

#endif
