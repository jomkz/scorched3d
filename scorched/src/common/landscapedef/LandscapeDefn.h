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

#if !defined(__INCLUDE_LandscapeDefnh_INCLUDE__)
#define __INCLUDE_LandscapeDefnh_INCLUDE__

#include <landscapedef/LandscapeInclude.h>
#include <landscapedef/LandscapeDefnDeform.h>
#include <landscapedef/LandscapeDefnHeightMap.h>
#include <landscapedef/LandscapeDefnTankStart.h>

class LandscapeDefn : public LandscapeInclude
{
public:
	LandscapeDefn(const char *name, const char *description, bool required);
	virtual ~LandscapeDefn();

	int getMinPlayers() { return minplayers.getValue(); }
	int getMaxPlayers() { return maxplayers.getValue(); }
	int getLandscapeWidth() { return landscapewidth.getValue(); }
	int getLandscapeHeight() { return landscapeheight.getValue(); }
	int getArenaWidth() { return arenawidth.getValue(); }
	int getArenaHeight() { return arenaheight.getValue(); }
	int getArenaX() { return arenax; }
	int getArenaY() { return arenay; }
	int getLandscapeGenerationSeed() { return landscapeGenerationSeed.getValue(); }

	LandscapeDefnTankStartChoice tankstart;
	LandscapeDefnHeightMapChoice heightmap;
	LandscapeDefnDeformChoice deform;

	virtual bool readXML(XMLNode *parentNode, void *xmlData);
protected:
	XMLEntryInt minplayers, maxplayers;
	XMLEntryInt landscapewidth, landscapeheight;
	XMLEntryInt arenawidth, arenaheight;
	XMLEntryInt landscapeGenerationSeed;
	int arenax, arenay;

private:
	LandscapeDefn(const LandscapeDefn &other);
	LandscapeDefn &operator=(LandscapeDefn &other);

};

class LandscapeDefinitionFile : public XMLEntryRoot<LandscapeDefn>
{
public:
	LandscapeDefinitionFile();
	virtual ~LandscapeDefinitionFile();
};

#endif
