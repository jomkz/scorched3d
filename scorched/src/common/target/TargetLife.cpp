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

#include <target/TargetLife.h>
#include <target/TargetRenderer.h>
#include <target/TargetSpace.h>
#include <target/TargetState.h>
#include <target/Target.h>
#include <tank/TankType.h>
#include <engine/ScorchedContext.h>
#include <engine/ActionController.h>
#include <tankai/TankAIAdder.h>
#include <common/Defines.h>
#include <common/Logger.h>

TargetLife::TargetLife(ScorchedContext &context, unsigned int playerId) :
	context_(context), sphereGeom_(true),
	life_(0), maxLife_(1), target_(0),
	size_(2, 2, 2), floatBoundingSize_(0.0f)
{
}

TargetLife::~TargetLife()
{
	life_ = 0;
	updateSpace();
}

void TargetLife::newGame()
{
	setLife(maxLife_);
}

void TargetLife::setLife(fixed life)
{
	life_ = life;

	if (life_ >= maxLife_) life_ = maxLife_;
	if (life_ < 1)
	{
		life_ = 0;
		setRotation(0); // Updates space too
	}
	else
	{
		updateAABB();
		updateSpace();
	}
}

void TargetLife::setSize(FixedVector &size)
{
	size_ = size;

	updateAABB();
	updateSpace();
}

void TargetLife::setTargetPositionAndRotation(FixedVector &pos, fixed rotation)
{
	targetPosition_ = pos;
	if (!context_.getServerMode()) targetPosition_.asVector(floatPosition_);

	FixedVector zaxis(0, 0, 1);
	quaternion_.setQuatFromAxisAndAngle(zaxis, rotation / 180 * fixed::XPI);
	if (!context_.getServerMode()) quaternion_.getOpenGLRotationMatrix(floatRotMatrix_);

	updateAABB();
	updateSpace();
}

void TargetLife::setTargetPosition(FixedVector &pos)
{
	targetPosition_ = pos;
	if (!context_.getServerMode()) targetPosition_.asVector(floatPosition_);

	updateSpace();
}

void TargetLife::setRotation(fixed rotation)
{
	FixedVector zaxis(0, 0, 1);
	quaternion_.setQuatFromAxisAndAngle(zaxis, rotation / 180 * fixed::XPI);
	if (!context_.getServerMode()) quaternion_.getOpenGLRotationMatrix(floatRotMatrix_);

	updateAABB();
	updateSpace();
}

FixedVector &TargetLife::getCenterPosition()
{
	static FixedVector result;
	result = getTargetPosition();
	result[2] += getSize()[2] / 2;
	return result;
}

Vector &TargetLife::getFloatCenterPosition()
{
	static Vector result;
	result = getFloatPosition();
	result[2] += getFloatAabbSize()[2] / 2.0f;
	return result;
}

fixed TargetLife::collisionDistance(FixedVector &position)
{
	FixedVector &currentPosition = getCenterPosition();
	FixedVector direction = position - currentPosition;
	fixed dist = 0;

	// Get how close the explosion was
	if (getBoundingSphere())
	{
		// Find how close the explosion was to the 
		// outside of the sphere
		fixed sphereRadius = MAX(MAX(size_[0], size_[1]), size_[2]) / 2;
		dist = direction.Magnitude() - sphereRadius;
		if (dist < 0) dist = 0;
	}
	else
	{
		// Make the direction relative to the geom
		// incase the geom has been rotated
		FixedVector relativeDirection;
		quaternion_.getRelativeVector(relativeDirection, direction);

		// Find how close the explosion was to the 
		// outside edge of the cube
		FixedVector touchPosition = relativeDirection;

		// Check each size of the cube to see if the point is outside.
		// If it is, then scale it back until the point sits on the
		// outside edge of the cube.
		int inner = 0;
		for (int i=0; i<3; i++)
		{
			fixed halfSize = size_[i] / 2;
			if (touchPosition[i] < -halfSize)
			{
				// Scale the point so it sits on this edge
				fixed diff = -halfSize / touchPosition[i];
				touchPosition *= diff;
			}
			else if (touchPosition[i] > halfSize)
			{
				// Scale the point so it sits on this edge
				fixed diff = halfSize / touchPosition[i];
				touchPosition *= diff;
			}
			else inner++; // The point is inside this edge
		}

		if (inner == 3)
		{
			// We are inside the cube
			dist = 0;
		}
		else
		{
			// We are outside the cube
			relativeDirection -= touchPosition;
			dist = relativeDirection.Magnitude();
		}
	}

	return dist;
}

