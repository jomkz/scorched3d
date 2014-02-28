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

#include <landscapedef/LandscapeSoundSound.h>
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

LandscapeSoundSoundChoice::LandscapeSoundSoundChoice() :
	XMLEntryTypeChoice<LandscapeSoundSound>("sound", 
		"Associates sounds with a given position")
{
}

LandscapeSoundSoundChoice::~LandscapeSoundSoundChoice()
{
}

LandscapeSoundSound *LandscapeSoundSoundChoice::createXMLEntry(const std::string &type)
{
	if (0 == strcmp(type.c_str(), "file")) return new LandscapeSoundSoundFile;
	S3D::dialogMessage("LandscapeSoundSoundChoice", S3D::formatStringBuffer("Unknown sound type %s", type));
	return 0;
}

LandscapeSoundSound::LandscapeSoundSound(const char *name, const char *description) :
	XMLEntryContainer("LandscapeSoundSound", 
		"Specifies where to load sounds files from")
{
}

LandscapeSoundSound::~LandscapeSoundSound()
{
}

LandscapeSoundSoundFileList::LandscapeSoundSoundFileList() :
	XMLEntryList<XMLEntryString>("file", 
		"A list of files to load sounds from, a random file will be chosen each time a sound is played.")
{
}

LandscapeSoundSoundFileList::~LandscapeSoundSoundFileList()
{
}

XMLEntryString *LandscapeSoundSoundFileList::createXMLEntry()
{
	return new XMLEntryString("file", "A file to load sounds from");
}

LandscapeSoundSoundFile::LandscapeSoundSoundFile() :
	LandscapeSoundSound("LandscapeSoundSoundFile", 
		"Loads a sound file from a file on the file system"),
	gain("gain", "The amound of extra gain (volume) to apply to the sound, 1.0 = no gain", 0, 1),
	files(),
	referencedistance("referencedistance", "", 0, 75),
	rolloff("rolloff", "", 0, 1)
{
}

LandscapeSoundSoundFile::~LandscapeSoundSoundFile()
{
}

bool LandscapeSoundSoundFile::readXML(XMLNode *node)
{
	if (!LandscapeSoundSound::readXML(node)) return false;
	if (files.getChildren().empty()) return node->returnError("No file node");
	return true;
}

bool LandscapeSoundSoundFile::play(VirtualSoundSource *source, float ambientGain)
{
#ifndef S3D_SERVER
	/*
	std::string &file = files[rand() % files.size()];
	SoundBuffer *buffer = 
		Sound::instance()->fetchOrCreateBuffer(file.c_str());
	if (!buffer) return false;

	source->setGain(gain * ambientGain);
	source->setRolloff(rolloff);
	source->setReferenceDistance(referencedistance);
	source->play(buffer);
	*/
#endif

	return true;
}
