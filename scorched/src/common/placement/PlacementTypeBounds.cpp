////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
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
//    You should have received a copy of the GNU General Public License
//    along with Scorched3D; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

#include <placement/PlacementTypeBounds.h>
#include <landscapemap/LandscapeMaps.h>
#include <engine/ScorchedContext.h>
#include <common/ProgressCounter.h>
#include <common/RandomGenerator.h>
#include <common/Defines.h>
#include <XML/XMLParser.h>

PlacementTypeBounds::PlacementTypeBounds()
{
}

PlacementTypeBounds::~PlacementTypeBounds()
{
}

bool PlacementTypeBounds::readXML(XMLNode *node)
{
	if (!node->getNamedChild("count", count)) return false;
	if (!node->getNamedChild("minbounds", minbounds)) return false;
	if (!node->getNamedChild("maxbounds", maxbounds)) return false;

	if (maxbounds[0] - minbounds[0] < 25 ||
		maxbounds[1] - minbounds[1] < 25 ||
		maxbounds[2] - minbounds[2] < 10)
	{
		return node->returnError(
			"PlacementTypeBounds bounding box is too small, it must be at least 25x10 units");
	}

	return PlacementType::readXML(node);
}

void PlacementTypeBounds::getPositions(ScorchedContext &context,
	RandomGenerator &generator,
	std::list<Position> &returnPositions,
	ProgressCounter *counter)
{
	for (int i=0; i<count; i++)
	{
		// Set the position, and make sure it is above ground
		Position position;
		do
		{
			position.position[0] = 
				generator.getRandFixed() * (maxbounds[0] - minbounds[0]) + minbounds[0];
			position.position[1] = 
				generator.getRandFixed() * (maxbounds[1] - minbounds[1]) + minbounds[1];
			position.position[2] = 
				generator.getRandFixed() * (maxbounds[2] - minbounds[2]) + minbounds[2];
		} 
		while (context.getLandscapeMaps().getGroundMaps().
			getInterpHeight(position.position[0], position.position[1]) >= position.position[2]);

		// Set velocity pointing into the middle
		FixedVector middle = (maxbounds + minbounds) / 2;
		position.velocity = middle - position.position;
		position.velocity.StoreNormalize();
		position.velocity[2] = 0;

		returnPositions.push_back(position);
	}
}
