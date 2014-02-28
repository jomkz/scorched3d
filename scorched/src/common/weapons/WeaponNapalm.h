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

#if !defined(__INCLUDE_WeaponNapalmh_INCLUDE__)
#define __INCLUDE_WeaponNapalmh_INCLUDE__

#include <weapons/Weapon.h>
#include <XML/XMLEntryComplexTypes.h>

class WeaponNapalm : public Weapon
{
public:
	WeaponNapalm();
	virtual ~WeaponNapalm();

	fixed getNapalmTime(ScorchedContext &context) { return napalmTime_.getValue(context); }
	fixed getNapalmHeight(ScorchedContext &context) { return napalmHeight_.getValue(context); }
	fixed getStepTime(ScorchedContext &context) { return  stepTime_.getValue(context); }
	fixed getHurtStepTime(ScorchedContext &context) { return hurtStepTime_.getValue(context); }
	fixed getHurtPerSecond(ScorchedContext &context) { return hurtPerSecond_.getValue(context); }
	fixed getLandscapeErosion(ScorchedContext &context) { return landscapeErosion_.getValue(context); }
	int getNumberOfParticles() { return numberOfParticles_.getValue(); }
	int getEffectRadius() { return effectRadius_.getValue(); }
	bool getNoObjectDamage() { return noObjectDamage_.getValue();  }
	bool getAllowUnderWater() { return allowUnderWater_.getValue(); }
	bool getSingleFlow() { return singleFlow_.getValue(); }
	bool getNoCameraTrack() { return noCameraTrack_.getValue(); }

	// Inherited from Weapon
	virtual void fireWeapon(ScorchedContext &context,
		WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity);

	REGISTER_ACCESSORY_HEADER(WeaponNapalm, AccessoryPart::AccessoryWeapon);

protected:
	XMLEntryInt numberStreams_;			// The number of napalm paths
	XMLEntryNumberParser napalmTime_;   // The time to generate napalm
	XMLEntryNumberParser napalmHeight_; // The height of a napalm point
	XMLEntryNumberParser stepTime_;     // Add/rm napalm every StepTime secs
	XMLEntryNumberParser hurtStepTime_; // Calculate damage every HurtStepTime secs
	XMLEntryNumberParser hurtPerSecond_;// Damage per second
	XMLEntryNumberParser landscapeErosion_; // How much height will be removed for the erosion
	XMLEntryInt numberOfParticles_; 
	XMLEntryInt effectRadius_;   // How close do tanks take damage
	XMLEntryBool noObjectDamage_;// Turns off burning damage to landscape objects
	XMLEntryBool allowUnderWater_;// Turns on/off napalm working under water
	XMLEntryBool singleFlow_;    // Use a single flow of napalm or cover the whole downward area
	XMLEntryBool noCameraTrack_;
};

#endif
