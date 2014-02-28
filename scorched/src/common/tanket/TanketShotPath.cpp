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

#include <tanket/TanketShotPath.h>
#include <tank/TankLib.h>
#include <target/TargetLife.h>
#include <common/OptionsScorched.h>

TanketShotPath::TanketShotPath(ScorchedContext &context, Tanket *tanket, bool keepPath) :
	context_(context), tanket_(tanket), collision_(false), keepPath_(keepPath)
{
}

TanketShotPath::~TanketShotPath()
{
}

void TanketShotPath::collision(PhysicsParticleObject &position, 
	ScorchedCollisionId collisionId)
{
	resultPosition_ = position.getPosition();
	collision_ = true;
}

void TanketShotPath::wallCollision(PhysicsParticleObject &position,
	ScorchedCollisionId collisionId)
{
	// For now the AIs cannot cope with bouncy and wrap walls.
	// So they will count the shot end where they hit the wall
	resultPosition_ = position.getPosition();
	collision_ = true;
}

bool TanketShotPath::makeShot(fixed rotation, fixed elevation, fixed power, FixedVector &result)
{
	bool actionSyncCheck = context_.getOptionsGame().
		getMainOptions().getActionSyncCheck();
	context_.getOptionsGame().getMainOptions().
		getActionSyncCheckEntry().setValue(false);

	FixedVector shotVelocity;
	TankLib::getVelocityVector(shotVelocity, rotation, elevation);
	shotVelocity *=	(power + 1);
	FixedVector tankTurretPosition;
	tanket_->getLife().getTankTurretPosition(tankTurretPosition);
	FixedVector shotPosition;
	TankLib::getTankGunPosition(shotPosition, tankTurretPosition, rotation, elevation);

	PhysicsParticleObject particleObject;
	PhysicsParticleInfo info(ParticleTypeShot, tanket_->getPlayerId(), 0);
	particleObject.setPhysics(info, context_, shotPosition, shotVelocity);
	particleObject.setHandler(this);

	positions_.clear();
	collision_ = false;
	for (int i=0; i<10000 && !collision_; i++)
	{
		if (keepPath_)
		{
			positions_.push_back(particleObject.getPosition());
		}
		particleObject.simulate(1);
	}

	result = resultPosition_;
	context_.getOptionsGame().getMainOptions().
		getActionSyncCheckEntry().setValue(actionSyncCheck);

	return collision_;
}
