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

#if !defined(__INCLUDE_LandscapeDefinitionsh_INCLUDE__)
#define __INCLUDE_LandscapeDefinitionsh_INCLUDE__

#include <landscapedef/LandscapeDefinitionsBase.h>
#include <landscapedef/LandscapeDefinitionsItem.h>
#include <landscapedef/LandscapeDefinition.h>

class LandscapeDefinitions;
class LandscapeDefn;
class LandscapeTex;
class LandscapeInclude;
class OptionsGame;
class TankContainer;

class LandscapeDefinitions : public LandscapeDefinitionsBase
{
public:
	LandscapeDefinitions();
	virtual ~LandscapeDefinitions();

	virtual bool readLandscapeDefinitions();
	virtual void clearLandscapeDefinitions();

	void checkEnabled(OptionsScorched &context);
	LandscapeDefinition getBlankLandscapeDefn();
	LandscapeDefinition getLandscapeDefn(const char *name);
	LandscapeDefinition getRandomLandscapeDefn(OptionsScorched &context, TankContainer &tankContainer);
	LandscapeTex *getTex(const char *file, bool load = false);
	LandscapeDefn *getDefn(const char *file, bool load = false);
	LandscapeInclude *getInclude(const char *file, bool load = false);

protected:
	static unsigned int lastDefinitionNumber_;
	LandscapeDefinitionsEntry* lastDefinition_;
	LandscapeDefinitionsEntry blankDefinition_;
	std::map<std::string, int> usedFiles_;

	LandscapeDefinitionsItem<LandscapeTex> texs_;
	LandscapeDefinitionsItem<LandscapeDefn> defns_;
	LandscapeDefinitionsItem<LandscapeInclude> include_;

	const std::string getLeastUsedFile(
		OptionsScorched &context, std::vector<std::string> &files);
	LandscapeDefinitionsEntry *getRandomLandscapeDefnEntry(
		OptionsScorched &context,
		std::list<LandscapeDefinitionsEntry *> passedLandscapes);
	bool readLandscapeDefinitionsEntry(LandscapeDefinitionsEntry &entry);
};

#endif
