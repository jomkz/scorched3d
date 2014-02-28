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
#include <common/ModelID.h>
#include <engine/ObjectGroupEntryDefinition.h>

class WeaponProjectile : public Weapon
{
public:
	WeaponProjectile();
	virtual ~WeaponProjectile();

	virtual bool parseXML(AccessoryCreateContext &context,
		XMLNode *accessoryNode);

	Weapon *getCollisionAction() { return collisionAction_; }

	// Inherited from Weapon
	virtual void fireWeapon(ScorchedContext &context,
		WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity);

	REGISTER_ACCESSORY_HEADER(WeaponProjectile, AccessoryPart::AccessoryWeapon);

	bool getUnder() { return under_; }
	bool getShowShotPath() { return showShotPath_; }
	bool getShowEndPoint() { return showEndPoint_; }
	bool getApexCollision() { return apexCollision_; }
	bool getWaterCollision() { return waterCollision_; }
	bool getLandscapeCollision() { return landscapeCollision_; }
	bool getShieldCollision() { return shieldCollision_; }
	bool getWallCollision() { return wallCollision_; }
	bool getApexNoDud() { return apexNoDud_; }
	bool getTimedDud() { return timedDud_; }
	fixed getWindFactor(ScorchedContext &context);
	fixed getGravityFactor(ScorchedContext &context);
	fixed getShieldHurtFactor(ScorchedContext &context);
	fixed getTimedCollision(ScorchedContext &context) { return timedCollision_.getValue(context); }
	fixed getHeightCollision(ScorchedContext &context) { return heightCollision_.getValue(context); }
	fixed getSpinSpeed(ScorchedContext &context) { return spinSpeed_.getValue(context); }
	fixed getStepSize() { return stepSize_; }
	fixed getThrustAmount(ScorchedContext &context) { return thrustAmount_.getValue(context); }
	fixed getThrustTime(ScorchedContext &context) { return thrustTime_.getValue(context); }
	fixed getDrag(ScorchedContext &context) { return drag_.getValue(context); }
	fixed getWobbleSpin(ScorchedContext &context) { return wobbleSpin_.getValue(context); }
	fixed getWobbleAmount(ScorchedContext &context) { return wobbleAmount_.getValue(context); }
	const char *getEngineSound() { return engineSound_.c_str(); }
	bool getNoCameraTrack() { return noCameraTrack_; }
	fixed getScale(ScorchedContext &context) { return scale_.getValue(context); }
	ModelID &getModelID() { return modelId_; }
	ObjectGroupEntryDefinition &getLocalGroups() { return localGroups_; }
	ObjectGroupEntryDefinition &getGlobalGroups() { return globalGroups_; }

protected:
	bool under_;
	bool showShotPath_;
	bool showEndPoint_;
	bool apexCollision_;
	bool waterCollision_, wallCollision_;
	bool landscapeCollision_;
	bool shieldCollision_;
	bool apexNoDud_, timedDud_;
	bool noCameraTrack_;
	NumberParser spinSpeed_;
	FixedVector spinAxis_;

	ObjectGroupEntryDefinition localGroups_, globalGroups_;
	NumberParser thrustTime_, thrustAmount_;
	NumberParser timedCollision_;
	NumberParser shieldHurtFactor_;
	NumberParser scale_;
	NumberParser windFactor_;
	NumberParser gravityFactor_;
	NumberParser drag_;
	NumberParser heightCollision_;
	NumberParser wobbleSpin_;
	NumberParser wobbleAmount_;
	fixed stepSize_;
	std::string engineSound_;
	Weapon *collisionAction_;
	ModelID modelId_;
};

#endif // !defined(AFX_WEAPONPROJECTILE_H__70119A64_2064_4066_8EE5_FD6A3E24D5FC__INCLUDED_)
