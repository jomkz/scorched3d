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

#if !defined(__INCLUDE_WeaponLightningh_INCLUDE__)
#define __INCLUDE_WeaponLightningh_INCLUDE__

#include <weapons/Weapon.h>
#include <XML/XMLEntryComplexTypes.h>

class WeaponLightning : public Weapon
{
public:
	WeaponLightning();
	virtual ~WeaponLightning();

	fixed getConeLength() { return coneLength_; }
	fixed getSegLength() { return segLength_; }
	fixed getSegVar() { return segVar_; }
	fixed getSize() { return size_; }
	fixed getSizeVar() { return sizeVar_; }
	fixed getMinSize() { return minSize_; }
	fixed getSplitProb() { return splitProb_; }
	fixed getSplitVar() { return splitVar_; }
	fixed getDeathProb() { return deathProb_; }
	fixed getDerivAngle() { return derivAngle_; }
	fixed getAngleVar() { return angleVar_; }
	fixed getTotalTime() { return totalTime_; }
	fixed getSegHurt() { return segHurt_; }
	fixed getSegHurtRadius() { return segHurtRadius_; }

	// Inherited from Weapon
	void fireWeapon(ScorchedContext &context,
		WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity);

	REGISTER_ACCESSORY_HEADER(WeaponLightning, AccessoryPart::AccessoryWeapon);

protected:
	fixed coneLength_;
	fixed segLength_;
	fixed segVar_;
	fixed size_;
	fixed sizeVar_;
	fixed minSize_;
	fixed splitProb_;
	fixed splitVar_;
	fixed deathProb_;
	fixed derivAngle_;
	fixed angleVar_;
	fixed totalTime_;
	fixed segHurt_;
	fixed segHurtRadius_;
	XMLEntryNumberParser coneLengthExp_;
	XMLEntryNumberParser segLengthExp_;
	XMLEntryNumberParser segVarExp_;
	XMLEntryNumberParser sizeExp_;
	XMLEntryNumberParser sizeVarExp_;
	XMLEntryNumberParser minSizeExp_;
	XMLEntryNumberParser splitProbExp_;
	XMLEntryNumberParser splitVarExp_;
	XMLEntryNumberParser deathProbExp_;
	XMLEntryNumberParser derivAngleExp_;
	XMLEntryNumberParser angleVarExp_;
	XMLEntryNumberParser totalTimeExp_;
	XMLEntryNumberParser segHurtExp_;
	XMLEntryNumberParser segHurtRadiusExp_;

};

#endif // __INCLUDE_WeaponLightningh_INCLUDE__
