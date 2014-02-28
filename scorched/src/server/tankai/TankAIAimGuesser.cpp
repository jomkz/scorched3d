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

#include <tankai/TankAIAimGuesser.h>
#include <tanket/Tanket.h>
#include <tanket/TanketShotInfo.h>
#include <tanket/TanketShotPath.h>
#include <tank/TankLib.h>
#include <target/TargetLife.h>
#include <common/OptionsScorched.h>
#include <common/Logger.h>
#include <common/RandomGenerator.h>
#include <math.h>

TankAIAimGuesser::TankAIAimGuesser(ScorchedContext &context) :
	context_(context)
{
}

TankAIAimGuesser::~TankAIAimGuesser()
{
}

bool TankAIAimGuesser::guess(Tanket *tanket, Vector &target, 
	float angleYZDegs, float distance, 
	Vector &actualPosition,
	TankAIAimResult &result)
{
	result.elevation_ = fixed::fromFloat(angleYZDegs);

	// Make an initial randomish shot up
	initialShot(tanket, target, result);

	TanketShotPath shotPath(context_, tanket);
	for (int i=0;;i++)
	{
		// Find out where this may land
		FixedVector currentGuess;
		if (!shotPath.makeShot(result.rotation_, result.elevation_, result.power_, currentGuess))
		{
			// No collision
			// A bad thing
			return false; 
		}

		// Its landed
		actualPosition = currentGuess.asVector();
		Vector direction = currentGuess.asVector() - target;
		float actualDistance = 
			float(sqrt(direction[0]*direction[0] + direction[1]*direction[1]));
		if (actualDistance < distance)
		{
			// Its close
			// A good thing
			return true;
		}

		if (i > 25)
		{
			// No soultion
			// A bad thing
			return false;
		}

		// Not close
		refineShot(tanket, currentGuess.asVector(), target, result);
	}

	// Never gets here
	return false;
}

void TankAIAimGuesser::initialShot(Tanket *tanket, Vector &target, TankAIAimResult &result)
{
	fixed angleXYDegs;
	fixed angleYZDegs;
	fixed power;

	// Makes a randow powered shot towards the target
	FileRandomGenerator random;
	random.seed(rand());
	TankLib::getShotTowardsPosition(
		context_,
		random,
		tanket->getLife().getTargetPosition(), 
		FixedVector::fromVector(target), 
		angleXYDegs, angleYZDegs, power);

	// Set the parameters
	// Sets the angle of the gun and the power
	result.rotation_ = angleXYDegs;
	result.power_ = power;
}

void TankAIAimGuesser::refineShot(Tanket *tanket,
	Vector &currentPos, Vector &wantedPos,
	TankAIAimResult &result)
{
	// Get the used velocity
	FixedVector shotVelocity;
	TankLib::getVelocityVector(shotVelocity, result.rotation_, result.elevation_);
	shotVelocity *=	(result.power_ + 1);

	// Figure out how much the last shot missed by
	Vector missedBy = wantedPos - currentPos;
	
	// Adjust velocity to take this into account
	shotVelocity[0] += fixed::fromFloat(missedBy[0]) * fixed(true, rand() % 200 + 200);
	shotVelocity[1] += fixed::fromFloat(missedBy[1]) * fixed(true, rand() % 200 + 200);

	// Figure out a new XY angle based on these
	fixed angleXYRads = atan2x(shotVelocity[1], shotVelocity[0]);
	fixed angleXYDegs = (angleXYRads / fixed::XPI) * 180 - 90;
	result.rotation_ = angleXYDegs;

	// And the new best power
	float dist = (currentPos - tanket->getLife().getTargetPosition().asVector()).Magnitude2d();
	float wanteddist = (wantedPos - tanket->getLife().getTargetPosition().asVector()).Magnitude2d();
	float currentPower = (float) (log(dist) / log(2.0));
	float wantedPower = (float) (log(wanteddist) / log(2.0));

	fixed power = result.power_ * fixed::fromFloat(wantedPower) / 
		fixed::fromFloat(currentPower);
	result.power_ = power;
}
