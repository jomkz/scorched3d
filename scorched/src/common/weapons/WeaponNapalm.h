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

class WeaponNapalm : public Weapon
{
public:
	WeaponNapalm();
	virtual ~WeaponNapalm();

	virtual bool parseXML(AccessoryCreateContext &context,
		XMLNode *accessoryNode);

	const fixed getNapalmTime(ScorchedContext &context) { return napalmTime_.getValue(context); }
	const fixed getNaplamHeight(ScorchedContext &context) { return napalmHeight_.getValue(context); }
	const fixed getStepTime(ScorchedContext &context) { return stepTime_.getValue(context); }
	const fixed getHurtStepTime(ScorchedContext &context) { return hurtStepTime_.getValue(context); }
	const fixed getHurtPerSecond(ScorchedContext &context) { return hurtPerSecond_.getValue(context); }
	const fixed getGroundScorchPer(ScorchedContext &context) { return groundScorchPer_.getValue(context); }
	const int getEffectRadius() { return effectRadius_; }
	const int getNumberStreams() { return numberStreams_; }
	const char *getNapalmSound() { return napalmSound_.c_str(); }
	const char *getNapalmTexture() { return napalmTexture_.c_str(); }
	const bool getNoSmoke() { return noSmoke_; }
	const bool getNoObjectDamage() { return noObjectDamage_; }
	const bool getAllowUnderWater() { return allowUnderWater_; }
	const char *getDeformTexture() { return deformTexture_.c_str(); }
	const bool getLuminance() { return luminance_; }

	// Inherited from Weapon
	virtual void fireWeapon(ScorchedContext &context,
		WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity);

	REGISTER_ACCESSORY_HEADER(WeaponNapalm, AccessoryPart::AccessoryWeapon);

protected:
	NumberParser napalmTime_;   // The time to generate napalm
	NumberParser napalmHeight_; // The height of a napalm point
	NumberParser stepTime_;     // Add/rm napalm every StepTime secs
	NumberParser hurtStepTime_; // Calculate damage every HurtStepTime secs
	NumberParser hurtPerSecond_;// Damage per second
	NumberParser groundScorchPer_; // The percentage chance the ground will be scorched
	int numberStreams_;  // The number of napalm paths
	int effectRadius_;   // How close do tanks take damage
	bool noSmoke_;       // Turns off smoke emitted by napalm
	bool noObjectDamage_;// Turns off burning damage to landscape objects
	bool allowUnderWater_;// Turns on/off napalm working under water
	bool luminance_;     // The texutre luminance
	std::string napalmSound_;
	std::string napalmTexture_;
	std::string deformTexture_;

	void addNapalm(ScorchedContext &context, WeaponFireContext &weaponContext,
		int x, int y);
};

#endif
