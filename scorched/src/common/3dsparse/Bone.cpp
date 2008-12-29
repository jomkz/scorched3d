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

#include <3dsparse/Bone.h>
#include <3dsparse/ModelMaths.h>
#include <common/Defines.h>

BoneType::BoneType() : parent_(-1)
{
}

BonePositionKey::BonePositionKey(float t, Vector &pos) :
	time(t), position(pos)
{
}

BoneRotationKey::BoneRotationKey(float t, Vector &rot) :
	time(t), rotation(rot)
{
}

Bone::Bone(const char *name) : name_(name)
{
	while (!positionKeys_.empty())
	{
		BonePositionKey *key = positionKeys_.back();
		positionKeys_.pop_back();
		delete key;
	}
	while (!rotationKeys_.empty())
	{
		BoneRotationKey *key = rotationKeys_.back();
		rotationKeys_.pop_back();
		delete key;
	}
}

Bone::~Bone()
{
}

Vector &Bone::getPositionAtTime(float currentTime)
{
	static Vector tmp;
	BonePositionKey *lastPositionKey = 0, *thisPositionKey = 0;
	std::vector<BonePositionKey *>::iterator itor;
	for (itor = positionKeys_.begin();
		itor != positionKeys_.end();
		itor++)
	{
		BonePositionKey *key = (*itor);
		if (key->time >= currentTime)
		{
			thisPositionKey = key;
			break;
		}
		lastPositionKey = key;
	}
	if (lastPositionKey && thisPositionKey)
	{
		float d = thisPositionKey->time - lastPositionKey->time;
		float s = (currentTime - lastPositionKey->time) / d;
		tmp[0] = lastPositionKey->position[0] + (thisPositionKey->position[0] - lastPositionKey->position[0]) * s;
		tmp[1] = lastPositionKey->position[1] + (thisPositionKey->position[1] - lastPositionKey->position[1]) * s;
		tmp[2] = lastPositionKey->position[2] + (thisPositionKey->position[2] - lastPositionKey->position[2]) * s;
	}
	else if (thisPositionKey)
	{
		return thisPositionKey->position;
	}
	else if (lastPositionKey)
	{
		return lastPositionKey->position;
	}

	return tmp;
}

void Bone::getRotationAtTime(float currentTime, BoneMatrixType &m)
{
	BoneRotationKey *lastRotationKey = 0, *thisRotationKey = 0;
	std::vector<BoneRotationKey *>::iterator itor;
	for (itor = rotationKeys_.begin();
		itor != rotationKeys_.end();
		itor++)
	{
		BoneRotationKey *key = (*itor);
		if (key->time >= currentTime)
		{
			thisRotationKey = key;
			break;
		}
		lastRotationKey = key;
	}
	if (lastRotationKey && thisRotationKey)
	{
		float d = thisRotationKey->time - lastRotationKey->time;
		float s = (currentTime - lastRotationKey->time) / d;

#ifndef MODEL_FAST_ANGLE_CALC
		float q1[4], q2[4], q[4];
		ModelMaths::angleQuaternion(lastRotationKey->rotation, q1);
		ModelMaths::angleQuaternion(thisRotationKey->rotation, q2);
		ModelMaths::quaternionSlerp(q1, q2, s, q);
		ModelMaths::quaternionMatrix(q, m);
#else
		Vector rot;
		rot[0] = lastRotationKey->rotation[0] + (thisRotationKey->rotation[0] - lastRotationKey->rotation[0]) * s;
		rot[1] = lastRotationKey->rotation[1] + (thisRotationKey->rotation[1] - lastRotationKey->rotation[1]) * s;
		rot[2] = lastRotationKey->rotation[2] + (thisRotationKey->rotation[2] - lastRotationKey->rotation[2]) * s;
		rot[0] *= 180.0f / PI;
		rot[1] *= 180.0f / PI;
		rot[2] *= 180.0f / PI;
		ModelMaths::angleMatrix(rot, m);
#endif
	}
	else if (thisRotationKey)
	{
		Vector rot;
		rot[0] = thisRotationKey->rotation[0] * 180.0f / PI;
		rot[1] = thisRotationKey->rotation[1] * 180.0f / PI;
		rot[2] = thisRotationKey->rotation[2] * 180.0f / PI;
		ModelMaths::angleMatrix(rot, m);
	}
	else if (lastRotationKey )
	{
		Vector rot;
		rot[0] = lastRotationKey->rotation[0] * 180.0f / PI;
		rot[1] = lastRotationKey->rotation[1] * 180.0f / PI;
		rot[2] = lastRotationKey->rotation[2] * 180.0f / PI;
		ModelMaths::angleMatrix(rot, m);
	}
}
