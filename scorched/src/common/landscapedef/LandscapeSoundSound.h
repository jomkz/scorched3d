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

#if !defined(__INCLUDE_LandscapeSoundSoundh_INCLUDE__)
#define __INCLUDE_LandscapeSoundSoundh_INCLUDE__

#include <XML/XMLEntrySimpleTypes.h>

class VirtualSoundSource;
class LandscapeSoundSound : public XMLEntryContainer
{
public:
	LandscapeSoundSound(const char *type, const char *description);
	virtual ~LandscapeSoundSound();

	virtual bool play(VirtualSoundSource *source, float ambientGain) = 0;
	virtual float getGain() = 0;
};

class LandscapeSoundSoundChoice : public XMLEntryTypeChoice<LandscapeSoundSound>
{
public:
	LandscapeSoundSoundChoice();
	virtual ~LandscapeSoundSoundChoice();

	virtual LandscapeSoundSound *createXMLEntry(const std::string &type);
	virtual void getAllTypes(std::set<std::string> &allTypes);
};

class LandscapeSoundSoundFileList : public XMLEntryList<XMLEntryString>
{
public:
	LandscapeSoundSoundFileList();
	virtual ~LandscapeSoundSoundFileList();

	virtual XMLEntryString *createXMLEntry();
};

class LandscapeSoundSoundFile : public LandscapeSoundSound
{
public:
	LandscapeSoundSoundFile();
	virtual ~LandscapeSoundSoundFile();

	virtual bool play(VirtualSoundSource *source, float ambientGain);
	virtual float getGain() { return gain.getValue().asFloat(); }

protected:
	LandscapeSoundSoundFileList files;
	XMLEntryFixed gain;
	XMLEntryFixed referencedistance;
	XMLEntryFixed rolloff;
};

#endif // __INCLUDE_LandscapeSoundSoundh_INCLUDE__
