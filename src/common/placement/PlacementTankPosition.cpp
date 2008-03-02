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

#include <placement/PlacementTankPosition.h>
#include <landscapedef/LandscapeDefn.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapemap/DeformLandscape.h>
#include <tank/TankContainer.h>
#include <tank/TankPosition.h>
#include <tank/TankState.h>
#include <target/TargetLife.h>
#include <target/TargetSpace.h>
#include <target/TargetState.h>
#include <image/ImageFactory.h>
#include <common/Logger.h>

void PlacementTankPosition::flattenTankPositions(std::list<FixedVector> &tankPositions, 
	ScorchedContext &context)
{
	if (context.serverMode)
	{
		tankPositions.clear();

		LandscapeDefinitionCache &definitions = 
			context.landscapeMaps->getDefinitions();

		RandomGenerator generator;
		generator.seed(definitions.getSeed());

		std::map<unsigned int, Tank *> &tanks = 
			context.tankContainer->getPlayingTanks();
		std::map<unsigned int, Tank *>::iterator mainitor;
		for (mainitor = tanks.begin();
			mainitor != tanks.end();
			mainitor++)
		{
			Tank *tank = (*mainitor).second;

			if (!tank->getState().getSpectator() &&
				((tank->getState().getState() == TankState::sDead) ||
				tank->getState().getState() == TankState::sNormal))
			{
				FixedVector tankPos = PlacementTankPosition::placeTank(
					tank->getPlayerId(), tank->getTeam(), 
					context, generator);

				tank->getLife().setTargetPosition(tankPos);
				DeformLandscape::flattenArea(context, tankPos);

				tankPositions.push_back(tankPos);
			}
		}
	}
	else
	{
		std::list<FixedVector>::iterator itor;
		for (itor = tankPositions.begin();
			itor != tankPositions.end();
			itor++)
		{
			FixedVector &tankPos = *itor;
			DeformLandscape::flattenArea(context, tankPos);
		}
	}
}

static bool tankMaskCloseness(ScorchedContext &context, int team, 
	FixedVector &tankPos, Image *tankMask)
{
	// Find the mask position
	int maskX = (tankPos[0] * fixed(tankMask->getWidth())).asInt() / 
		context.landscapeMaps->getDefinitions().getDefn()->landscapewidth;
	int maskY = (tankPos[1] * fixed(tankMask->getHeight())).asInt() / 
		context.landscapeMaps->getDefinitions().getDefn()->landscapeheight;
	unsigned char *maskPos = tankMask->getBits() +
		maskX * 3 + maskY * tankMask->getWidth() * 3;
		
	if (maskPos[0] == 0 && maskPos[1] == 0 && maskPos[2] == 0)
	{
		// No tank is allowed on the black parts ot the mask
		// regardless of the team
		return false;
	}
	else if (maskPos[0] == 255 && maskPos[1] == 255 && maskPos[2] == 255)
	{
		// All tanks are allowed on the white parts of the mask
		// regardless of the team
		return true;
	}
	else if (team > 0)
	{
		// Check for team specific colors
		switch(team)
		{
		case 1:
			if (maskPos[0] != 255 || maskPos[1] != 0 || maskPos[2] != 0)
			{
				return false;
			}
			break;
		case 2:
			if (maskPos[0] != 0 || maskPos[1] != 0 || maskPos[2] != 255)
			{
				return false;
			}
			break;
		case 3:
			if (maskPos[0] != 0 || maskPos[1] != 255 || maskPos[2] != 0)
			{
				return false;
			}
			break;
		case 4:
			if (maskPos[0] != 255 || maskPos[1] != 255 || maskPos[2] != 0)
			{
				return false;
			}
			break;						
		}
	}
	return true;
}

