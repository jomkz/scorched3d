////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
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

#include <landscapemap/DeformLandscape.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapedef/LandscapeDefinition.h>
#include <target/TargetContainer.h>
#include <target/TargetLife.h>
#include <target/TargetSpace.h>
#include <target/TargetState.h>
#include <engine/ScorchedContext.h>
#include <engine/ActionController.h>
#include <common/OptionsScorched.h>
#include <common/Defines.h>
#include <math.h>

bool DeformLandscape::deformLandscape(
	ScorchedContext &context,
	FixedVector &pos, fixed radius, bool down, DeformPoints &map)
{
	if (context.optionsGame->getActionSyncCheck())
	{
		context.actionController->addSyncCheck(
			S3D::formatStringBuffer("Deform : %i,%i,%i %i %s", 
				pos[0].getInternal(), pos[1].getInternal(), pos[2].getInternal(), 
				radius.getInternal(), (down?"Down":"Up")));
	}

	HeightMap &hmap = context.landscapeMaps->getGroundMaps().getHeightMap();

	bool hits = false;
	int iradius = (int) radius.asInt() + 1;
	if (iradius > 49) iradius = 49;

	fixed lowestHeight = fixed(0);
		//context.landscapeMaps->getDefinitions().getTex()->lowestlandheight;

	// Take out or add a chunk into the landsacpe
	for (int x=-iradius; x<=iradius; x++)
	{
		for (int y=-iradius; y<=iradius; y++)
		{
			DIALOG_ASSERT(x+iradius<100 && y+iradius<100);
			map.map[x+iradius][y+iradius] = fixed(-1);

			FixedVector newPos(pos[0] + x, pos[1] + y, pos[2]);
			if ((newPos[0] >= fixed(0)) && (newPos[0] < hmap.getMapWidth()) &&
				(newPos[1] >= fixed(0)) && (newPos[1] < hmap.getMapHeight()))
			{
				fixed dist = (pos - newPos).Magnitude();

				if (dist < radius)
				{
					fixed distToRadius = radius - dist;
					fixed currentHeight = hmap.getHeight(newPos[0].asInt(), newPos[1].asInt());
					fixed explosionDepth = ((distToRadius / radius) * fixed::XPIO2).sin() * radius;

					fixed newHeight = currentHeight;
					fixed newMap = fixed(-1);
					if (down)
					{
						if (currentHeight > newPos[2] - explosionDepth)
						{
							newMap = fixed(1) - (dist / radius);
							newMap *= fixed(3);
							if (newMap > fixed(1)) newMap = fixed(1);

							if (currentHeight > newPos[2] + explosionDepth)
							{
								newHeight -= explosionDepth + explosionDepth;
							}
							else
							{
								newHeight = newPos[2] - explosionDepth;
							}

							if (newHeight < lowestHeight)
							{
								if (currentHeight < lowestHeight)
								{
									newHeight = currentHeight;
								}
								else
								{
									newHeight = lowestHeight;
								}
							}
						}
					}
					else
					{
						newMap = fixed(1) - (dist / radius);
						newMap *= fixed(3);
						if (newMap > fixed(1)) newMap = fixed(1);

						if (currentHeight < newPos[2] + explosionDepth)
						{
							if (newPos[0] == 0 || newPos[1] == 0 ||
								newPos[0] == hmap.getMapWidth() -1 ||
								newPos[1] == hmap.getMapHeight() -1)
							{
							}
							else
							{
								newHeight = newPos[2] + explosionDepth;
							}
						}
					}

					if (newHeight != currentHeight)	hits = true;
					hmap.setHeight(newPos[0].asInt(), newPos[1].asInt(), newHeight);
					map.map[x+iradius][y+iradius] = newMap;
				}
			}
		}
	}

	return hits;
}

void DeformLandscape::flattenArea(
	ScorchedContext &context, FixedVector &tankPos, 
	bool removeObjects, fixed size)
{
	if (context.optionsGame->getActionSyncCheck())
	{
		context.actionController->addSyncCheck(
			S3D::formatStringBuffer("Flatten : %i,%i,%i %i", 
			tankPos[0].getInternal(), tankPos[1].getInternal(), tankPos[2].getInternal(), 
			size.getInternal()));
	}

	int iSize = size.asInt();
	HeightMap &hmap = context.landscapeMaps->getGroundMaps().getHeightMap();
	int posX = tankPos[0].asInt();
	int posY = tankPos[1].asInt();

	// Flatten a small area around the tank
	for (int x=-iSize; x<=iSize; x++)
	{
		for (int y=-iSize; y<=iSize; y++)
		{
			int ix = posX + x;
			int iy = posY + y;
			if (ix >= 0 && iy >= 0 &&
				ix < hmap.getMapWidth() &&
				iy < hmap.getMapHeight())
			{
				hmap.setHeight(ix, iy, tankPos[2]);
			}
		}
	}

	if (removeObjects)
	{
		// Remove any targets in this location
		std::map<unsigned int, Target *> collisionTargets;
		context.targetSpace->getCollisionSet(tankPos, size * fixed(true, 15000), collisionTargets);
		std::map<unsigned int, Target *>::iterator itor;
		for (itor = collisionTargets.begin();
			itor != collisionTargets.end();
			itor++)
		{
			Target *target = (*itor).second;
			if (target->isTarget() &&
				target->getTargetState().getFlattenDestroy())
			{
				Target *removedTarget = 
					context.targetContainer->removeTarget(target->getPlayerId());
				delete removedTarget;
			}
		}
	}
}

