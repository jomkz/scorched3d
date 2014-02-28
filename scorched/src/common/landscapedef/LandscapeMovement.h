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

#include <XML/XMLEntryComplexTypes.h>

class LandscapeMovement : public XMLEntryContainer
{
public:
	LandscapeMovement(const char *name, const char *description);
	virtual ~LandscapeMovement();

	XMLEntryString groupname;
};

class LandscapeMovementChoice : public XMLEntryTypeChoice<LandscapeMovement>
{
public:
	LandscapeMovementChoice();
	virtual ~LandscapeMovementChoice();

	virtual LandscapeMovement *createXMLEntry(const std::string &type);
	virtual void getAllTypes(std::set<std::string> &allTypes);
};

class LandscapeMovementList : public XMLEntryList<LandscapeMovementChoice>
{
public:
	LandscapeMovementList();
	virtual ~LandscapeMovementList();

	virtual LandscapeMovementChoice *createXMLEntry();
};

class LandscapeMovementBoids : public LandscapeMovement
{
public:
	LandscapeMovementBoids();
	virtual ~LandscapeMovementBoids();

	//XMLEntryModelID model;
	XMLEntryFixedVector minbounds, maxbounds;
	XMLEntryFixed maxvelocity;
	XMLEntryFixed cruisedistance;
	XMLEntryFixed maxacceleration;

	virtual bool readXML(XMLNode *parentNode, void *xmlData);
};

class LandscapeMovementShips : public LandscapeMovement
{
public:
	LandscapeMovementShips();
	virtual ~LandscapeMovementShips();

	XMLEntryFixed speed;
	XMLEntryInt controlpoints;
	XMLEntryFixed controlpointswidth;
	XMLEntryFixed controlpointsheight;
	XMLEntryFixed controlpointsrand;
	XMLEntryFixed starttime;
};

class LandscapeMovementSplineControlPoints : public XMLEntryList<XMLEntryFixedVector>
{
public:
	LandscapeMovementSplineControlPoints();
	virtual ~LandscapeMovementSplineControlPoints();

	virtual XMLEntryFixedVector *createXMLEntry();
};

class LandscapeMovementSpline : public LandscapeMovement
{
public:
	LandscapeMovementSpline();
	virtual ~LandscapeMovementSpline();

	XMLEntryFixed speed;
	XMLEntryFixed starttime;
	XMLEntryBool groundonly;
	LandscapeMovementSplineControlPoints points;
};


#endif // __INCLUDE_LandscapeMovementh_INCLUDE__
