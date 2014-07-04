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

#if !defined(AFX_WEAPONEXPLOSION_H__70119A64_2064_4066_8EE5_FD6A3E24D5FC__INCLUDED_)
#define AFX_WEAPONPEXPLOSION_H__70119A64_2064_4066_8EE5_FD6A3E24D5FC__INCLUDED_

#include <weapons/Weapon.h>
#include <XML/XMLEntryComplexTypes.h>

class WeaponExplosion : public Weapon
{
public:
	enum DeformType
	{
		DeformNone = 0,
		DeformDown = 1,
		DeformUp = 2,
	};

	WeaponExplosion();
	virtual ~WeaponExplosion();

	DeformType getDeformType() { return (DeformType) deform_.getValue(); }
	fixed getDeformSize(ScorchedContext &context) { return deformSizeExp_.getValue(context); }
	fixed getHurtAmount(ScorchedContext &context) { return hurtAmountExp_.getValue(context); }
	bool getExplodeUnderGround() { return explodeUnderGround_.getValue(); }
	bool getOnlyHurtShield() { return onlyHurtShield_.getValue(); }
	WeaponCameraTrack &getCameraTrack() { return cameraTrack_; }
	XMLEntryParticleIDList &getExplosionParticles() { return explosionParticles_; }

	// Inherited from Weapon
	virtual void fireWeapon(ScorchedContext &context,
		WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity);

	REGISTER_ACCESSORY_HEADER(WeaponExplosion, AccessoryPart::AccessoryWeapon);

protected:
	XMLEntryEnum deform_;
	XMLEntryNumberParser deformSizeExp_;
	XMLEntryNumberParser hurtAmountExp_;
	XMLEntryBool explodeUnderGround_;
	XMLEntryBool onlyHurtShield_;
	XMLEntryParticleIDList explosionParticles_;
	WeaponCameraTrack cameraTrack_;
};

#endif // !defined(AFX_WEAPONPEXPLOSION_H__70119A64_2064_4066_8EE5_FD6A3E24D5FC__INCLUDED_)

