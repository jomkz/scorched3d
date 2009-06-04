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

#if !defined(AFX_PhysicsParticle_H__53A44445_C8C9_4108_B5E1_744C0AF7A1D5__INCLUDED_)
#define AFX_PhysicsParticle_H__53A44445_C8C9_4108_B5E1_744C0AF7A1D5__INCLUDED_

#include <actions/Action.h>
#include <engine/PhysicsParticleObject.h>

class PhysicsParticle : 
	public Action,
	public PhysicsParticleObjectHandler
{
public:
	PhysicsParticle();
	virtual ~PhysicsParticle();

	virtual void setPhysics(
		PhysicsParticleInfo info,
		FixedVector &position, FixedVector &velocity,
		fixed sphereSize = 0,
		fixed sphereDensity = 0,
		fixed windFactor = 1,
		bool underGroundCollision = false,
		bool rotateOnCollision = false,
		bool wallCollision = true);
	virtual void collision(PhysicsParticleObject &position, 
		ScorchedCollisionId collisionId);

	FixedVector &getCurrentPosition();
	FixedVector &getCurrentVelocity();
	FixedVector4 &getRotationQuat();
	void setCurrentPosition(FixedVector &position);

	void applyForce(FixedVector &force);

	// Inherited from action
	virtual void simulate(fixed timepassed, bool &remove);

protected:
	PhysicsParticleActionObject physicsObject_;
	bool collision_;
	fixed totalActionTime_;

};


class PhysicsParticleReferenced : 
	public PhysicsParticle
{
public:
	PhysicsParticleReferenced();
	virtual ~PhysicsParticleReferenced();

	virtual bool getReferenced() { return true; }
};

#endif // !defined(AFX_PhysicsParticle_H__53A44445_C8C9_4108_B5E1_744C0AF7A1D5__INCLUDED_)
