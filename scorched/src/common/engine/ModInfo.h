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

#if !defined(__INCLUDE_ModInfoh_INCLUDE__)
#define __INCLUDE_ModInfoh_INCLUDE__

#include <XML/XMLEntryRoot.h>
#include <XML/XMLEntrySimpleTypes.h>

class ModInfoGame : public XMLEntryContainer
{
public:
	ModInfoGame();
	virtual ~ModInfoGame();

	XMLEntryFile icon_;
	XMLEntryFile shortdescription_;
	XMLEntryFile description_;
	XMLEntryFile gamefile_;
};

class ModInfoGameList : public XMLEntryList<ModInfoGame>
{
public:
	ModInfoGameList();
	virtual ~ModInfoGameList();

	virtual ModInfoGame *createXMLEntry(void *xmlData);
};

class ModInfoMain : public XMLEntryContainer
{
public:
	ModInfoMain();
	virtual ~ModInfoMain();

	XMLEntryString url_;
	XMLEntryFile icon_;
	XMLEntryString description_;
	XMLEntryString shortDescription_;
	XMLEntryString protocolversion_;
};

class ModInfoRoot : public XMLEntryRoot<XMLEntryContainer>
{
public:
	ModInfoRoot();
	virtual ~ModInfoRoot();

	ModInfoMain main_;
	ModInfoGameList games_;
};

class ModInfo
{
public:
	ModInfo(const std::string &name);
	virtual ~ModInfo();

	bool parse(const std::string &fileName);

	const char *getName() { return name_.c_str(); }
	const char *getUrl() { return modInfo_.main_.url_.getValue().c_str(); }
	const char *getIcon() { return modInfo_.main_.icon_.getValue().c_str(); }
	const char *getDescription() { return modInfo_.main_.description_.getValue().c_str(); }
	const char *getShortDescription() { return modInfo_.main_.shortDescription_.getValue().c_str(); }
	const char *getProtocolVersion() { return modInfo_.main_.protocolversion_.getValue().c_str(); }
	std::list<ModInfoGame *> &getMenuEntries() { return modInfo_.games_.getChildren(); }

protected:
	std::string name_;
	ModInfoRoot modInfo_;
};

#endif // __INCLUDE_ModInfoh_INCLUDE__