bool TargetLife::collision(FixedVector &position)
{
	FixedVector &currentPosition = getCenterPosition();
	FixedVector direction = position - currentPosition;

	// Check against bounding box
	if (direction[0] < -aabbSize_[0] ||
		direction[0] > aabbSize_[0] ||
		direction[1] < -aabbSize_[1] ||
		direction[1] > aabbSize_[1] ||
		direction[2] < -aabbSize_[2] ||
		direction[2] > aabbSize_[2])
	{
		return false;
	}

	// Check against actual bounds
	if (sphereGeom_)
	{
		fixed radius = MAX(MAX(size_[0], size_[1]), size_[2]) / 2;
		if (direction.Magnitude() > radius) return false;
	}
	else
	{
		// Make the direction relative to the geom
		// incase the geom has been rotated
		FixedVector relativeDirection;
		quaternion_.getRelativeVector(relativeDirection, direction);

		// Check each side of the cube to see if the point is inside it
		int inner = 0;
		for (int i=0; i<3; i++)
		{
			fixed halfSize = size_[i] / 2;
			if (-halfSize < relativeDirection[i] && 
				relativeDirection[i] < halfSize)
			{
				inner++; // The point is inside this edge
			}
		}

		// We need the point to be inside each of the 6 edges
		if (inner < 3) return false;
	}

	return true;
}

void TargetLife::setBoundingSphere(bool sphereGeom)
{ 
	sphereGeom_ = sphereGeom; 

	updateAABB();
	updateSpace();
}

void TargetLife::updateSpace()
{
	if (target_->getRenderer()) target_->getRenderer()->moved();
	context_.getTargetSpace().updateTarget(target_);
}

bool TargetLife::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(maxLife_);
	buffer.addToBuffer(life_);
	buffer.addToBuffer(size_);
	buffer.addToBuffer(velocity_);
	buffer.addToBuffer(targetPosition_);
	buffer.addToBuffer(quaternion_);
	return true;
}

bool TargetLife::readMessage(NetBufferReader &reader)
{
	life_ = 0; // Suppress target space updates

	fixed newLife;
	if (!reader.getFromBuffer(maxLife_))
	{
		Logger::log("TargetLife::maxLife_ read failed");
		return false;
	}
	if (!reader.getFromBuffer(newLife))
	{
		Logger::log("TargetLife::life_ read failed");
		return false;
	}

	FixedVector newSize;
	if (!reader.getFromBuffer(newSize))
	{
		Logger::log("TargetLife::size_ read failed");
		return false;
	}
	setSize(newSize);

	if (!reader.getFromBuffer(velocity_))
	{
		Logger::log("TargetLife::velocity_ read failed");
		return false;
	}

	FixedVector newPosition;
	if (!reader.getFromBuffer(newPosition))
	{
		Logger::log("TargetLife::targetPosition_ read failed");
		return false;
	}
	setTargetPosition(newPosition);

	if (!reader.getFromBuffer(quaternion_))
	{
		Logger::log("TargetLife::quaternion_ read failed");
		return false;
	}

	// Update target space
	setLife(newLife);

	return true;
}

void TargetLife::updateAABB()
{
	if (sphereGeom_)
	{
		fixed radius = MAX(MAX(size_[0], size_[1]), size_[2]) / 2;
		aabbSize_ = FixedVector(radius * 2, radius * 2, radius * 2);
	}
	else
	{
		for (int i=0; i<8; i++)
		{
			FixedVector position;
			switch (i)
			{
			case 0:
				position = FixedVector(size_[0], size_[1], size_[2]);
				break;
			case 1:
				position = FixedVector(-size_[0], size_[1], size_[2]);
				break;
			case 2:
				position = FixedVector(size_[0], -size_[1], size_[2]);
				break;
			case 3:
				position = FixedVector(-size_[0], -size_[1], size_[2]);
				break;
			case 4:
				position = FixedVector(size_[0], size_[1], -size_[2]);
				break;
			case 5:
				position = FixedVector(-size_[0], size_[1], -size_[2]);
				break;
			case 6:
				position = FixedVector(size_[0], -size_[1], -size_[2]);
				break;
			case 7:
				position = FixedVector(-size_[0], -size_[1], -size_[2]);
				break;
			}

			FixedVector result;
			quaternion_.getRelativeVector(result, position);

			if (i == 0) aabbSize_ = result;
			else
			{
				aabbSize_[0] = MAX(aabbSize_[0], result[0]);
				aabbSize_[1] = MAX(aabbSize_[1], result[1]);
				aabbSize_[2] = MAX(aabbSize_[2], result[2]);
			}
		}
	}
	if (!context_.getServerMode())
	{
		aabbSize_.asVector(floatAabbSize_);
		floatBoundingSize_ = floatAabbSize_.Max();
	}
}
