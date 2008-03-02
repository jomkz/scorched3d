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

#if !defined(AFX_WEAPONEXPLOSION_H__70119A64_2064_4066_8EE5_FD6A3E24D5FC__INCLUDED_)
#define AFX_WEAPONPEXPLOSION_H__70119A64_2064_4066_8EE5_FD6A3E24D5FC__INCLUDED_

#include <weapons/Weapon.h>
#include <actions/Explosion.h>

class WeaponExplosion : public Weapon
{
public:
	WeaponExplosion();
	virtual ~WeaponExplosion();

	virtual bool parseXML(AccessoryCreateContext &context,
		XMLNode *accessoryNode);

	const char *getExplosionTexture();
	const char *getExplosionSound();
	virtual FixedVector &getExplosionColor();
	fixed getSize() { return size_; }
	fixed getMaxLife() { return maxLife_; }
	fixed getMinLife() { return minLife_; }
	Explosion::DeformType getDeformType() { return deform_; }
	fixed getHurtAmount() { return hurtAmount_; }
	bool getCreateDebris() { return createDebris_; }
	fixed getCreateMushroomAmount() { return createMushroomAmount_; }
	bool getCreateSplash() { return createSplash_; }
	bool getWindAffected() { return windAffected_; }
	bool getOnlyHurtShield() { return onlyHurtShield_; }
	bool getLuminance() { return luminance_; }
	bool getAnimate() { return animate_; }
	bool getExplodeUnderGround() { return explodeUnderGround_; }
	fixed getShake() { return shake_; }
	const char *getDeformTexture() { return deformTexture_.c_str(); }

	// Inherited from Weapon
	virtual void fireWeapon(ScorchedContext &context,
		WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity);

	REGISTER_ACCESSORY_HEADER(WeaponExplosion, AccessoryPart::AccessoryWeapon);

protected:
	fixed size_;
	fixed shake_;
	fixed minLife_, maxLife_;
	bool luminance_;
	bool windAffected_;
	bool multiColored_;
	bool createDebris_;
	fixed createMushroomAmount_;
	bool createSplash_;
	bool explodeUnderGround_;
	bool onlyHurtShield_;
	bool animate_;
	fixed hurtAmount_;
	std::string deformTexture_;
	std::string explosionTexture_;
	std::string explosionSound_;
	Explosion::DeformType deform_;

	NumberParser sizeExp_;
	NumberParser shakeExp_;
	NumberParser minLifeExp_, maxLifeExp_;
	NumberParser createMushroomAmountExp_;
	NumberParser hurtAmountExp_;

};

#endif // !defined(AFX_WEAPONPEXPLOSION_H__70119A64_2064_4066_8EE5_FD6A3E24D5FC__INCLUDED_)

