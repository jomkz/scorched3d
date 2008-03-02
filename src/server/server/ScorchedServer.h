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

#if !defined(__INCLUDE_ScorchedServerh_INCLUDE__)
#define __INCLUDE_ScorchedServerh_INCLUDE__

#include <engine/ScorchedContext.h>

class TankDeadContainer;
class TankAIStore;
class ScorchedServer
{
public:
	static ScorchedServer *instance();

	AccessoryStore &getAccessoryStore() { return *context_.accessoryStore; }
	GameState &getGameState() { return *context_.gameState; }
	TankContainer &getTankContainer() { return *context_.tankContainer; }
	TargetContainer &getTargetContainer() { return *context_.targetContainer; }
	TargetMovement &getTargetMovement() { return *context_.targetMovement; }
	TankDeadContainer &getTankDeadContainer() { return *deadContainer_; }
	ActionController &getActionController() { return *context_.actionController; }
	LandscapeMaps &getLandscapeMaps() { return *context_.landscapeMaps; }
	ScorchedContext &getContext() { return context_; }
	NetInterface &getNetInterface() { return *context_.netInterface; }
	OptionsScorched &getOptionsGame() { return *context_.optionsGame; }
	OptionsTransient &getOptionsTransient() { return *context_.optionsTransient; }
	ComsMessageHandler &getComsMessageHandler() { return *context_.comsMessageHandler; }
	ModFiles &getModFiles() { return *context_.modFiles; }
	LandscapeDefinitions &getLandscapes() { return *context_.landscapes; }
	TankAIStore &getTankAIs() { return *tankAIStore_; }
	TankModelStore &getTankModels() { return *context_.tankModelStore; }
	LUAWrapper &getLUAWrapper() { return *context_.luaWrapper; }

protected:
	static ScorchedServer *instance_;
	ScorchedContext context_;
	TankDeadContainer *deadContainer_;
	TankAIStore *tankAIStore_;

private:
	ScorchedServer();
	virtual ~ScorchedServer();
};

#endif
