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

#if !defined(__INCLUDE_LandscapeMovementh_INCLUDE__)
#define __INCLUDE_LandscapeMovementh_INCLUDE__

#include <XML/XMLEntrySimpleTypes.h>

class LandscapeMovementTypeFactory : public XMLEntryTypeFactory
{
public:
	static LandscapeMovementTypeFactory *instance;

	// XMLEntryFactory
	virtual XMLEntry *createXMLEntry(const std::string &type);
};

class LandscapeMovementType : public XMLEntryNamedContainer
{
public:
	LandscapeMovementType(const std::string &name, const std::string &description);
	virtual ~LandscapeMovementType();

	XMLEntryString groupname;
};

class LandscapeMovementTypeBoids : public LandscapeMovementType
{
public:
	LandscapeMovementTypeBoids();
	virtual ~LandscapeMovementTypeBoids();

	ModelID model;
	XMLEntryFixedVector minbounds, maxbounds;
	XMLEntryFixed maxvelocity;
	XMLEntryFixed cruisedistance;
	XMLEntryFixed maxacceleration;
};

class LandscapeMovementTypeShips : public LandscapeMovementType
{
public:
	LandscapeMovementTypeShips();
	virtual ~LandscapeMovementTypeShips();

	XMLEntryFixed speed;
	XMLEntryInt controlpoints;
	XMLEntryFixed controlpointswidth;
	XMLEntryFixed controlpointsheight;
	XMLEntryFixed controlpointsrand;
	XMLEntryFixed starttime;
};

class LandscapeMovementTypeSpline : public LandscapeMovementType
{
public:
	LandscapeMovementTypeSpline();
	virtual ~LandscapeMovementTypeSpline();

	XMLEntryFixed speed;
	XMLEntryFixed starttime;
	XMLEntryBool groundonly;
	std::vector<FixedVector> points;
};


#endif // __INCLUDE_LandscapeMovementh_INCLUDE__
