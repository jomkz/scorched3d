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

#if !defined(__INCLUDE_LandscapeDescriptionsh_INCLUDE__)
#define __INCLUDE_LandscapeDescriptionsh_INCLUDE__

#include <landscapedef/LandscapeDescriptionsBase.h>
#include <landscapedef/LandscapeDescriptionsItem.h>
#include <landscapedef/LandscapeDescription.h>
#include <landscapedef/LandscapeDefn.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapedef/LandscapeInclude.h>

class LandscapeDescriptions;
class OptionsGame;
class TargetContainer;

class LandscapeDescriptions : public LandscapeDescriptionsBase
{
public:
	LandscapeDescriptions();
	virtual ~LandscapeDescriptions();

	virtual bool readLandscapeDescriptions();
	virtual void clearLandscapeDescriptions();

	void checkEnabled(OptionsScorched &context);
	LandscapeDescription getBlankLandscapeDescription();
	LandscapeDescription getRandomLandscapeDescription(OptionsScorched &context, TargetContainer &TargetContainer);
	LandscapeTex *getTex(const char *file, bool load = false);
	LandscapeDefn *getDefn(const char *file, bool load = false);
	LandscapeInclude *getInclude(const char *file, bool load = false);

protected:
	static unsigned int lastDescriptionNumber_;
	LandscapeDescriptionsEntry* lastDescription_;
	LandscapeDescriptionsEntry blankDescription_;
	std::map<std::string, int> usedFiles_;

	LandscapeDescriptionsItem<LandscapeTextureFile> texs_;
	LandscapeDescriptionsItem<LandscapeDefinitionFile> defns_;
	LandscapeDescriptionsItem<LandscapeIncludeFile> include_;

	XMLEntryString *getLeastUsedFile(
		OptionsScorched &context, std::list<XMLEntryString *> &files);
	XMLEntryString *getFileAtPosition(
		std::list<XMLEntryString *> &files, int position);
	LandscapeDescriptionsEntry *getRandomLandscapeDescriptionEntry(
		OptionsScorched &context,
		std::list<LandscapeDescriptionsEntry *> passedLandscapes);
	bool readLandscapeDescriptionsEntry(LandscapeDescriptionsEntry &entry);
};

#endif
