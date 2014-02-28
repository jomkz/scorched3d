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

#if !defined(__INCLUDE_ShotProjectileh_INCLUDE__)
#define __INCLUDE_ShotProjectileh_INCLUDE__

#include <engine/PhysicsParticle.h>
#include <engine/ScorchedCollisionIds.h>
#include <actions/ParticleGroup.h>
#include <weapons/WeaponProjectile.h>
#include <list>

class TankViewPointProvider;
class ShotProjectile : public PhysicsParticle
{
public:
	ShotProjectile(
		FixedVector &startPosition, FixedVector &velocity,
		WeaponProjectile *weapon, WeaponFireContext &weaponContext,
		fixed spinSpeed, const FixedVector &spinAxis);
	virtual ~ShotProjectile();

	virtual void simulate(fixed frameTime, bool &remove);
	virtual void init();
	virtual void collision(PhysicsParticleObject &position, 
		ScorchedCollisionId collisionId);
	virtual std::string getActionDetails();
	virtual std::string getActionType() { return "ShotProjectile"; }

	unsigned int getPlayerId() { return weaponContext_.getPlayerId(); }
	WeaponProjectile *getWeapon() { return weapon_; }
protected:
	ParticleGroup *groups_;
	FixedVector startPosition_, velocity_;
	WeaponProjectile *weapon_;
	TankViewPointProvider *vPoint_;
	WeaponFireContext weaponContext_;
	unsigned int flareType_;
	bool up_, collided_;
	fixed snapTime_;
	fixed totalTime_, simulateTime_, timeout_;
	fixed thrustTime_;
	fixed thrustAmount_;
	fixed wobbleSpin_, wobbleAmount_;
	fixed drag_, stepSize_;
	fixed timedCollision_, heightCollision_;
	fixed spinSpeed_;
	fixed physicsSpin_;
	FixedVector spinAxis_;

	void doCollision(FixedVector &position);

private:
	ShotProjectile(const ShotProjectile &);
	const ShotProjectile & operator=(const ShotProjectile &);

};

#endif
