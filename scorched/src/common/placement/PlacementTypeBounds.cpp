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

#include <placement/PlacementTypeBounds.h>
#include <landscapemap/LandscapeMaps.h>
#include <engine/ScorchedContext.h>
#include <common/ProgressCounter.h>
#include <common/RandomGenerator.h>
#include <common/Defines.h>
#include <XML/XMLParser.h>

PlacementTypeBounds::PlacementTypeBounds() : 
	PlacementType("PlacementTypeBounds",
		"Defines the placement of objects by randomly distributing them in a given bounding box"),
	onground("If objects should be aligned to the landscape surface", 0, false),
	count("The number of objects to place"),
	minbounds("The minimum extent of the placement bounding box"),
	maxbounds("The maximum extent of the placement bounding box")
{
	addChildXMLEntry("onground", &onground);
	addChildXMLEntry("count", &count);
	addChildXMLEntry("minbounds", &minbounds);
	addChildXMLEntry("maxbounds", &maxbounds);
}

PlacementTypeBounds::~PlacementTypeBounds()
{
}

bool PlacementTypeBounds::readXML(XMLNode *node)
{
	if (!PlacementType::readXML(node)) return false;

	if (maxbounds.getValue()[0] - minbounds.getValue()[0] < 25 ||
		maxbounds.getValue()[1] - minbounds.getValue()[1] < 25 ||
		maxbounds.getValue()[2] - minbounds.getValue()[2] < 10)
	{
		return node->returnError(
			"PlacementTypeBounds bounding box is too small, it must be at least 25x25x10 units");
	}

	return true;
}

void PlacementTypeBounds::getPositions(ScorchedContext &context,
	RandomGenerator &generator,
	std::list<Position> &returnPositions,
	ProgressCounter *counter)
{
	for (int i=0; i<count.getValue(); i++)
	{
		// Set the position, and make sure it is above ground
		Position position;
		do
		{
			position.position[0] = 
				generator.getRandFixed("PlacementTypeBounds") * (maxbounds.getValue()[0] - minbounds.getValue()[0]) + minbounds.getValue()[0];
			position.position[1] = 
				generator.getRandFixed("PlacementTypeBounds") * (maxbounds.getValue()[1] - minbounds.getValue()[1]) + minbounds.getValue()[1];

			fixed groundHeight = context.getLandscapeMaps().getGroundMaps().
				getInterpHeight(position.position[0], position.position[1]);

			if (onground.getValue()) 
			{
				position.position[2] = groundHeight;
			} 
			else
			{
				position.position[2] = 
					generator.getRandFixed("PlacementTypeBounds") * (maxbounds.getValue()[2] - minbounds.getValue()[2]) + minbounds.getValue()[2];
			}

			if (position.position[2] >= groundHeight) break;
		} 
		while (true);

		// Set velocity pointing into the middle
		FixedVector middle = (maxbounds.getValue() + minbounds.getValue()) / 2;
		position.velocity = middle - position.position;
		position.velocity.StoreNormalize();
		position.velocity[2] = 0;

		returnPositions.push_back(position);
	}
}
