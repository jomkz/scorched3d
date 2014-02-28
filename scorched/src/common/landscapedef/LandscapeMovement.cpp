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

LandscapeMovementTypeFactory *LandscapeMovementTypeFactory::instance(new LandscapeMovementTypeFactory());

XMLEntry *LandscapeMovementTypeFactory::createXMLEntry(const std::string &type)
{
	if (0 == strcmp(type.c_str(), "boids")) return new LandscapeMovementTypeBoids;
	if (0 == strcmp(type.c_str(), "ships")) return new LandscapeMovementTypeShips;
	if (0 == strcmp(type.c_str(), "spline")) return new LandscapeMovementTypeSpline;
	S3D::dialogMessage("LandscapeMovementType", S3D::formatStringBuffer("Unknown movement type %s", type));
	return 0;
}

LandscapeMovementType::LandscapeMovementType(const std::string &name, const std::string &description) :
	XMLEntryNamedContainer(name, description),
	groupname("groupname", "The name of the object group that contains the objects to move")
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

LandscapeMovementTypeSpline::LandscapeMovementTypeSpline() :
	LandscapeMovementType("LandscapeMovementTypeSpline", 
		"A movement type where objects are moved along a path defined by a series of control points joined by a spline curve."),
	speed("speed", "The speed that animation occurs around the points"),
	groundonly("groundonly", "If the height in the control points should be ignored and the curve should follow the landscape terrain height"),
	starttime("starttime", "How far around the circle this group of objects will start")
{
}

bool LandscapeMovementTypeSpline::readXML(XMLNode *node)
{
	if (!node->getNamedChild("speed", speed)) return false;
	if (!node->getNamedChild("starttime", starttime)) return false;
	if (!node->getNamedChild("groundonly", groundonly)) return false;
	FixedVector point;
	while (node->getNamedChild("controlpoint", point, false))
	{
		points.push_back(point);
	}

	if (points.size() < 3) return node->returnError("Must have at least 3 control points");

	return LandscapeMovementType::readXML(node);
}

bool LandscapeMovementTypeBoids::readXML(XMLNode *node)
{
	if (!node->getNamedChild("minbounds", minbounds)) return false;
	if (!node->getNamedChild("maxbounds", maxbounds)) return false;
	if (!node->getNamedChild("maxvelocity", maxvelocity)) return false;
	if (!node->getNamedChild("cruisedistance", cruisedistance)) return false;
	if (!node->getNamedChild("maxacceleration", maxacceleration)) return false;

	if (maxbounds[0] - minbounds[0] < fixed(25) ||
		maxbounds[1] - minbounds[1] < fixed(25) ||
		maxbounds[2] - minbounds[2] < fixed(10))
	{
		return node->returnError(
			"Boid bounding box is too small, it must be at least 25x10 units");
	}

	return LandscapeMovementType::readXML(node);
}
