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

#if !defined(__INCLUDE_LandscapeEventActionsh_INCLUDE__)
#define __INCLUDE_LandscapeEventActionsh_INCLUDE__

#include <XML/XMLEntrySimpleTypes.h>

class LandscapeEventAction : public XMLEntryContainer
{
public:
	LandscapeEventAction(const char *name, const char *description);
	virtual ~LandscapeEventAction();

	virtual void fireAction(ScorchedContext &context) = 0;
};

class LandscapeEventActionChoice : public XMLEntryTypeChoice<LandscapeEventAction>
{
public:
	LandscapeEventActionChoice(const char *name, const char *description);
	virtual ~LandscapeEventActionChoice();

	virtual LandscapeEventAction *createXMLEntry(const std::string &type);
};

class LandscapeEventActionFireWeapon : public LandscapeEventAction
{
public:
	LandscapeEventActionFireWeapon();
	virtual ~LandscapeEventActionFireWeapon();

	XMLEntryString weapon;

	virtual void fireAction(ScorchedContext &context);
};

class LandscapeEventActionFireWeaponFromGroup : public LandscapeEventAction
{
public:
	LandscapeEventActionFireWeaponFromGroup();
	virtual ~LandscapeEventActionFireWeaponFromGroup();

	XMLEntryString weapon;
	XMLEntryString groupname;

	virtual void fireAction(ScorchedContext &context);
};

#endif // __INCLUDE_LandscapeEventActionsh_INCLUDE__
