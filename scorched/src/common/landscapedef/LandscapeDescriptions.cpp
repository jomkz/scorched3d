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

#include <landscapedef/LandscapeTex.h>
#include <landscapedef/LandscapeDefn.h>
#include <landscapedef/LandscapeInclude.h>
#include <landscapedef/LandscapeDescriptions.h>
#include <target/TargetContainer.h>
#include <common/OptionsScorched.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

unsigned int LandscapeDescriptions::lastDescriptionNumber_(0);

LandscapeDescriptions::LandscapeDescriptions() :
	lastDescription_(0)
{
}

LandscapeDescriptions::~LandscapeDescriptions()
{
	clearLandscapeDescriptions();
}

void LandscapeDescriptions::clearLandscapeDescriptions()
{
	LandscapeDescriptionsBase::clearLandscapeDescriptions();

	defns_.clearItems();
	texs_.clearItems();
	include_.clearItems();
}

LandscapeTex *LandscapeDescriptions::getTex(const char *file, bool load)
{
	return texs_.getItem(this, file, load);
}

LandscapeInclude *LandscapeDescriptions::getInclude(const char *file, bool load)
{
	return include_.getItem(this, file, load);
}

LandscapeDefn *LandscapeDescriptions::getDefn(const char *file, bool load)
{
	return defns_.getItem(this, file, load);
}

bool LandscapeDescriptions::readLandscapeDescriptionsEntry(LandscapeDescriptionsEntry &entry)
{
	std::list<XMLEntryString *>::iterator itor2;

	std::list<XMLEntryString *> &defns = entry.defns.getChildren();
	for (itor2 = defns.begin();
		itor2 != defns.end();
		++itor2)
	{
		const char *landscapeDefnFile = (*itor2)->getValue().c_str();
		LandscapeDefn *landscapeDefn = getDefn(landscapeDefnFile, true);
		if (!landscapeDefn) return false;
	}

	std::list<XMLEntryString *> &texs = entry.texs.getChildren();
	for (itor2 = texs.begin();
		itor2 != texs.end();
		++itor2)
	{
		const char *landscapeTexFile = (*itor2)->getValue().c_str();
		LandscapeTex *landscapeTex = getTex(landscapeTexFile, true);
		if (!landscapeTex) return false;
	}
	return true;
}

bool LandscapeDescriptions::readLandscapeDescriptions()
{
	// Clear existing landscapes
	clearLandscapeDescriptions();

	// Parse base landscape information
	if (!LandscapeDescriptionsBase::readLandscapeDescriptions()) return false;

	// Now check that the landscape tex and defn files parse correctly
	std::list<LandscapeDescriptionsEntry *>::iterator itor;
	for (itor = entries_.getChildren().begin();
		itor != entries_.getChildren().end();
		++itor)
	{
		LandscapeDescriptionsEntry *entry = (*itor);
		if (!readLandscapeDescriptionsEntry(*entry)) return false;
	}

	// Read blank landscape definiton
	{
		XMLFile file;
		if (!file.readFile(S3D::getModFile("data/landscapes/landscapeblank.xml"), true) ||
			!file.getRootNode())
		{
			S3D::dialogMessage("Scorched Landscape", S3D::formatStringBuffer(
						  "Failed to parse \"data/landscapes/landscapeblank.xml\"\n%s", 
						  file.getParserError()));
			return false;
		}
		if (!blankDescription_.readXML(file.getRootNode(), 0)) return false;
		if (!readLandscapeDescriptionsEntry(blankDescription_)) return false;
	}

	return true;
}

XMLEntryString *LandscapeDescriptions::getLeastUsedFile(
	OptionsScorched &context, std::list<XMLEntryString *> &files)
{
	DIALOG_ASSERT(!files.empty());

	std::vector<XMLEntryString *> results;
	int usedTimes = INT_MAX;

	std::list<XMLEntryString *>::iterator itor;
	for (itor = files.begin();
		itor != files.end();
		++itor)
	{
		XMLEntryString *file = (*itor);

		int used = 0;
		std::map<std::string, int>::iterator findItor = 
			usedFiles_.find(file->getValue());
		if (findItor != usedFiles_.end())
		{
			used = (*findItor).second;
		}

		if (used <= usedTimes)
		{
			if (used < usedTimes) results.clear();
			usedTimes = used;
			results.push_back(file);
		}
	}
	
	DIALOG_ASSERT(!results.empty());
	XMLEntryString *result = results.front();
	if (!context.getCycleMaps())
	{
		result = results[rand() % results.size()];
	}

	usedFiles_[result->getValue()] = usedTimes + 1;
	return result;
}

void LandscapeDescriptions::checkEnabled(OptionsScorched &context)
{
	std::list<LandscapeDescriptionsEntry *>::iterator itor;
	for (itor = entries_.getChildren().begin();
		itor != entries_.getChildren().end();
		++itor)
	{
		LandscapeDescriptionsEntry *result = *itor;
		if (landscapeEnabled(context.getMainOptions(), result->name.getValue().c_str()))
		{
			return;
		}
	}

	context.getChangedOptions().getLandscapesEntry().setValue("");
	Logger::log(S3D::formatStringBuffer(
		"Warning: No existing landscapes are enabled (Landscapes : %s)",
		context.getLandscapes()));
}

