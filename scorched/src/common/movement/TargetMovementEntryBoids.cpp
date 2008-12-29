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

#include <movement/TargetMovementEntryBoids.h>
#include <movement/Boid2.h>
#include <common/Defines.h>
#include <common/RandomGenerator.h>
#include <engine/ScorchedContext.h>
#include <target/Target.h>
#include <target/TargetLife.h>
#include <target/TargetState.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapedef/LandscapeMovement.h>

TargetMovementEntryBoids::TargetMovementEntryBoids() :
	movementNumber_(0)
{
}

TargetMovementEntryBoids::~TargetMovementEntryBoids()
{
}

void TargetMovementEntryBoids::generate(ScorchedContext &context, 
	RandomGenerator &random, LandscapeMovementType *movementType)
{
	LandscapeMovementTypeBoids *boids = 
		(LandscapeMovementTypeBoids *) movementType;

	// Set the boid parameters
	cruiseDistance_ = boids->cruisedistance * fixed(10);
	maxVelocity_ = boids->maxvelocity * fixed(true, 750);
	cruiseVelocity_ = boids->maxvelocity * fixed(true, 400);
	maxAcceleration_ = boids->maxacceleration * fixed(true, 20);
	minBounds_ = boids->minbounds;
	maxBounds_ = boids->maxbounds;

	// Find the group to move the objects in
	groupEntry_ = context.getLandscapeMaps().getGroundMaps().getGroups().
		getGroup(boids->groupname.c_str());
	if (!groupEntry_)
	{
		S3D::dialogExit("TargetMovementEntryBoids", 
			S3D::formatStringBuffer("Group entry %s has no objects defined for it", 
			boids->groupname.c_str()));
	}

	// Create boids
	makeBoids(context, random, boids->maxbounds, boids->minbounds);
}

void TargetMovementEntryBoids::makeBoids(ScorchedContext &context, 
	RandomGenerator &random, FixedVector &maxBounds, FixedVector &minBounds)
{
	// Generate the list of offsets for all of the targets in the group
	std::map<unsigned int, TargetGroup *> &objects = groupEntry_->getObjects();
	std::map<unsigned int, TargetGroup *>::iterator itor;
	for (itor = objects.begin();
		itor != objects.end();
		itor++)
	{
		unsigned int playerId = (*itor).first;
		TargetGroup *groupEntry = (*itor).second;
		makeBoid(context, groupEntry);
	}
}

Boid2 *TargetMovementEntryBoids::makeBoid(ScorchedContext &context, TargetGroup *groupEntry)
{
	if (!groupEntry->getTarget()->isTarget() ||
		groupEntry->getTarget()->getPlayerId() >= TargetID::MIN_TARGET_TRANSIENT_ID)
	{
		S3D::dialogExit("TargetMovementEntryBoids",
			"Movement can be assigned to level targets only (no tanks)");
	}
	if (groupEntry->getTarget()->getTargetState().getMovement())
	{
		S3D::dialogExit("TargetMovementEntryBoids",
			"Only one movement can be assigned to each target");
	}

	// Set this target as moving
	Boid2 *boid = new Boid2(context, groupEntry->getTarget(), this);
	groupEntry->getTarget()->getTargetState().setMovement(boid);
	return boid;
}

void TargetMovementEntryBoids::simulate(ScorchedContext &context, fixed frameTime)
{
	movementNumber_++;
	std::vector<Boid2*> boidSet;

	// For each target set position and rotation based on its offset
	std::map<unsigned int, TargetGroup *> &objects = groupEntry_->getObjects();
	std::map<unsigned int, TargetGroup *>::iterator itor;
	for (itor = objects.begin();
		itor != objects.end();
		itor++)
	{
		unsigned int playerId = (*itor).first;
		TargetGroup *groupEntry = (*itor).second;

		Boid2 *boid = (Boid2 *) groupEntry->getTarget()->getTargetState().getMovement();
		if (!boid) boid = makeBoid(context, groupEntry);

		boidSet.push_back(boid);
		if (boidSet.size() == 5)
		{
			processSet(frameTime, boidSet);
			boidSet.clear();
		}
	}

	processSet(frameTime, boidSet);
}

void TargetMovementEntryBoids::processSet(fixed frameTime, std::vector<Boid2*> &boidSet)
{
	if (boidSet.empty()) return;

	std::vector<Boid2*>::iterator itor;
	for (itor = boidSet.begin();
		itor != boidSet.end();
		itor++)
	{
		Boid2 *boid = (*itor);
		boid->update(frameTime, boidSet, (movementNumber_ % 10) == 0);
	}
}

bool TargetMovementEntryBoids::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(movementNumber_);
	return true;
}

bool TargetMovementEntryBoids::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(movementNumber_)) return false;
	return true;
}

void TargetMovementEntryBoids::draw()
{
}
