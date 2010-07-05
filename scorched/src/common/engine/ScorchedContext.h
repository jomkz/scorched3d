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

#if !defined(__INCLUDE_ScorchedContexth_INCLUDE__)
#define __INCLUDE_ScorchedContexth_INCLUDE__

class AccessoryStore;
class ActionController;
class TargetSpace;
class TargetContainer;
class TargetMovement;
class LandscapeMaps;
class ComsMessageHandler;
class NetInterface;
class OptionsScorched;
class OptionsTransient;
class ModFiles;
class LandscapeDefinitions;
class TankContainer;
class TankModelStore;
class TankTeamScore;
class LUAScriptFactory;
class LUAScriptHook;
class Simulator;

class ScorchedContext
{
public:
	ScorchedContext(const char *name);
	virtual ~ScorchedContext();

	virtual bool getServerMode() = 0;

	void setNetInterface(NetInterface *i) { netInterface_ = i; }
	NetInterface &getNetInterface() { return *netInterface_; }
	bool getNetInterfaceValid() { return netInterface_ != 0; }

	virtual TargetSpace &getTargetSpace() = 0;
	virtual Simulator &getSimulator() = 0;
	AccessoryStore &getAccessoryStore() { return *accessoryStore_; }
	TankContainer &getTankContainer() { return *tankContainer_; }
	TargetContainer &getTargetContainer() { return *targetContainer_; }
	TargetMovement &getTargetMovement() { return *targetMovement_; }
	TankTeamScore &getTankTeamScore() { return *tankTeamScore_; }
	ActionController &getActionController();
	LandscapeMaps &getLandscapeMaps() { return *landscapeMaps_; }
	OptionsScorched &getOptionsGame() { return *optionsGame_; }
	OptionsTransient &getOptionsTransient() { return *optionsTransient_; }
	ComsMessageHandler &getComsMessageHandler() { return *comsMessageHandler_; }
	ModFiles &getModFiles() { return *modFiles_; }
	LandscapeDefinitions &getLandscapes() { return *landscapes_; }
	TankModelStore &getTankModels() { return *tankModelStore_; }
	LUAScriptFactory &getLUAScriptFactory() { return *luaScriptFactory_; }
	LUAScriptHook &getLUAScriptHook() { return *luaScriptHook_; }
	
protected:
	LandscapeMaps *landscapeMaps_;
	ComsMessageHandler *comsMessageHandler_;
	NetInterface *netInterface_;
	OptionsScorched *optionsGame_;
	OptionsTransient *optionsTransient_;
	ModFiles *modFiles_;
	AccessoryStore *accessoryStore_;
	LandscapeDefinitions *landscapes_;
	TargetContainer *targetContainer_;
	TankContainer *tankContainer_;
	TankModelStore *tankModelStore_;
	TankTeamScore *tankTeamScore_;
	TargetMovement *targetMovement_;
	LUAScriptFactory *luaScriptFactory_;
	LUAScriptHook *luaScriptHook_;
};

#endif
