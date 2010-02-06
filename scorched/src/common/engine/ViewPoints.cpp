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

#include <engine/ViewPoints.h>
#include <engine/ScorchedContext.h>
#include <common/Defines.h>
#include <common/OptionsScorched.h>
#include <tank/TankContainer.h>

ViewPoints::ViewPoints() : context_(0), totalTime_(0)
{
}

ViewPoints::~ViewPoints()
{
}

void ViewPoints::getValues(FixedVector &lookAt, 
						   FixedVector &lookFrom)
{
	lookAt = lookAt_;
	lookFrom = lookFrom_;
}

void ViewPoints::setValues(FixedVector &lookAt, 
						   FixedVector &lookFrom)
{
	lookAt_ = lookAt;
	lookFrom_ = lookFrom;
}

void ViewPoints::simulate(fixed frameTime)
{
	if (getLookAtCount() == 0) return;

	FixedVector max, min;
	bool firstItor = true;
	fixed count = 0;

	static FixedVector lookAt;
	static FixedVector lookFrom;
	lookAt.zero();
	lookFrom.zero();

	std::list<ViewPoint *>::iterator itor =
		points_.begin();
	std::list<ViewPoint *>::iterator enditor =
		points_.end();
	for (; itor != enditor; itor++)
	{
		FixedVector &itorPosition = (*itor)->getPosition();
		FixedVector &itorLookAt = (*itor)->getLookFrom();
		fixed itorRadius = (*itor)->getRadius();

		if (firstItor)
		{
			firstItor = false;
			min = itorPosition;
			max = itorPosition;
		}

		min[0] = MIN(min[0], itorPosition[0] - itorRadius);
		min[1] = MIN(min[1], itorPosition[1] - itorRadius);
		min[2] = MIN(min[2], itorPosition[2] - itorRadius);
		max[0] = MAX(max[0], itorPosition[0] + itorRadius);
		max[1] = MAX(max[1], itorPosition[1] + itorRadius);
		max[2] = MAX(max[2], itorPosition[2] + itorRadius);		

		lookAt += itorPosition;
		lookFrom += itorLookAt;
		count += 1;
	}

	fixed dist = 25;
	fixed maxMin = (max - min).Magnitude();
	if (maxMin > 0)
	{
		dist = maxMin + 25;
	}
	lookFrom.StoreNormalize();

	lookAt /= count;
	lookFrom *= dist;

	lookAt_ = lookAt;
	lookFrom_ = lookFrom;
}

int ViewPoints::getLookAtCount()
{
	return (int) points_.size();
}

ViewPoints::ViewPoint *ViewPoints::getNewViewPoint(unsigned int playerId)
{
	if (context_->getServerMode()) return 0;
	if (playerId == 0) return 0;

	if (context_->getTankContainer().getCurrentPlayerId() != playerId)
	{
		return 0;
	}

	ViewPoint *viewpoint = new ViewPoint();
	points_.push_back(viewpoint);
	return viewpoint;
}

void ViewPoints::explosion(unsigned int playerId, FixedVector &position)
{
	if (context_->getServerMode()) return;

	if (context_->getTankContainer().getCurrentPlayerId() != playerId)
	{
		return;
	}

	explosionPosition_ = position;
}

void ViewPoints::releaseViewPoint(ViewPoint *point)
{
	points_.remove(point);
}
