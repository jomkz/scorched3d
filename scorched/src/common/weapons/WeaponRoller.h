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

#if !defined(__INCLUDE_WeaponRollerh_INCLUDE__)
#define __INCLUDE_WeaponRollerh_INCLUDE__

#include <weapons/Weapon.h>
#include <common/ModelID.h>

class ScorchedContext;
class WeaponRoller : public Weapon
{
public:
	WeaponRoller();
	virtual ~WeaponRoller();

	virtual bool parseXML(AccessoryCreateContext &context,
		XMLNode *accessoryNode);

	Weapon *getCollisionAction() { return collisionAction_; }
	ModelID &getRollerModelID() { return rollerModelId_; }
	bool getRoll() { return roll_; }
	bool getStickyShields() { return stickyShields_; }
	fixed getShieldHurtFactor(ScorchedContext &context);
	fixed getTime(ScorchedContext &context);
	fixed getWindFactor(ScorchedContext &context);
	fixed getStepSize() { return stepSize_; }

	// Inherited from Weapon
	virtual void fireWeapon(ScorchedContext &context,
		WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity);

	REGISTER_ACCESSORY_HEADER(WeaponRoller, AccessoryPart::AccessoryWeapon);

protected:
	NumberParser numberRollers_;
	Weapon *collisionAction_;
	ModelID rollerModelId_;
	NumberParser shieldHurtFactorExp_;
	NumberParser windFactorExp_;
	NumberParser timeExp_;
	fixed shieldHurtFactor_;
	fixed windFactor_;
	fixed time_, stepSize_;
	bool roll_;
	bool stickyShields_;

	bool maintainVelocity_;
	NumberParser dampenVelocityExp_;
	fixed dampenVelocity_;;

	void addRoller(ScorchedContext &context, 
		WeaponFireContext &weaponContext,
		FixedVector &position, FixedVector &velocity);

};

#endif
