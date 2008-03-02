////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
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

#include <landscapedef/LandscapeSound.h>
#include <landscapemap/LandscapeMaps.h>
#ifndef S3D_SERVER
	#include <landscape/Landscape.h>
	#include <graph/MainCamera.h>
	#include <sound/Sound.h>
	#include <water/Water.h>
	#include <client/ScorchedClient.h>
#endif
#include <target/Target.h>
#include <target/TargetLife.h>
#include <common/Defines.h>
#include <math.h>

bool LandscapeSoundPositionSet::readXML(XMLNode *node)
{
	if (!node->getNamedChild("name", name)) return false;
	if (!node->getNamedChild("maxsounds", maxsounds)) return false;
	
	return node->failChildren();
}

bool LandscapeSoundPositionSet::setPosition(VirtualSoundSource *source, unsigned int data)
{
#ifndef S3D_SERVER
	TargetGroupsGroupEntry *groupEntry =
		ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getGroups().getGroup(
			name.c_str());
	if (!groupEntry) return false;
	if (groupEntry->getObjectCount() <= 0) return false;

	TargetGroup *obj = groupEntry->getObjectById(data);
	if (!obj) return false;

	Vector position = obj->getPosition().asVector();
	Vector velocity = obj->getTarget()->getLife().getVelocity().asVector();
	source->setPosition(position);
	source->setVelocity(velocity);
#endif

	return true;
}

int LandscapeSoundPositionSet::getInitCount()
{
#ifndef S3D_SERVER
	TargetGroupsGroupEntry *groupEntry =
		ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getGroups().getGroup(
			name.c_str());
	if (groupEntry) return (MIN(maxsounds, groupEntry->getObjectCount()));	
#endif

	return 0;
}

unsigned int LandscapeSoundPositionSet::getInitData(int count)
{
#ifndef S3D_SERVER
	TargetGroupsGroupEntry *groupEntry =
		ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getGroups().getGroup(
			name.c_str());
	if (groupEntry)
	{
		unsigned int playerId = groupEntry->getObjectByPos(count)->getTarget()->getPlayerId();
		return playerId;
	}
#endif

	return 0;
}

bool LandscapeSoundPositionGroup::readXML(XMLNode *node)
{
	if (!node->getNamedChild("falloff", falloff)) return false;
	if (!node->getNamedChild("name", name)) return false;
	return node->failChildren();
}

bool LandscapeSoundPositionGroup::setPosition(VirtualSoundSource *source, unsigned int data)
{
#ifndef S3D_SERVER
	Vector &cameraPos = 
		MainCamera::instance()->getCamera().getCurrentPos();
	int groundMapWidth = ScorchedClient::instance()->
		getLandscapeMaps().getGroundMaps().getMapWidth();
	int groundMapHeight = ScorchedClient::instance()->
		getLandscapeMaps().getGroundMaps().getMapHeight();

	int a = int(cameraPos[0]);
	int b = int(cameraPos[1]);
	int x = MAX(0, MIN(a, groundMapWidth));
	int y = MAX(0, MIN(b, groundMapHeight));

	float distance = 255.0f;
	TargetGroupsGroupEntry *groupEntry =
		ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getGroups().getGroup(
			name.c_str());
	if (groupEntry)
	{
		if (groupEntry->getObjectCount() <= 0) return false;

		distance = groupEntry->getDistance(x, y);
	}
	distance += fabsf(float(a - x)) + fabsf(float(b - y));
	distance *= 4.0f * falloff;

	Vector position(0.0f, 0.0f, distance + cameraPos[2]);

	source->setRelative();
	source->setPosition(position);
#endif

	return true;
}

bool LandscapeSoundPositionWater::readXML(XMLNode *node)
{
	if (!node->getNamedChild("falloff", falloff)) return false;
	return node->failChildren();
}

bool LandscapeSoundPositionWater::setPosition(VirtualSoundSource *source, unsigned int data)
{
#ifndef S3D_SERVER
	Vector &cameraPos = 
		MainCamera::instance()->getCamera().getCurrentPos();

	float distance = Landscape::instance()->getWater().
		getWaveDistance(int(cameraPos[0]), int(cameraPos[1]));
	distance *= 4.0f * falloff;

	Vector position(0.0f, 0.0f, distance + cameraPos[2]);

	source->setRelative();
	source->setPosition(position);
#endif

	return true;
}

bool LandscapeSoundPositionAmbient::readXML(XMLNode *node)
{
	return node->failChildren();
}