LandscapeDescription LandscapeDescriptions::getBlankLandscapeDescription()
{
	LandscapeDescriptionsEntry *result = &blankDescription_;

	// Return the chosen description
	XMLEntryString *tex = getFileAtPosition(result->texs.getChildren(), rand() % 
		result->texs.getChildren().size());
	XMLEntryString *defn = getFileAtPosition(result->defns.getChildren(), rand() % 
		result->defns.getChildren().size());
	unsigned int seed = (unsigned int) rand();

	LandscapeDescription entry(
		tex->getValue().c_str(), defn->getValue().c_str(), 
		seed, result->name.getValue().c_str(), 
		++lastDescriptionNumber_);
	return entry;
}

LandscapeDescription LandscapeDescriptions::getRandomLandscapeDescription(
	OptionsScorched &context, TargetContainer &TargetContainer)
{
	// Build a list of the maps that are enabled
	int players = TargetContainer.getNoOfNonSpectatorTanks();
	std::list<LandscapeDescriptionsEntry *> allPassedLandscapes;
	std::list<LandscapeDescriptionsEntry *> minMaxPassedLandscapes;
	std::list<LandscapeDescriptionsEntry *>::iterator itor;
	for (itor = entries_.getChildren().begin();
		itor != entries_.getChildren().end();
		++itor)
	{
		LandscapeDescriptionsEntry *current = *itor;
		if (landscapeEnabled(context.getMainOptions(), current->name.getValue().c_str()))
		{
			allPassedLandscapes.push_back(current);

			// Check that min/max players are ok
			std::list<XMLEntryString *>::iterator defnitor;
			for (defnitor = current->defns.getChildren().begin();
				defnitor != current->defns.getChildren().end();
				++defnitor)
			{
				LandscapeDefn *defn = getDefn((*defnitor)->getValue().c_str());
				DIALOG_ASSERT(defn);
				if (players >= defn->getMinPlayers() && players <= defn->getMaxPlayers())
				{
					minMaxPassedLandscapes.push_back(current);
					break;
				}
			}
		}
	}

	// Check we have a least one map
	DIALOG_ASSERT(!allPassedLandscapes.empty());

	// Try min max maps
	LandscapeDescriptionsEntry *result = getRandomLandscapeDescriptionEntry(
		context, minMaxPassedLandscapes);
	if (!result)
	{
		// Try all maps
		Logger::log("Warning: Cannot find any landscapes for number of players");
		result = getRandomLandscapeDescriptionEntry(context, allPassedLandscapes);
	}

	// Check we found map
	if (!result)
	{
		S3D::dialogExit("Scorched3D",
			"Failed to select a landscape description");
	}

	// Return the chosen description
	XMLEntryString *tex = getLeastUsedFile(context, result->texs.getChildren());
	XMLEntryString *defn = getLeastUsedFile(context, result->defns.getChildren());
	unsigned int seed = (unsigned int) rand();
	LandscapeDefn *landscapeDefn = getDefn(defn->getValue().c_str());
	DIALOG_ASSERT(landscapeDefn);
	if (landscapeDefn->getLandscapeGenerationSeed() != 0) 
		seed = landscapeDefn->getLandscapeGenerationSeed();

	LandscapeDescription entry(
		tex->getValue().c_str(), defn->getValue().c_str(), 
		seed, result->name.getValue().c_str(),
		++lastDescriptionNumber_);
	return entry;
}

XMLEntryString *LandscapeDescriptions::getFileAtPosition(
		std::list<XMLEntryString *> &files, int position)
{
	std::list<XMLEntryString *>::iterator itor = files.begin(),
		end = files.end();
	int pos = 0;
	for (;itor!=end;++itor, ++pos)
	{
		if (pos == position) return *itor;
	}
	return *files.begin();
}

LandscapeDescriptionsEntry *LandscapeDescriptions::getRandomLandscapeDescriptionEntry(
	OptionsScorched &context,
	std::list<LandscapeDescriptionsEntry *> passedLandscapes)
{
	// Map cycle mode
	LandscapeDescriptionsEntry *result = 0;
	if (context.getCycleMaps())
	{
		// Just cycle through the maps
		bool next = false;
		result = passedLandscapes.front();
		std::list<LandscapeDescriptionsEntry*>::iterator passedItor;
		for (passedItor = passedLandscapes.begin();
			passedItor != passedLandscapes.end();
			++passedItor)
		{
			LandscapeDescriptionsEntry *current = *passedItor;
			if (next) 
			{
				result = current;
				break;
			}
			if (current == lastDescription_) next = true;
		}
		lastDescription_ = result;
	}
	else
	{
		fixed totalWeight = 0;
		std::list<LandscapeDescriptionsEntry*>::iterator passedItor;
		for (passedItor = passedLandscapes.begin();
			passedItor != passedLandscapes.end();
			++passedItor)
		{
			LandscapeDescriptionsEntry *current = *passedItor;
			totalWeight += current->weight.getValue();
		}

		// Choose a map based on probablity
		fixed pos = fixed(true, rand() % fixed::FIXED_RESOLUTION) * totalWeight;
		fixed soFar = 0;
		for (passedItor = passedLandscapes.begin();
			passedItor != passedLandscapes.end();
			++passedItor)
		{
			LandscapeDescriptionsEntry *current = *passedItor;
			soFar += current->weight.getValue();

			if (pos <= soFar)
			{
				result = current;
				break;
			}
		}
	}

	return result;
}

