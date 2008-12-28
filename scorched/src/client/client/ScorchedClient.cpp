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

#include <client/ScorchedClient.h>
#include <engine/MainLoop.h>
#include <engine/GameState.h>
#include <graph/MainCamera.h>
#include <graph/ParticleEngine.h>
#include <graph/OptionsDisplay.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscape/GraphicalLandscapeMap.h>

ScorchedClient *ScorchedClient::instance_ = 0;

ScorchedClient *ScorchedClient::instance()
{
	if (!instance_)
	{
		instance_ = new ScorchedClient;
	}
	return instance_;
}

ScorchedClient::ScorchedClient() : 
	ScorchedContext("Client", false)
{
	mainLoop_ = new MainLoop();
	mainLoop_->clear();
	mainLoop_->addMainLoop(gameState);

	// Calculate how many particles we can see
	int numberOfBilboards = 6000;
	if (OptionsDisplay::instance()->getEffectsDetail() == 0) 
		numberOfBilboards = 100;
	else if (OptionsDisplay::instance()->getEffectsDetail() == 2) 
		numberOfBilboards = 10000;
	particleEngine_ = new ParticleEngine(
		&MainCamera::instance()->getCamera(), 
		numberOfBilboards);

	getLandscapeMaps().getGroundMaps().getHeightMap().setGraphicalMap(
		new GraphicalLandscapeMap());
}

ScorchedClient::~ScorchedClient()
{
}
