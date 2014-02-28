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

#if !defined(__INCLUDE_WeaponRollerh_INCLUDE__)
#define __INCLUDE_WeaponRollerh_INCLUDE__

#include <weapons/Weapon.h>
#include <engine/PhysicsParticleObject.h>
#include <engine/ObjectGroupEntryDefinition.h>
#include <XML/XMLEntryComplexTypes.h>

class ScorchedContext;
class WeaponRoller : public Weapon
{
public:
	WeaponRoller();
	virtual ~WeaponRoller();

	Weapon *getCollisionAction() { return collisionAction_.getValue(); }
	XMLEntryModel &getRollerModel() { return rollerModel_; }
	bool getRoll() { return roll_.getValue(); }
	WeaponCameraTrack &getCameraTrack() { return cameraTrack_; }
	fixed getShieldHurtFactor(ScorchedContext &context) { return shieldHurtFactorExp_.getValue(context); }
	fixed getTime(ScorchedContext &context) { return timeExp_.getValue(context); }
	fixed getTimeout(ScorchedContext &context) { return timeoutExp_.getValue(context); }
	fixed getStepSize() { return stepSize_.getValue(); }

	ObjectGroupEntryDefinition &getLocalGroups() { return localGroups_; }
	ObjectGroupEntryDefinition &getGlobalGroups() { return globalGroups_; }
	PhysicsParticleObjectDefinition &getParticleDefinition() { return particleDefinition_; }

	// Inherited from Weapon
	virtual void fireWeapon(ScorchedContext &context,
		WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity);

	REGISTER_ACCESSORY_HEADER(WeaponRoller, AccessoryPart::AccessoryWeapon);

protected:
	XMLEntryNumberParser numberRollers_;
	XMLEntryWeaponChoice collisionAction_;
	XMLEntryModel rollerModel_;
	XMLEntryNumberParser dampenVelocityExp_;
	XMLEntryNumberParser shieldHurtFactorExp_;
	XMLEntryNumberParser timeExp_, timeoutExp_;
	XMLEntryFixed stepSize_;
	XMLEntryBool roll_;
	XMLEntryBool maintainVelocity_;
	WeaponCameraTrack cameraTrack_;
	PhysicsParticleObjectDefinition particleDefinition_;
	ObjectGroupEntryDefinition localGroups_, globalGroups_;
	
	void addRoller(ScorchedContext &context, 
		WeaponFireContext &weaponContext,
		FixedVector &position, FixedVector &velocity);
};

#endif
