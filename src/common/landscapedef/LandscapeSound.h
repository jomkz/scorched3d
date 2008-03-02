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

#if !defined(__INCLUDE_LandscapeSoundh_INCLUDE__)
#define __INCLUDE_LandscapeSoundh_INCLUDE__

#include <XML/XMLFile.h>
#include <string>
#include <vector>

class LandscapeSoundTiming
{
public:
	virtual bool readXML(XMLNode *node) = 0;
	virtual float getNextEventTime() = 0;
};

class LandscapeSoundTimingLooped : public LandscapeSoundTiming
{
public:
	virtual bool readXML(XMLNode *node);
	virtual float getNextEventTime();
};

class LandscapeSoundTimingRepeat : public LandscapeSoundTiming
{
public:
	virtual bool readXML(XMLNode *node);
	virtual float getNextEventTime();

protected:
	float min, max;
};

class VirtualSoundSource;
class LandscapeSoundPosition
{
public:
	virtual bool readXML(XMLNode *node) = 0;
	virtual bool setPosition(VirtualSoundSource *source, unsigned int data) = 0;

	virtual int getInitCount() { return 1; }
	virtual unsigned int getInitData(int count) { return 0; }
};

class LandscapeSoundPositionAmbient : public LandscapeSoundPosition
{
public:
	virtual bool readXML(XMLNode *node);
	virtual bool setPosition(VirtualSoundSource *source, unsigned int data);
};

class LandscapeSoundPositionAbsoulte : public LandscapeSoundPosition
{
public:
	virtual bool readXML(XMLNode *node);
	virtual bool setPosition(VirtualSoundSource *source, unsigned int data);

protected:
	Vector position;
};

class LandscapeSoundPositionWater : public LandscapeSoundPosition
{
public:
	virtual bool readXML(XMLNode *node);
	virtual bool setPosition(VirtualSoundSource *source, unsigned int data);

protected:
	float falloff;
};

class LandscapeSoundPositionGroup : public LandscapeSoundPosition
{
public:
	virtual bool readXML(XMLNode *node);
	virtual bool setPosition(VirtualSoundSource *source, unsigned int data);

protected:
	std::string name;
	float falloff;
};

class LandscapeSoundPositionSet : public LandscapeSoundPosition
{
public:
	virtual bool readXML(XMLNode *node);
	virtual bool setPosition(VirtualSoundSource *source, unsigned int data);

	virtual int getInitCount();
	virtual unsigned int getInitData(int count);

protected:
	std::string name;
	int maxsounds;
};

class LandscapeSoundSound 
{
public:
	virtual bool readXML(XMLNode *node) = 0;
	virtual bool play(VirtualSoundSource *source) = 0;
};

class LandscapeSoundSoundFile : public LandscapeSoundSound
{
public:
	virtual bool readXML(XMLNode *node);
	virtual bool play(VirtualSoundSource *source);

protected:
	std::vector<std::string> files;
	float gain;
	float referencedistance;
	float rolloff;
};

class LandscapeSoundType
{
public:
	LandscapeSoundType();
	virtual ~LandscapeSoundType();

	virtual bool readXML(XMLNode *node);

	LandscapeSoundPosition *position;
	LandscapeSoundTiming *timing;
	LandscapeSoundSound *sound;
};

#endif // __INCLUDE_LandscapeSoundh_INCLUDE__
