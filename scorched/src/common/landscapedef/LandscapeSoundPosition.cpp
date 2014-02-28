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

#include <landscapedef/LandscapeSoundPosition.h>
#include <landscapemap/LandscapeMaps.h>
#ifndef S3D_SERVER
	#include <client/ScorchedClient.h>
#endif
#include <engine/ObjectGroups.h>
#include <engine/ObjectGroup.h>
#include <engine/ObjectGroupEntry.h>
#include <target/Target.h>
#include <target/TargetLife.h>
#include <target/TargetSpace.h>
#include <target/TargetGroup.h>
#include <common/Defines.h>
#include <math.h>

LandscapeSoundPositionChoice::LandscapeSoundPositionChoice() :
	XMLEntryTypeChoice<LandscapeSoundPosition>(
		"LandscapeSoundPositionChoice",
		"Associates sounds with a given position")
{
}

LandscapeSoundPositionChoice::~LandscapeSoundPositionChoice()
{
}

LandscapeSoundPosition *LandscapeSoundPositionChoice::createXMLEntry(const std::string &type)
{
	if (0 == strcmp(type.c_str(), "ambient")) return new LandscapeSoundPositionAmbient;
	if (0 == strcmp(type.c_str(), "absolute")) return new LandscapeSoundPositionAbsoulte;
	if (0 == strcmp(type.c_str(), "water")) return new LandscapeSoundPositionWater;
	if (0 == strcmp(type.c_str(), "group")) return new LandscapeSoundPositionGroup;
	if (0 == strcmp(type.c_str(), "set")) return new LandscapeSoundPositionSet;
	S3D::dialogMessage("LandscapeSoundPositionChoice", S3D::formatStringBuffer("Unknown sound type %s", type));
	return 0;
}

void LandscapeSoundPositionChoice::getAllTypes(std::set<std::string> &allTypes)
{
	allTypes.insert("ambient");
	allTypes.insert("absolute");
	allTypes.insert("water");
	allTypes.insert("group");
	allTypes.insert("set");
}

LandscapeSoundPosition::LandscapeSoundPosition(const char *type, const char *description) :
	XMLEntryContainer(type, description)
{
}

LandscapeSoundPosition::~LandscapeSoundPosition()
{
}

LandscapeSoundPositionSetItem::LandscapeSoundPositionSetItem(
	ObjectGroupEntryReference *reference) : reference_(reference)
{
}

LandscapeSoundPositionSetItem::~LandscapeSoundPositionSetItem()
{
	delete reference_;
}

LandscapeSoundPositionSet::LandscapeSoundPositionSet() :
	LandscapeSoundPosition("LandscapeSoundPositionSet",
		"Sound location from an object in the specified object group"),
	name("The name of the object group to take objects from"),
	maxsounds("The maximum number of sounds to play at the same time")
{
	addChildXMLEntry("name", &name, "maxsounds", &maxsounds);
}

LandscapeSoundPositionSet::~LandscapeSoundPositionSet()
{
}

bool LandscapeSoundPositionSet::setPosition(VirtualSoundSource *source, LandscapeSoundItem *data)
{
#ifndef S3D_SERVER
	if (!data) return false;
	LandscapeSoundPositionSetItem *item = (LandscapeSoundPositionSetItem *) data;
	ObjectGroupEntryReference *reference = item->getReference();
	ObjectGroupEntry *entry = reference->getEntry();
	if (!entry) return false;

	Vector position = entry->getPosition().asVector();
	Vector velocity = entry->getVelocity().asVector();
	//source->setPosition(position);
	//source->setVelocity(velocity);
#endif

	return true;
}

int LandscapeSoundPositionSet::getInitCount()
{
#ifndef S3D_SERVER
	ObjectGroup *objectGroup =
		ScorchedClient::instance()->getObjectGroups().getGroup(name.getValue().c_str());
	if (objectGroup) return (S3D_MIN(maxsounds.getValue(), objectGroup->getObjectCount()));	
#endif

	return 0;
}

LandscapeSoundItem *LandscapeSoundPositionSet::getInitData(int count)
{
#ifndef S3D_SERVER
	ObjectGroup *objectGroup =
		ScorchedClient::instance()->getObjectGroups().getGroup(name.getValue().c_str());
	if (objectGroup)
	{
		ObjectGroupEntry *entry = objectGroup->getObjectByPos(count % objectGroup->getObjectCount());
		if (entry)
		{
			ObjectGroupEntryReference *reference = new ObjectGroupEntryReference(entry);
			return new LandscapeSoundPositionSetItem(reference);
		}
	}
#endif

	return 0;
}

