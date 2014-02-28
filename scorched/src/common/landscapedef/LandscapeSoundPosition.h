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

#if !defined(__INCLUDE_LandscapeSoundPositionh_INCLUDE__)
#define __INCLUDE_LandscapeSoundPositionh_INCLUDE__

#include <XML/XMLEntrySimpleTypes.h>

class LandscapeSoundItem 
{
public:
	LandscapeSoundItem()
	{
	}
	virtual ~LandscapeSoundItem()
	{
	}
};

class VirtualSoundSource;
class LandscapeSoundPosition : public XMLEntryContainer
{
public:
	LandscapeSoundPosition(const char *type, const char *description);
	virtual ~LandscapeSoundPosition();

	virtual bool setPosition(VirtualSoundSource *source, LandscapeSoundItem *data) = 0;

	virtual int getInitCount() { return 1; }
	virtual LandscapeSoundItem *getInitData(int count) { return 0; }
};

class LandscapeSoundPositionChoice : public XMLEntryTypeChoice<LandscapeSoundPosition>
{
public:
	LandscapeSoundPositionChoice();
	virtual ~LandscapeSoundPositionChoice();

	virtual LandscapeSoundPosition *createXMLEntry(const std::string &type);
};

class LandscapeSoundPositionAmbient : public LandscapeSoundPosition
{
public:
	LandscapeSoundPositionAmbient();
	virtual ~LandscapeSoundPositionAmbient();

	virtual bool setPosition(VirtualSoundSource *source, LandscapeSoundItem *data);
};

class LandscapeSoundPositionAbsoulte : public LandscapeSoundPosition
{
public:
	LandscapeSoundPositionAbsoulte();
	virtual ~LandscapeSoundPositionAbsoulte();

	virtual bool setPosition(VirtualSoundSource *source, LandscapeSoundItem *data);
protected:
	XMLEntryFixedVector position;
};

class LandscapeSoundPositionWater : public LandscapeSoundPosition
{
public:
	LandscapeSoundPositionWater();
	virtual ~LandscapeSoundPositionWater();

	virtual bool setPosition(VirtualSoundSource *source, LandscapeSoundItem *data);
protected:
	XMLEntryFixed falloff;
};

class LandscapeSoundPositionGroup : public LandscapeSoundPosition
{
public:
	LandscapeSoundPositionGroup();
	virtual ~LandscapeSoundPositionGroup();
	
	virtual bool setPosition(VirtualSoundSource *source, LandscapeSoundItem *data);
protected:
	XMLEntryString name;
	XMLEntryFixed falloff;
};

class ObjectGroupEntryReference;
class LandscapeSoundPositionSetItem : public LandscapeSoundItem
{
public:
	LandscapeSoundPositionSetItem(ObjectGroupEntryReference *reference);
	virtual ~LandscapeSoundPositionSetItem();

	ObjectGroupEntryReference *getReference() { return reference_; }

private:
	ObjectGroupEntryReference *reference_;
};

class LandscapeSoundPositionSet : public LandscapeSoundPosition
{
public:
	LandscapeSoundPositionSet();
	virtual ~LandscapeSoundPositionSet();

	virtual bool setPosition(VirtualSoundSource *source, LandscapeSoundItem *data);

	virtual int getInitCount();
	virtual LandscapeSoundItem *getInitData(int count);
protected:
	XMLEntryString name;
	XMLEntryInt maxsounds;
};

#endif // __INCLUDE_LandscapeSoundPositionh_INCLUDE__