bool LandscapeSoundPositionAmbient::setPosition(VirtualSoundSource *source, unsigned int data)
{
#ifndef S3D_SERVER
	source->setRelative();
	source->setPosition(Vector::getNullVector());
#endif

	return true;
}

bool LandscapeSoundPositionAbsoulte::readXML(XMLNode *node)
{
	if (!node->getNamedChild("position", position)) return false;
	return node->failChildren();
}

bool LandscapeSoundPositionAbsoulte::setPosition(VirtualSoundSource *source, unsigned int data)
{
#ifndef S3D_SERVER
	source->setPosition(position);
#endif
	return true;
}

bool LandscapeSoundTimingLooped::readXML(XMLNode *node)
{
	return node->failChildren();
}

float LandscapeSoundTimingLooped::getNextEventTime()
{
	return -1.0f;
}

bool LandscapeSoundTimingRepeat::readXML(XMLNode *node)
{
	if (!node->getNamedChild("min", min)) return false;
	if (!node->getNamedChild("max", max)) return false;
	return node->failChildren();
}

float LandscapeSoundTimingRepeat::getNextEventTime()
{
	return min + max * RAND;
}

bool LandscapeSoundSoundFile::readXML(XMLNode *node)
{
	std::string file;
	while (node->getNamedChild("file", file, false))
	{
		if (!S3D::checkDataFile(file.c_str())) return false;
		files.push_back(file);
	}
	if (files.empty()) return node->returnError("No file node");

	gain = 1.0f;
	node->getNamedChild("gain", gain, false);
	referencedistance = 75.0f;
	node->getNamedChild("referencedistance", referencedistance, false);
	rolloff = 1.0f;
	node->getNamedChild("rolloff", rolloff, false);

	return node->failChildren();
}

bool LandscapeSoundSoundFile::play(VirtualSoundSource *source)
{
#ifndef S3D_SERVER
	std::string &file = files[rand() % files.size()];
	SoundBuffer *buffer = 
		Sound::instance()->fetchOrCreateBuffer(
			S3D::getDataFile(file.c_str()));
	if (!buffer) return false;

	source->setGain(gain);
	source->setRolloff(rolloff);
	source->setReferenceDistance(referencedistance);
	source->play(buffer);
#endif

	return true;
}

LandscapeSoundType::LandscapeSoundType() : 
	position(0), timing(0), sound(0)
{
}

LandscapeSoundType::~LandscapeSoundType()
{
	delete position;
	delete timing;
	delete sound;
}

bool LandscapeSoundType::readXML(XMLNode *node)
{
	{
		std::string soundtype;
		XMLNode *soundNode;
		if (!node->getNamedChild("sound", soundNode)) return false;
		if (!soundNode->getNamedParameter("type", soundtype)) return false;
		if (0 == strcmp(soundtype.c_str(), "file"))
			sound = new LandscapeSoundSoundFile;
		else return false;

		if (!sound->readXML(soundNode)) return false;
	}

	{
		std::string positiontype;
		XMLNode *positionNode;
		if (!node->getNamedChild("position", positionNode)) return false;
		if (!positionNode->getNamedParameter("type", positiontype)) return false;
		if (0 == strcmp(positiontype.c_str(), "ambient"))
			position = new LandscapeSoundPositionAmbient;
		else if (0 == strcmp(positiontype.c_str(), "absolute"))
			position = new LandscapeSoundPositionAbsoulte;
		else if (0 == strcmp(positiontype.c_str(), "water"))
			position = new LandscapeSoundPositionWater;
		else if (0 == strcmp(positiontype.c_str(), "group"))
			position = new LandscapeSoundPositionGroup;
		else if (0 == strcmp(positiontype.c_str(), "set"))
			position = new LandscapeSoundPositionSet;
		else return false;
		if (!position->readXML(positionNode)) return false;
	}
	{
		std::string timingtype;
		XMLNode *timingNode;
		if (!node->getNamedChild("timing", timingNode)) return false;
		if (!timingNode->getNamedParameter("type", timingtype)) return false;
		if (0 == strcmp(timingtype.c_str(), "looped"))
			timing = new LandscapeSoundTimingLooped;
		else if (0 == strcmp(timingtype.c_str(), "repeat"))
			timing = new LandscapeSoundTimingRepeat;
		else return false;
		if (!timing->readXML(timingNode)) return false;
	}
	return node->failChildren();
}
