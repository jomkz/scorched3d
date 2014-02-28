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

#include <landscapedef/LandscapeMovement.h>
#include <XML/XMLNode.h>
#include <common/Defines.h>

LandscapeMovementTypeChoice::LandscapeMovementTypeChoice() :
	XMLEntryTypeChoice<LandscapeMovementType>("movement", 
		"Associates object movement animation to a give group of objects")
{
}

LandscapeMovementTypeChoice::~LandscapeMovementTypeChoice()
{
}

LandscapeMovementType *LandscapeMovementTypeChoice::createXMLEntry(const std::string &type)
{
	if (0 == strcmp(type.c_str(), "boids")) return new LandscapeMovementTypeBoids;
	if (0 == strcmp(type.c_str(), "ships")) return new LandscapeMovementTypeShips;
	if (0 == strcmp(type.c_str(), "spline")) return new LandscapeMovementTypeSpline;
	S3D::dialogMessage("LandscapeMovementTypeChoice", S3D::formatStringBuffer("Unknown movement type %s", type));
	return 0;
}

LandscapeMovementTypeList::LandscapeMovementTypeList() :
	XMLEntryList("movement", 
		"A list of movement animations that can be associated with an object group")
{
}

LandscapeMovementTypeList::~LandscapeMovementTypeList()
{
}

LandscapeMovementTypeChoice *LandscapeMovementTypeList::createXMLEntry()
{
	return new LandscapeMovementTypeChoice();
}

LandscapeMovementType::LandscapeMovementType(const char *name, const char *description) :
	XMLEntryContainer(name, description),
	groupname("groupname", 
		"The name of the object group that contains the objects to move")
{
}

LandscapeMovementType::~LandscapeMovementType()
{
}

LandscapeMovementTypeShips::LandscapeMovementTypeShips() :
	LandscapeMovementType("LandscapeMovementTypeShips", 
		"A special movement type for ships, this movement type moves objects circumferentially around a spline curve defined by "
		"a series of random points around the landscape center."
		"All movement occurs on the same horizontal plane"),
	speed("speed", "The speed that animation occurs around the points"),
	controlpoints("controlpoints", "The number of random control points to generate"),
	controlpointswidth("controlpointswidth", "The width of the circle points will be generated on"),
	controlpointsheight("controlpointsheight", "The height of the circle points will be generated on"),
	controlpointsrand("controlpointsrand", "The maximum amount of randomness applied to the width and height for each point"),
	starttime("starttime", "How far around the circle this group of objects will start")
{
}

LandscapeMovementTypeShips::~LandscapeMovementTypeShips()
{
}

LandscapeMovementTypeSplineControlPoints::LandscapeMovementTypeSplineControlPoints() :
	XMLEntryList("controlpoint", "The list of control points that defined the spline curve")
{
}

LandscapeMovementTypeSplineControlPoints::~LandscapeMovementTypeSplineControlPoints()
{
}

XMLEntryFixedVector *LandscapeMovementTypeSplineControlPoints::createXMLEntry()
{
	return new XMLEntryFixedVector("controlpoint", "A control point for the spline curve");
}

LandscapeMovementTypeSpline::LandscapeMovementTypeSpline() :
	LandscapeMovementType("LandscapeMovementTypeSpline", 
		"A movement type where objects are moved along a path defined by a series of control points joined by a spline curve."),
	speed("speed", "The speed that animation occurs around the points"),
	groundonly("groundonly", "If the height in the control points should be ignored and the curve should follow the landscape terrain height"),
	starttime("starttime", "How far around the circle this group of objects will start"),
	points()
{
	addChildXMLEntry(&speed, &groundonly, &starttime);
}

LandscapeMovementTypeSpline::~LandscapeMovementTypeSpline()
{
}

bool LandscapeMovementTypeSpline::readXML(XMLNode *parentNode)
{
	if (!LandscapeMovementType::readXML(parentNode)) return false;

	if (points.getChildren().size() < 3) return parentNode->returnError("Must have at least 3 control points");
	return true;
}

LandscapeMovementTypeBoids::LandscapeMovementTypeBoids() :
	LandscapeMovementType("LandscapeMovementTypeBoids", 
		"A movement type where objects are moved due to a Boids simulation, i.e. in a flocking motion"),
	minbounds("minbounds", "The minimum extent that defines a bounding box tha will confine the boids"),
	maxbounds("maxbounds", "The maximum extent that defines a bounding box tha will confine the boids"),
	maxvelocity("maxvelocity", "The maximum velocity a boid can reach"),
	cruisedistance("cruisedistance", "The ideal distance between boids that will be strived for"),
	maxacceleration("maxacceleration", "The maximum acceleration a boid can reach")//,
	//model("", "")
{
	addChildXMLEntry(&minbounds, &maxbounds, &maxvelocity, &cruisedistance, &maxacceleration);
}

LandscapeMovementTypeBoids::~LandscapeMovementTypeBoids()
{
}

bool LandscapeMovementTypeBoids::readXML(XMLNode *parentNode)
{
	if (!LandscapeMovementType::readXML(parentNode)) return false;

	if (maxbounds.getValue()[0] - minbounds.getValue()[0] < fixed(25) ||
		maxbounds.getValue()[1] - minbounds.getValue()[1] < fixed(25) ||
		maxbounds.getValue()[2] - minbounds.getValue()[2] < fixed(10))
	{
		return parentNode->returnError(
			"Boid bounding box is too small, it must be at least 25x10 units");
	}

	return true;
}