static bool tankTargetCloseness(ScorchedContext &context, unsigned int playerId, 
	FixedVector &tankPos, fixed tankCloseness)
{
	// Make sure the tank is not too close to other targets
	std::map<unsigned int, Target *> targets;
	std::map<unsigned int, Target *>::iterator itor;
	context.targetSpace->getCollisionSet(tankPos, tankCloseness.asInt(), targets);
	for (itor = targets.begin();
		itor != targets.end();
		itor++)
	{
		Target *thisTarget = (*itor).second;
		if (!thisTarget->isTarget()) continue;

		fixed closeness = MAX(tankCloseness/2, thisTarget->getBorder());
		if ((tankPos - thisTarget->getLife().getTargetPosition()).Magnitude() < closeness) 
		{
			return false;
		}
	}

	// Make sure the tank is not too close to other tanks
	std::map<unsigned int, Tank *> tanks = context.tankContainer->getAllTanks();
	std::map<unsigned int, Tank *>::iterator tankItor;
	for (tankItor = tanks.begin();
		tankItor != tanks.end();
		tankItor++)
	{
		Tank *thisTank = (*tankItor).second;
		if (thisTank->getPlayerId() == playerId) continue;

		if (!thisTank->getState().getSpectator() &&
			((thisTank->getState().getState() == TankState::sDead) ||
			thisTank->getState().getState() == TankState::sNormal))
		{
			if ((tankPos - thisTank->getLife().getTargetPosition()).Magnitude() < tankCloseness) 
			{
				return false;
			}
		}
	}

	return true;
}

FixedVector PlacementTankPosition::placeTank(unsigned int playerId, int team,
	ScorchedContext &context, RandomGenerator &generator)
{
	FixedVector tankPos;
	const int tankBorder = 10;

	fixed minHeight = -1000;
	fixed maxHeight = 1000;
	fixed tankCloseness = 0;
	fixed flatness = 0;
	Image *tankMask = 0;

	LandscapeDefnType *defn =
		context.landscapeMaps->getDefinitions().getDefn()->tankstart;
	if (defn->getType() == LandscapeDefnType::eStartHeight)
	{
		LandscapeDefnStartHeight *height = 
			(LandscapeDefnStartHeight *) defn;
		minHeight = height->heightmin;
		maxHeight = height->heightmax;
		tankCloseness = height->startcloseness;
		flatness = height->flatness;
		if (!height->startmask.empty())
		{
			tankMask = ImageFactory::loadImage(
				S3D::getDataFile(height->startmask.c_str()));
			DIALOG_ASSERT(tankMask->getBits());
		}
	}
	else
	{
		S3D::dialogExit("ServerNewGameState",
			S3D::formatStringBuffer("Failed to find tank start type \"%i\"",
			defn->getType()));
	}
    
	int maxIt = 100;
	int i;
	for (i=maxIt; i>0; i--)
	{
		// Find a new position for the tank
		fixed posX = fixed (context.landscapeMaps->getDefinitions().
			getDefn()->landscapewidth - tankBorder * 2) * 
			generator.getRandFixed() + fixed(tankBorder);
		fixed posY = fixed (context.landscapeMaps->getDefinitions().
			getDefn()->landscapeheight - tankBorder * 2) * 
			generator.getRandFixed() + fixed(tankBorder);
		fixed height = context.landscapeMaps->getGroundMaps().
			getHeight(posX.asInt(), posY.asInt());
		FixedVector normal = context.landscapeMaps->getGroundMaps().
			getNormal(posX.asInt(), posY.asInt());
		tankPos = FixedVector(posX, posY, height);

		// Make sure not lower than water line
		if (tankPos[2] < minHeight || tankPos[2] > maxHeight) continue;

		// Make sure normal is less than given
		if (normal[2] < flatness) continue;

		// Make sure the mask allows the tank
		if (tankMask && 
			!tankMaskCloseness(context, team, tankPos, tankMask)) continue;

		// Check tanks are not too close to others or targets
		fixed closeness = (tankCloseness * fixed(i)) / fixed(maxIt);
		if (!tankTargetCloseness(context, playerId, tankPos, closeness)) continue;

		// Everything looks ok
		break;
	}
	if (i == 0) Logger::log("Tank closeness exceeded");

	delete tankMask;

	// Get the height of the tank
	tankPos[2] = context.landscapeMaps->getGroundMaps().getInterpHeight(
		tankPos[0], tankPos[1]);

	return tankPos;
}
