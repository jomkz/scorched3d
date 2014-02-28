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

#if !defined(__INCLUDE_LandscapeIncludeh_INCLUDE__)
#define __INCLUDE_LandscapeIncludeh_INCLUDE__

#include <XML/XMLEntry.h>
#include <landscapedef/LandscapeSound.h>
#include <landscapedef/LandscapeEvent.h>
#include <landscapedef/LandscapeMovement.h>
#include <landscapedef/LandscapeSound.h>
#include <landscapedef/LandscapeMusic.h>
#include <landscapedef/LandscapeOptions.h>
#include <placement/PlacementType.h>

class LansdscapeIncludeList : public XMLEntryList<XMLEntryString>
{
public:
	LansdscapeIncludeList();
	virtual ~LansdscapeIncludeList();

	virtual XMLEntryString *createXMLEntry();
};

class LandscapeDefinitions;
class LandscapeInclude : public XMLEntryContainer
{
public:
	LandscapeInclude(LandscapeDefinitions *definitions, const char *name, const char *description);
	virtual ~LandscapeInclude();

	LandscapeEventList events;
	LandscapeMovementList movements;
	LandscapeSoundList sounds;
	LandscapeMusicList musics;
	PlacementTypeList placements;
	LandscapeOptions options;
	
	LansdscapeIncludeList includeList;
	std::list<LandscapeInclude *> includes;

	virtual bool readXML(XMLNode *parentNode);
protected:
	LandscapeDefinitions *definitions_;

private:
	LandscapeInclude(const LandscapeInclude &other);
	LandscapeInclude &operator=(LandscapeInclude &other);
};

class LandscapeIncludeFile : public LandscapeInclude 
{
public:
	LandscapeIncludeFile(LandscapeDefinitions *definitions);
	virtual ~LandscapeIncludeFile();
};

#endif // __INCLUDE_LandscapeIncludeh_INCLUDE__
