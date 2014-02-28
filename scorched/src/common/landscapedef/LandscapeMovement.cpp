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

LandscapeMovementChoice::LandscapeMovementChoice() :
	XMLEntryTypeChoice<LandscapeMovement>("LandscapeMovementChoice", 
		"Associates object movement animation to a give group of objects")
{
}

LandscapeMovementChoice::~LandscapeMovementChoice()
{
}

LandscapeMovement *LandscapeMovementChoice::createXMLEntry(const std::string &type)
{
	if (0 == strcmp(type.c_str(), "boids")) return new LandscapeMovementBoids;
	if (0 == strcmp(type.c_str(), "ships")) return new LandscapeMovementShips;
	if (0 == strcmp(type.c_str(), "spline")) return new LandscapeMovementSpline;
	S3D::dialogMessage("LandscapeMovementChoice", S3D::formatStringBuffer("Unknown movement type %s", type));
	return 0;
}

LandscapeMovementList::LandscapeMovementList() :
	XMLEntryList("LandscapeMovementList", 
		"A list of movement animations that can be associated with an object group")
{
}

LandscapeMovementList::~LandscapeMovementList()
{
}

LandscapeMovementChoice *LandscapeMovementList::createXMLEntry()
{
	return new LandscapeMovementChoice();
}

LandscapeMovement::LandscapeMovement(const char *name, const char *description) :
	XMLEntryContainer(name, description),
	groupname("The name of the object group that contains the objects to move")
{
	addChildXMLEntry("groupname", &groupname);
}

LandscapeMovement::~LandscapeMovement()
{
}

LandscapeMovementShips::LandscapeMovementShips() :
	LandscapeMovement("LandscapeMovementShips", 
		"A special movement type for ships, this movement type moves objects circumferentially around a spline curve defined by "
		"a series of random points around the landscape center."
		"All movement occurs on the same horizontal plane"),
	speed("The speed that animation occurs around the points"),
	controlpoints("The number of random control points to generate"),
	controlpointswidth("The width of the circle points will be generated on"),
	controlpointsheight("The height of the circle points will be generated on"),
	controlpointsrand("The maximum amount of randomness applied to the width and height for each point"),
	starttime("How far around the circle this group of objects will start")
{
	addChildXMLEntry("speed", &speed, "starttime", &starttime);
	addChildXMLEntry("controlpoints", &controlpoints, "controlpointswidth", &controlpointswidth, 
		"controlpointsheight", &controlpointsheight, "controlpointsrand", &controlpointsrand);
}

LandscapeMovementShips::~LandscapeMovementShips()
{
}

LandscapeMovementSplineControlPoints::LandscapeMovementSplineControlPoints() :
	XMLEntryList("LandscapeMovementSplineControlPoints", "The list of control points that defined the spline curve")
{
}

LandscapeMovementSplineControlPoints::~LandscapeMovementSplineControlPoints()
{
}

XMLEntryFixedVector *LandscapeMovementSplineControlPoints::createXMLEntry()
{
	return new XMLEntryFixedVector("A control point for the spline curve");
}

LandscapeMovementSpline::LandscapeMovementSpline() :
	LandscapeMovement("LandscapeMovementSpline", 
		"A movement type where objects are moved along a path defined by a series of control points joined by a spline curve."),
	speed("The speed that animation occurs around the points"),
	groundonly("If the height in the control points should be ignored and the curve should follow the landscape terrain height"),
	starttime("How far around the circle this group of objects will start"),
	points()
{
	addChildXMLEntry("speed", &speed, "starttime", &starttime, "groundonly", &groundonly, "controlpoint", &points);
}

LandscapeMovementSpline::~LandscapeMovementSpline()
{
}

bool LandscapeMovementSpline::readXML(XMLNode *parentNode)
{
	if (!LandscapeMovement::readXML(parentNode)) return false;

	if (points.getChildren().size() < 3) return parentNode->returnError("Must have at least 3 control points");
	return true;
}

LandscapeMovementBoids::LandscapeMovementBoids() :
	LandscapeMovement("LandscapeMovementBoids", 
		"A movement type where objects are moved due to a Boids simulation, i.e. in a flocking motion"),
	minbounds("The minimum extent that defines a bounding box tha will confine the boids"),
	maxbounds("The maximum extent that defines a bounding box tha will confine the boids"),
	maxvelocity("The maximum velocity a boid can reach"),
	cruisedistance("The ideal distance between boids that will be strived for"),
	maxacceleration("The maximum acceleration a boid can reach")//,
	//model("", "")
{
	addChildXMLEntry("minbounds", &minbounds, "maxbounds", &maxbounds, "maxvelocity", &maxvelocity, 
		"cruisedistance", &cruisedistance, "maxacceleration", &maxacceleration);
}

LandscapeMovementBoids::~LandscapeMovementBoids()
{
}

bool LandscapeMovementBoids::readXML(XMLNode *parentNode)
{
	if (!LandscapeMovement::readXML(parentNode)) return false;

	if (maxbounds.getValue()[0] - minbounds.getValue()[0] < fixed(25) ||
		maxbounds.getValue()[1] - minbounds.getValue()[1] < fixed(25) ||
		maxbounds.getValue()[2] - minbounds.getValue()[2] < fixed(10))
	{
		return parentNode->returnError(
			"Boid bounding box is too small, it must be at least 25x10 units");
	}

	return true;
}
