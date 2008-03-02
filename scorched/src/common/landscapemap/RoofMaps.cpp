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

#include <landscapemap/RoofMaps.h>
#include <landscapemap/HeightMapLoader.h>
#include <common/ProgressCounter.h>
#include <engine/ScorchedContext.h>
#include <landscapedef/LandscapeDefinitionCache.h>
#include <landscapedef/LandscapeDefn.h>

RoofMaps::RoofMaps(LandscapeDefinitionCache &defnCache) :
	defnCache_(defnCache)
{
}

RoofMaps::~RoofMaps()
{
}

void RoofMaps::generateMaps(
	ScorchedContext &context,
	ProgressCounter *counter)
{
	generateRMap(context, counter);
}

fixed RoofMaps::getRoofHeight(int x, int y)
{
	if (defnCache_.getDefn()->roof->getType() != LandscapeDefnType::eRoofCavern)
	{
		return fixed::MAX_FIXED;
	}

	// Factors should be caclculated from hmap height/width
	int const xFactor = defnCache_.getDefn()->landscapewidth / rmap_.getMapWidth();
	int const yFactor = defnCache_.getDefn()->landscapeheight / rmap_.getMapHeight();

	x /= xFactor;
	y /= yFactor;
	if (x < 0 || y < 0)
	{
		return rmap_.getHeight(0, 0);
	}
	else if (x > rmap_.getMapWidth() || y > rmap_.getMapHeight())
	{
		return rmap_.getHeight(
			rmap_.getMapWidth(), rmap_.getMapHeight());
	}

	return rmap_.getHeight(x, y);
}

fixed RoofMaps::getInterpRoofHeight(fixed x, fixed y)
{
	if (defnCache_.getDefn()->roof->getType() != LandscapeDefnType::eRoofCavern)
	{
		return fixed::MAX_FIXED;
	}

	// Factors should be caclculated from hmap height/width
	int const xFactor = defnCache_.getDefn()->landscapewidth / rmap_.getMapWidth();
	int const yFactor = defnCache_.getDefn()->landscapeheight / rmap_.getMapHeight();

	x /= xFactor;
	y /= yFactor;
	if (x < 0 || y < 0 || x > rmap_.getMapWidth() || y > rmap_.getMapHeight())
	{
		return fixed::MAX_FIXED;
	}
	return rmap_.getInterpHeight(x, y);
}

void RoofMaps::generateRMap(
	ScorchedContext &context,
	ProgressCounter *counter)
{
	// calculate roof size and set it
	int mapWidth = defnCache_.getDefn()->landscapewidth / 4;
	int mapHeight = defnCache_.getDefn()->landscapewidth / 4;
	rmap_.create(mapWidth, mapHeight);

	// Generate the roof
	if (defnCache_.getDefn()->roof->getType() == LandscapeDefnType::eRoofCavern)
	{
		LandscapeDefnRoofCavern *cavern = 
			(LandscapeDefnRoofCavern *) defnCache_.getDefn()->roof;

		bool smooth = false;
		if (!HeightMapLoader::generateTerrain(
			defnCache_.getSeed() + 1,
			cavern->heightmap,
			rmap_,
			smooth,
			counter))
		{
			S3D::dialogExit("Landscape", "Failed to generate roof");
		}
		
		for (int j=0; j<=rmap_.getMapHeight(); j++)
		{
			for (int i=0; i<=rmap_.getMapWidth(); i++)
			{
				fixed height = rmap_.getHeight(i, j);
				height = fixed(cavern->height) - height;
				rmap_.setHeight(i, j, height);
				rmap_.getNormal(i, j)[2] = -rmap_.getNormal(i, j)[2];
			}
		}
	}
}
