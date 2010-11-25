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

#include <tankai/TankAISniperGuesser.h>
#include <tank/Tank.h>
#include <tank/TankLib.h>
#include <tank/TankPosition.h>
#include <server/ScorchedServer.h>
#include <common/Logger.h>
#include <common/RandomGenerator.h>

TankAISniperGuesser::TankAISniperGuesser()
{
}

TankAISniperGuesser::~TankAISniperGuesser()
{
}

bool TankAISniperGuesser::guess(Tank *tank, Vector &target, 
	float distForSniper, bool checkIntersection, float offset)
{
	fixed angleXYDegs, angleYZDegs, power;
	FileRandomGenerator generator;
	generator.seed(rand());

	FixedVector shotPosition = tank->getPosition().getTankPosition();
	if (TankLib::getSniperShotTowardsPosition(
		ScorchedServer::instance()->getContext(),
		shotPosition, 
		FixedVector::fromVector(target), 
		fixed::fromFloat(distForSniper), 
		angleXYDegs, angleYZDegs, power,
		checkIntersection))
	{
		fixed xyOffset = generator.getRandFixed("TankAISniperGuesser") * fixed::fromFloat(offset);
		fixed yzOffset = fixed::fromFloat(offset) - xyOffset;

		tank->getPosition().rotateGunXY(angleXYDegs + xyOffset, false);
		tank->getPosition().rotateGunYZ(angleYZDegs + yzOffset, false);
		tank->getPosition().changePower(power, false);

		return true;
	}
	return false;
}
