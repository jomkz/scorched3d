////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011
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

#include <placement/PlacementTankPosition.h>
#include <landscapedef/LandscapeDefn.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapemap/DeformLandscape.h>
#include <engine/Simulator.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <target/TargetLife.h>
#include <target/TargetSpace.h>
#include <target/TargetState.h>
#include <image/ImageFactory.h>
#include <common/Logger.h>
#include <common/OptionsScorched.h>

static bool tankMaskCloseness(ScorchedContext &context, int team, 
	FixedVector &tankPos, Image &tankMask)
{
	// Find the mask position
	int maskX = (tankPos[0] * fixed(tankMask.getWidth())).asInt() / 
		context.getLandscapeMaps().getDefinitions().getDefn()->getLandscapeWidth();
	int maskY = (tankPos[1] * fixed(tankMask.getHeight())).asInt() / 
		context.getLandscapeMaps().getDefinitions().getDefn()->getLandscapeHeight();
	unsigned char *maskPos = tankMask.getBits() +
		maskX * 3 + maskY * tankMask.getWidth() * 3;
		
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
	context.getTargetSpace().getCollisionSet(tankPos, tankCloseness.asInt(), targets);
	for (itor = targets.begin();
		itor != targets.end();
		++itor)
	{
		Target *thisTarget = (*itor).second;
		if (thisTarget->getType() == Target::TypeTank) continue;

		fixed closeness = MAX(tankCloseness/2, thisTarget->getBorder());
		if ((tankPos - thisTarget->getLife().getTargetPosition()).Magnitude() < closeness) 
		{
			if (context.getOptionsGame().getActionSyncCheck()) 
			{
				context.getSimulator().addSyncCheck(
					S3D::formatStringBuffer("Tank placement target close %u", 
					thisTarget->getPlayerId()));
			}
			return false;
		}
	}

	// Make sure the tank is not too close to other tanks
	std::map<unsigned int, Tank *> tanks = context.getTargetContainer().getTanks();
	std::map<unsigned int, Tank *>::iterator tankItor;
	for (tankItor = tanks.begin();
		tankItor != tanks.end();
		++tankItor)
	{
		Tank *thisTank = (*tankItor).second;
		if (thisTank->getPlayerId() == playerId) continue;

		if (context.getOptionsGame().getActionSyncCheck()) 
		{
			context.getSimulator().addSyncCheck(S3D::formatStringBuffer("Tank placement current : %u %s %s - %s", 
				thisTank->getPlayerId(), thisTank->getCStrName().c_str(), 
				thisTank->getState().getSmallStateString(),
				thisTank->getLife().getTargetPosition().asQuickString()));
		}

		if (thisTank->getState().getTankPlaying())
		{
			if ((tankPos - thisTank->getLife().getTargetPosition()).Magnitude() < tankCloseness) 
			{
				if (context.getOptionsGame().getActionSyncCheck()) 
				{
					context.getSimulator().addSyncCheck(
						S3D::formatStringBuffer("Tank placement tank close %u",
						thisTank->getPlayerId()));
				}

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
	Image tankMask;

	LandscapeDefnType *defn =
		context.getLandscapeMaps().getDefinitions().getDefn()->tankstart;
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
				S3D::eModLocation, height->startmask.c_str());
			DIALOG_ASSERT(tankMask.getBits());
		}
	}
	else
	{
		S3D::dialogExit("ServerNewGameState",
			S3D::formatStringBuffer("Failed to find tank start type \"%i\"",
			defn->getType()));
	}
    
	int arenaX = context.getLandscapeMaps().getGroundMaps().getArenaX();
	int arenaY = context.getLandscapeMaps().getGroundMaps().getArenaY();
	int arenaWidth = context.getLandscapeMaps().getGroundMaps().getArenaWidth();
	int arenaHeight = context.getLandscapeMaps().getGroundMaps().getArenaHeight();

	int maxIt = 100;
	int i;
	for (i=maxIt; i>0; i--)
	{
		// Find a new position for the tank
		fixed posX = fixed(arenaX) + (fixed(arenaWidth - tankBorder * 2) * 
			generator.getRandFixed("PlacementTankPosition")) + fixed(tankBorder);
		fixed posY = fixed(arenaY) + (fixed(arenaHeight - tankBorder * 2) * 
			generator.getRandFixed("PlacementTankPosition")) + fixed(tankBorder);
		fixed height = context.getLandscapeMaps().getGroundMaps().
			getHeight(posX.asInt(), posY.asInt());
		FixedVector normal = context.getLandscapeMaps().getGroundMaps().
			getNormal(posX.asInt(), posY.asInt());
		tankPos = FixedVector(posX, posY, height);

		// Make sure not lower than water line
		if (tankPos[2] < minHeight || tankPos[2] > maxHeight) 
		{
			if (context.getOptionsGame().getActionSyncCheck()) 
			{
				context.getSimulator().addSyncCheck(
					S3D::formatStringBuffer("Tank placement height %s %u", 
					tankPos.asQuickString(), minHeight.getInternalData()));
			}
			continue;
		}

		// Make sure normal is less than given
		if (normal[2] < flatness)
		{
			if (context.getOptionsGame().getActionSyncCheck()) 
			{
				context.getSimulator().addSyncCheck(
					S3D::formatStringBuffer("Tank placement flatness %s %u", 
					normal.asQuickString(), flatness.getInternalData()));
			}
			continue;
		}

		// Make sure the mask allows the tank
		if (tankMask.getBits() && 
			!tankMaskCloseness(context, team, tankPos, tankMask)) 
		{
			if (context.getOptionsGame().getActionSyncCheck()) 
			{
				context.getSimulator().addSyncCheck(
					S3D::formatStringBuffer("Tank placement mask %i %s", 
					team, tankPos.asQuickString()));
			}
			continue;
		}

		// Check tanks are not too close to others or targets
		fixed closeness = (tankCloseness * fixed(i)) / fixed(maxIt);
		if (!tankTargetCloseness(context, playerId, tankPos, closeness)) 
		{
			continue;
		}

		// Everything looks ok
		break;
	}
	if (i == 0) Logger::log("Tank closeness exceeded");

	// Get the height of the tank
	tankPos[2] = context.getLandscapeMaps().getGroundMaps().getInterpHeight(
		tankPos[0], tankPos[1]);

	// Log placement
	if (context.getOptionsGame().getActionSyncCheck()) 
	{
		context.getSimulator().addSyncCheck(
			S3D::formatStringBuffer("Tank placement %u %s",
				playerId, tankPos.asQuickString()));
	}

	return tankPos;
}