LandscapeSoundPositionGroup::LandscapeSoundPositionGroup() :
	LandscapeSoundPosition("LandscapeSoundPositionGroup",
		"Sound location played when close to any object in the specified object group"),
	falloff("The distance that the sounds will disipate over"),
	name("The name of the object group to take objects from")
{
	addChildXMLEntry("name", &name, "falloff", &falloff);
}

LandscapeSoundPositionGroup::~LandscapeSoundPositionGroup()
{
}

bool LandscapeSoundPositionGroup::setPosition(VirtualSoundSource *source, LandscapeSoundItem *data)
{
#ifndef S3D_SERVER
	ObjectGroup *objectGroup =
		ScorchedClient::instance()->getObjectGroups().getGroup(name.getValue().c_str());
	if (!objectGroup) return false;
	if (objectGroup->getObjectCount() <= 0) return false;

	/*
	TODO
	TargetCamera *targetCamera = TargetCamera::getTargetCameraByName("main");
	if (!targetCamera) return false;
	Vector &cameraPos = targetCamera->getCamera().getCurrentPos();

	float distance = 255.0f;
	std::map<unsigned int, Target *> collisionTargets;
	FixedVector fixedCameraPos = FixedVector::fromVector(cameraPos);
	ScorchedClient::instance()->getTargetSpace().getCollisionSet(
		fixedCameraPos, 5, collisionTargets, false);
	std::map<unsigned int, Target *>::iterator itor;
	for (itor = collisionTargets.begin();
		itor != collisionTargets.end();
		itor++)
	{
		TargetGroup &targetGroup = itor->second->getGroup();
		if (targetGroup.getAllGroups().find(objectGroup) != targetGroup.getAllGroups().end())
		{
			float newDistance = (fixedCameraPos - itor->second->getLife().getTargetPosition()).Magnitude().asFloat();
			if (newDistance < distance)
			{
				distance = newDistance;
			}
		}
	}

	distance *= 4.0f * falloff;
	Vector position(0.0f, 0.0f, distance + cameraPos[2]);

	source->setRelative();
	source->setPosition(position);
	*/
#endif

	return true;
}

LandscapeSoundPositionWater::LandscapeSoundPositionWater() :
	LandscapeSoundPosition("LandscapeSoundPositionWater",
		"Sound location played when close to the water shore"),
	falloff("The distance that the sounds will disipate over")
{
	addChildXMLEntry("falloff", &falloff);
}

LandscapeSoundPositionWater::~LandscapeSoundPositionWater()
{
}

bool LandscapeSoundPositionWater::setPosition(VirtualSoundSource *source, LandscapeSoundItem *data)
{
#ifndef S3D_SERVER
	/*
	TargetCamera *targetCamera = TargetCamera::getTargetCameraByName("main");
	if (!targetCamera) return false;

	Vector &cameraPos = targetCamera->getCamera().getCurrentPos();

	float distance = Landscape::instance()->getWater().
		getWaveDistance(int(cameraPos[0]), int(cameraPos[1]));
	distance *= 4.0f * falloff;

	Vector position(0.0f, 0.0f, distance + cameraPos[2]);

	source->setRelative();
	source->setPosition(position);
	*/
#endif

	return true;
}

LandscapeSoundPositionAmbient::LandscapeSoundPositionAmbient() :
	LandscapeSoundPosition("LandscapeSoundPositionAmbient",
		"Sound location played from the location of the camera (i.e. with no position)")
{
}

LandscapeSoundPositionAmbient::~LandscapeSoundPositionAmbient()
{
}

bool LandscapeSoundPositionAmbient::setPosition(VirtualSoundSource *source, LandscapeSoundItem *data)
{
#ifndef S3D_SERVER
	//source->setRelative();
	//source->setPosition(Vector::getNullVector());
#endif

	return true;
}

LandscapeSoundPositionAbsoulte::LandscapeSoundPositionAbsoulte() :
	LandscapeSoundPosition("LandscapeSoundPositionAbsoulte",
		"Sound location played from an absolutely specified position"),
	position("The absolute position to play the sound from")
{
	addChildXMLEntry("position", &position);
}

LandscapeSoundPositionAbsoulte::~LandscapeSoundPositionAbsoulte()
{
}

bool LandscapeSoundPositionAbsoulte::setPosition(VirtualSoundSource *source, LandscapeSoundItem *data)
{
#ifndef S3D_SERVER
	//source->setPosition(position);
#endif
	return true;
}
