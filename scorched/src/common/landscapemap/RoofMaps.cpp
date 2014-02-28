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

#include <landscapemap/RoofMaps.h>
#include <landscapemap/HeightMapLoader.h>
#include <image/ImageFactory.h>
#include <common/ProgressCounter.h>
#include <engine/ScorchedContext.h>
#include <landscapedef/LandscapeDefinitionCache.h>
#include <landscapedef/LandscapeDefn.h>

RoofMaps::RoofMaps(LandscapeDefinitionCache &defnCache) :
	defnCache_(defnCache), roofBaseHeight_(0)
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

bool RoofMaps::getRoofOn()
{
	return false;
}

fixed RoofMaps::getRoofHeight(int x, int y)
{
	if (!getRoofOn())
	{
		return fixed::MAX_FIXED;
	}

	if (x >= 0 && y >= 0 && x<=rmap_.getMapWidth() && y<=rmap_.getMapHeight())
	{
		return rmap_.getHeight(x, y);
	}
	return roofBaseHeight_;
}

fixed RoofMaps::getInterpRoofHeight(fixed x, fixed y)
{
	if (!getRoofOn())
	{
		return fixed::MAX_FIXED;
	}

	if (x >= 0 && y >= 0 && x<=rmap_.getMapWidth() && y<=rmap_.getMapHeight())
	{
		return rmap_.getInterpHeight(x, y);
	}
	return roofBaseHeight_;
}

void RoofMaps::generateRMap(
	ScorchedContext &context,
	ProgressCounter *counter)
{
	// calculate roof size and set it
	int mapWidth = defnCache_.getDefn()->getLandscapeWidth();
	int mapHeight = defnCache_.getDefn()->getLandscapeHeight();
	rmap_.create(mapWidth, mapHeight, true);
	deformRMap_.create(rmap_.getMapWidth(), rmap_.getMapHeight(), true);
	roofBaseHeight_ = 0;
}
