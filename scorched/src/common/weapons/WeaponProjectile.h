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

#if !defined(AFX_WEAPONPROJECTILE_H__70119A64_2064_4066_8EE5_FD6A3E24D5FC__INCLUDED_)
#define AFX_WEAPONPROJECTILE_H__70119A64_2064_4066_8EE5_FD6A3E24D5FC__INCLUDED_

#include <weapons/Weapon.h>
#include <engine/ObjectGroupEntryDefinition.h>
#include <engine/PhysicsParticleObject.h>
#include <XML/XMLEntryComplexTypes.h>

class WeaponProjectile : public Weapon
{
public:
	WeaponProjectile();
	virtual ~WeaponProjectile();

	Weapon *getCollisionAction() { return collisionAction_.getValue(); }

	// Inherited from Weapon
	virtual void fireWeapon(ScorchedContext &context,
		WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity);

	REGISTER_ACCESSORY_HEADER(WeaponProjectile, AccessoryPart::AccessoryWeapon);

	bool getShowShotPath() { return showShotPath_.getValue(); }
	bool getShowEndPoint() { return showEndPoint_.getValue(); }
	bool getApexCollision() { return apexCollision_.getValue(); }
	bool getApexNoDud() { return apexNoDud_.getValue(); }
	bool getTimedDud() { return timedDud_.getValue(); }
	fixed getShieldHurtFactor(ScorchedContext &context) { return shieldHurtFactor_.getValue(context); }
	fixed getTimedCollision(ScorchedContext &context) { return timedCollision_.getValue(context); }
	fixed getHeightCollision(ScorchedContext &context) { return heightCollision_.getValue(context); }
	fixed getSpinSpeed(ScorchedContext &context) { return spinSpeed_.getValue(context); }
	fixed getStepSize() { return stepSize_.getValue(); }
	fixed getThrustAmount(ScorchedContext &context) { return thrustAmount_.getValue(context); }
	fixed getThrustTime(ScorchedContext &context) { return thrustTime_.getValue(context); }
	fixed getDrag(ScorchedContext &context) { return drag_.getValue(context); }
	fixed getWobbleSpin(ScorchedContext &context) { return wobbleSpin_.getValue(context); }
	fixed getWobbleAmount(ScorchedContext &context) { return wobbleAmount_.getValue(context); }
	bool getNoCameraTrack() { return noCameraTrack_.getValue(); }
	XMLEntryModelID &getModelID() { return modelId_; }
	ObjectGroupEntryDefinition &getLocalGroups() { return localGroups_; }
	ObjectGroupEntryDefinition &getGlobalGroups() { return globalGroups_; }
	PhysicsParticleObjectDefinition &getParticleDefinition() { return particleDefinition_; }

protected:
	XMLEntryBool showShotPath_;
	XMLEntryBool showEndPoint_;
	XMLEntryBool apexCollision_;
	XMLEntryBool apexNoDud_, timedDud_;
	XMLEntryBool noCameraTrack_;
	XMLEntryNumberParser spinSpeed_;
	XMLEntryFixedVector spinAxis_;

	PhysicsParticleObjectDefinition particleDefinition_;
	ObjectGroupEntryDefinition localGroups_, globalGroups_;
	XMLEntryNumberParser thrustTime_, thrustAmount_;
	XMLEntryNumberParser timedCollision_;
	XMLEntryNumberParser shieldHurtFactor_;
	XMLEntryNumberParser drag_;
	XMLEntryNumberParser heightCollision_;
	XMLEntryNumberParser wobbleSpin_;
	XMLEntryNumberParser wobbleAmount_;
	XMLEntryFixed stepSize_;
	XMLEntryWeaponChoice collisionAction_;
	XMLEntryModelID modelId_;
};

#endif // !defined(AFX_WEAPONPROJECTILE_H__70119A64_2064_4066_8EE5_FD6A3E24D5FC__INCLUDED_)
