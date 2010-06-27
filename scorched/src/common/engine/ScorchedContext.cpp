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

#include <engine/ScorchedContext.h>
#include <engine/GameState.h>
#include <engine/ActionController.h>
#include <engine/Simulator.h>
#include <engine/ModFiles.h>
#include <net/NetInterface.h>
#include <coms/ComsMessageHandler.h>
#include <common/OptionsTransient.h>
#include <tank/TankContainer.h>
#include <tank/TankTeamScore.h>
#include <tank/TankModelStore.h>
#include <target/TargetSpace.h>
#include <movement/TargetMovement.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeDefinitions.h>
#include <weapons/AccessoryStore.h>
#include <lua/LUAScriptHook.h>

ScorchedContext::ScorchedContext(const char *name)
{
	accessoryStore = new AccessoryStore();
	targetContainer = new TargetContainer();
	tankContainer = new TankContainer(*targetContainer);
	landscapeMaps = new LandscapeMaps();
	comsMessageHandler = new ComsMessageHandler(name);
	netInterface = (NetInterface *) 0;
	optionsGame = new OptionsScorched();
	optionsTransient = new OptionsTransient(*optionsGame);
	modFiles = new ModFiles();
	landscapes = new LandscapeDefinitions();
	tankModelStore = new TankModelStore();
	tankTeamScore = new TankTeamScore();
	targetSpace = new TargetSpace();
	targetMovement = new TargetMovement();
	luaScriptFactory = new LUAScriptFactory();
	luaScriptHook = new LUAScriptHook(luaScriptFactory,
		name[0]=='S'?"server":"client",
		name[0]=='S'?S3D::getSettingsFile("serverhooks"):S3D::getSettingsFile("clienthooks"));

	getTargetSpace().setContext(this);
	luaScriptFactory->setContext(this);
}

ScorchedContext::~ScorchedContext()
{
}

ActionController &ScorchedContext::getActionController()
{
	return getSimulator().getActionController();
}
