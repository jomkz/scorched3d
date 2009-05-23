////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2009
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

#include <server/ServerLoadLevel.h>
#include <server/ScorchedServer.h>
#include <landscapedef/LandscapeDefinition.h>
#include <landscapemap/LandscapeMaps.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <coms/ComsLoadLevelMessage.h>
#include <coms/ComsMessageSender.h>

ServerLoadLevel::ServerLoadLevel()
{
}

ServerLoadLevel::~ServerLoadLevel()
{
}

void ServerLoadLevel::destinationLoadLevel(unsigned int destinationId)
{
	LandscapeDefinition &landscapeDefinition =
		ScorchedServer::instance()->getLandscapeMaps().getDefinitions().
		getDefinition();

	// Set any tanks from this destination that they are loading the level
	// set the current version of the level (definition number) that they are loading
	// so we can check if the level changes before we have finished loading
	std::map<unsigned int, Tank *> &tanks = 
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		// For each tank
		Tank *tank = (*itor).second;
		if (destinationId == tank->getDestinationId())
		{
			tank->getState().setState(TankState::sLoadingLevel);
			tank->getState().setLevelNumber(landscapeDefinition.getDefinitionNumber());
		}
	}

	// Tell this destination to start loading the level
	ComsLoadLevelMessage loadLevelMessage(&landscapeDefinition);
	ComsMessageSender::sendToSingleClient(loadLevelMessage, destinationId);
}
