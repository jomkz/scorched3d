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

#if !defined(__INCLUDE_LandscapeDescriptionsBaseh_INCLUDE__)
#define __INCLUDE_LandscapeDescriptionsBaseh_INCLUDE__

#include <common/OptionsScorched.h>
#include <XML/XMLEntrySimpleTypes.h>

class LandscapeDescriptionsTexEntryList : public XMLEntryList<XMLEntryString>
{
public:
	LandscapeDescriptionsTexEntryList();
	virtual ~LandscapeDescriptionsTexEntryList();

	virtual XMLEntryString *createXMLEntry(void *xmlData);
};

class LandscapeDescriptionsDefnEntryList : public XMLEntryList<XMLEntryString>
{
public:
	LandscapeDescriptionsDefnEntryList();
	virtual ~LandscapeDescriptionsDefnEntryList();

	virtual XMLEntryString *createXMLEntry(void *xmlData);
};

class LandscapeDescriptionsEntry : public XMLEntryContainer
{
public:
	LandscapeDescriptionsEntry();
	virtual ~LandscapeDescriptionsEntry();

	XMLEntryString name; 
	XMLEntryFixed weight; // The posibility this defn will be choosen
	XMLEntryString description;  // Description of this landscape definition type
	XMLEntryString picture; // Visible view of this landscape definition type
	LandscapeDescriptionsTexEntryList texs;
	LandscapeDescriptionsDefnEntryList defns;
};

class LandscapeDescriptionsEntryList : public XMLEntryList<LandscapeDescriptionsEntry>
{
public:
	LandscapeDescriptionsEntryList();
	virtual ~LandscapeDescriptionsEntryList();

	virtual LandscapeDescriptionsEntry *createXMLEntry(void *xmlData);
};

class LandscapeDescriptionsBase : public XMLEntryRoot<XMLEntryContainer>
{
public:
	LandscapeDescriptionsBase();
	virtual ~LandscapeDescriptionsBase();

	virtual bool readLandscapeDescriptions();
	virtual void clearLandscapeDescriptions();

	bool landscapeEnabled(OptionsGame &context, const char *name);
	LandscapeDescriptionsEntry *getLandscapeByName(const char *name);
	std::list<LandscapeDescriptionsEntry*> &getAllLandscapes() 
		{ return entries_.getChildren(); }

protected:
	LandscapeDescriptionsEntryList entries_;
};

#endif // __INCLUDE_LandscapeDescriptionsBaseh_INCLUDE__
