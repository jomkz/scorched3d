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

#if !defined(__INCLUDE_ShotProjectileh_INCLUDE__)
#define __INCLUDE_ShotProjectileh_INCLUDE__

#include <engine/PhysicsParticle.h>
#include <engine/ScorchedCollisionIds.h>
#include <engine/ViewPoints.h>
#ifndef	S3D_SERVER
	#include <tankgraph/RenderTracer.h>
#endif
#include <weapons/WeaponProjectile.h>
#include <list>

class ShotProjectile : 
	public PhysicsParticleReferenced
{
public:
	ShotProjectile(
		FixedVector &startPosition, FixedVector &velocity,
		WeaponProjectile *weapon, WeaponFireContext &weaponContext,
		unsigned int flareType, fixed spinSpeed);
	virtual ~ShotProjectile();

	virtual void simulate(fixed frameTime, bool &remove);
	virtual void init();
	virtual void collision(PhysicsParticleObject &position, 
		ScorchedCollisionId collisionId);
	virtual std::string getActionDetails();

	unsigned int getPlayerId() { return weaponContext_.getPlayerId(); }
	WeaponProjectile *getWeapon() { return weapon_; }
#ifndef S3D_SERVER
	std::list<RenderTracer::TracerLinePoint> &getPositions() { return positions_; }
#endif

protected:
	FixedVector startPosition_, velocity_;
	WeaponProjectile *weapon_;
	ViewPoints::ViewPoint *vPoint_;
	WeaponFireContext weaponContext_;
	unsigned int flareType_;
	bool up_;
	fixed snapTime_;
	fixed totalTime_;
	// 	things like thrusttime etc should have their 
	// 	values set ONCE per projectile
	fixed thrustTime_;
	fixed thrustAmount_;
	fixed drag_;
	fixed timedCollision_;
	fixed spinSpeed_;
#ifndef S3D_SERVER
	std::list<RenderTracer::TracerLinePoint> positions_;
#endif

	void doCollision(FixedVector &position);

private:
	ShotProjectile(const ShotProjectile &);
	const ShotProjectile & operator=(const ShotProjectile &);

};

#endif
