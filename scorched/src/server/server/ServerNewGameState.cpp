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

#include <set>
#include <server/ServerNewGameState.h>
#include <server/ScorchedServer.h>
#include <server/ServerMessageHandler.h>
#include <server/ServerChannelManager.h>
#include <server/TurnController.h>
#include <server/ServerCommon.h>
#ifndef S3D_SERVER
	#include <client/ClientSave.h>
#endif
#include <tankai/TankAIAdder.h>
#include <tank/TankContainer.h>
#include <tank/TankSort.h>
#include <tank/TankTeamScore.h>
#include <tank/TankDeadContainer.h>
#include <tank/TankModelStore.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <tank/TankPosition.h>
#include <tank/TankAccessories.h>
#include <tank/TankModelContainer.h>
#include <target/TargetLife.h>
#include <tankai/TankAI.h>
#include <weapons/EconomyStore.h>
#include <coms/ComsNewGameMessage.h>
#include <coms/ComsMessageSender.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeDefn.h>
#include <landscapedef/LandscapeDefinitions.h>
#include <placement/PlacementTankPosition.h>
#include <lua/LUAScriptHook.h>
#include <common/RandomGenerator.h>
#include <common/OptionsTransient.h>
#include <common/OptionsScorched.h>
#include <common/Clock.h>
#include <common/StatsLogger.h>
#include <common/Logger.h>
#include <common/Defines.h>
#include <algorithm>

extern Clock serverTimer;

ServerNewGameState::ServerNewGameState() :
	GameStateI("ServerNewGameState")
{
	ScorchedServer::instance()->getLUAScriptHook().addHookProvider("server_newgame");
}

ServerNewGameState::~ServerNewGameState()
{
}

void ServerNewGameState::enterState(const unsigned state)
{
	std::list<Tank *> currentTanks;
	std::map<unsigned int, Tank *> &playingTanks =
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank *>::iterator playingTanksItor;
	for (playingTanksItor = playingTanks.begin();
		playingTanksItor != playingTanks.end();
		playingTanksItor++)
	{
		Tank *tank = (*playingTanksItor).second;
		currentTanks.push_back(tank);
	}
	StatsLogger::instance()->gameStart(currentTanks);

	// Tell clients a new game is starting
	ServerChannelManager::instance()->sendText(
		ChannelText("info", "NEXT_ROUND", "Next Round"),
		true);

	// Make any enconomic changes
	EconomyStore::instance()->getEconomy()->calculatePrices();
	EconomyStore::instance()->getEconomy()->savePrices();

	// Make sure the most up-to-date options are used and sent to the client
	// The original options are sent in the connection accept message
	// but send new options if the server has changed the options
	bool sendGameState = false;
	if (ScorchedServer::instance()->getOptionsGame().commitChanges())
	{
		sendGameState = true;
		ServerChannelManager::instance()->sendText(
			ChannelText("info", 
				"GAME_OPTIONS_CHANGED", 
				"Game options have been changed!"),
			true);
	}

	// Get a landscape definition to use
	LandscapeDefinition defn = ScorchedServer::instance()->getLandscapes().getRandomLandscapeDefn(
		ScorchedServer::instance()->getContext().getOptionsGame(),
		ScorchedServer::instance()->getContext().getTankContainer());

	// Load the per level options
	ScorchedServer::instance()->getOptionsGame().updateLevelOptions(
		ScorchedServer::instance()->getContext(), defn);

	// Set all options (wind etc..)
	ScorchedServer::instance()->getContext().getOptionsTransient().newGame();
	ScorchedServer::instance()->getContext().getTankTeamScore().newGame();

	// Check if we can load/save a game
#ifndef S3D_SERVER
	//if (ClientParams::instance()->getConnectedToServer() == false)
	{
		if (ScorchedServer::instance()->getTankContainer().getNoOfTanks() == 0 ||
			ScorchedServer::instance()->getTankContainer().getNoOfTanks() -
			ScorchedServer::instance()->getTankContainer().getNoOfNonSpectatorTanks() > 1)
		{
			// We have not loaded players yet
			// this is the very first landscape
		}
		else
		{
			// Not the first landscape
			if (ClientSave::stateRestored())
			{
				ClientSave::restoreClient(false, true);
				ClientSave::setStateNotRestored();
			}
			else
			{
				ClientSave::storeClient();
			}
		}
	}
#endif

	// Setup landscape and tank start pos
	ServerCommon::serverLog( "Generating landscape");

	// Remove any old targets
	removeTargets();

	// Check that we dont have too many bots
	//checkBots(true);

	// Check teams are even
	//checkTeams();

	// Make sure all tanks that should be playing are playing
	resetTankStates(state);

	// Generate the new level
	ScorchedServer::instance()->getLandscapeMaps().generateMaps(
		ScorchedServer::instance()->getContext(), defn);

	// Add pending tanks (all tanks should be pending) into the game
	addTanksToGame(state, sendGameState);

	// Create the player order for this game
	TurnController::instance()->newGame();

	// Notify scripts of a new game starting
	ScorchedServer::instance()->getLUAScriptHook().callHook("server_newgame");

	// As we have not returned to the main loop for ages the
	// timer will have a lot of time in it
	// Get rid of this time so we don't screw things up
	serverTimer.getTimeDifference();

	// Move into the state that waits for players to become ready
	//ScorchedServer::instance()->getTankContainer().setAllNotReady();
	//ScorchedServer::instance()->getGameState().stimulate(ServerState::ServerStimulusNewGameReady);
}

int ServerNewGameState::addTanksToGame(const unsigned state,
									   bool addState)
{
	std::list<Tank *> tanks = resetTankStates(state);
	if (tanks.empty()) return 0;

	// Generate the level message
	ComsNewGameMessage newGameMessage;
	if (addState)
	{
		// Tell client(s) of game settings changes
		newGameMessage.addGameState(); 
	}

	// Add info about what targets are still alive
	std::map<unsigned int, Target *> &targets =
		ScorchedServer::instance()->getTargetContainer().getTargets();
	std::map<unsigned int, Target *>::iterator targetItor;
	for (targetItor = targets.begin();
		targetItor != targets.end();
		targetItor++)
	{
		Target *target = targetItor->second;
		if (target->getPlayerId() >= TargetID::MIN_TARGET_ID &&
			target->getPlayerId() < TargetID::MIN_TARGET_TRANSIENT_ID)
		{
			// Targets generated by the level code
			newGameMessage.getLevelMessage().getTargetIds().insert(
				target->getPlayerId());
			//if (state != ServerState::ServerStateNewGame &&
			//	target->isTarget())
			{
				newGameMessage.getLevelMessage().getOldTargets().addToBuffer(target->getPlayerId());
				target->writeMessage(newGameMessage.getLevelMessage().getOldTargets());				
			}
		} 
		else if (target->getPlayerId() >= TargetID::MIN_TARGET_TRANSIENT_ID &&
			target->getPlayerId() < TargetID::MAX_TARGET_ID)
		{
			// Targets generated during play
			newGameMessage.getLevelMessage().getNewTargets().addToBuffer(target->getPlayerId());
			target->writeMessage(newGameMessage.getLevelMessage().getNewTargets());
		}
	}

	// Add the height map info
	newGameMessage.getLevelMessage().createMessage(
		ScorchedServer::instance()->getLandscapeMaps().getDefinitions().getDefinition());
	newGameMessage.getLevelMessage().getDeformInfos() = 
		DeformLandscape::getInfos();
	LandscapeDefinitionCache &definitions =
		ScorchedServer::instance()->getLandscapeMaps().getDefinitions();
	ServerCommon::serverLog(
		S3D::formatStringBuffer("Finished generating landscape (%u, %s, %s)", 
		definitions.getSeed(), 
		definitions.getDefinition().getDefn(), 
		definitions.getDefinition().getTex()));

	// Check if the generated landscape is too large to send to the clients
	int sendSize = int(newGameMessage.getLevelMessage().getDeformInfos().size()) *
		sizeof(DeformLandscape::DeformInfo);
	if (sendSize > ScorchedServer::instance()->getOptionsGame().getMaxLandscapeSize())
	{
		ServerChannelManager::instance()->sendText(
			ChannelText("info", 
				"LANDSCAPE_TOO_LARGE",
				"Landscape too large to send to waiting clients ({0} bytes).", 
				sendSize),
			true);
		return 0;
	}

	// Used to ensure we only send messages to each
	// destination once
	std::list<unsigned int> sendDestinations;
	std::set<unsigned int> destinations;
	std::set<unsigned int>::iterator findItor;

	std::list<Tank *>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		Tank *tank = *itor;

		// We need to wait for a ready message
		//tank->getState().setNotReady();

		// Add to the list of destinations to send this message to
		// (if not already added)
		unsigned int destination = tank->getDestinationId();
		findItor = destinations.find(destination);
		if (findItor == destinations.end())
		{
			destinations.insert(destination);
			sendDestinations.push_back(destination);
		}
	}

	// Send after all of the states have been set
	// Do this after incase the message contains the new states
	ComsMessageSender::sendToMultipleClients(newGameMessage, sendDestinations);

	return (int) tanks.size();
}

std::list<Tank *> ServerNewGameState::resetTankStates(unsigned int state)
{
	std::list<Tank *> resultingTanks;

	// Tell any pending tanks to join the game
	std::map<unsigned int, Tank *> &tanks = 
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		Tank *tank = (*itor).second;
		// Check to see if any tanks are pending being added
		/*if (tank->getState().getState() == TankState::sPending ||
			(state == ServerState::ServerStateNewGame && (
			tank->getState().getState() == TankState::sDead ||
			tank->getState().getState() == TankState::sNormal)))
		{
			resultingTanks.push_back(tank);

			// This is the very first time this tank
			// has played the game, load it with the starting
			// weapons etc...
			if (tank->getState().getState() == TankState::sPending)
			{
				tank->newMatch();

				// Check if this is a bot
				// if not update from any residual tank we have.
				// Residual tanks are only available until the next
				// whole game starts.
				if (ScorchedServer::instance()->getTankDeadContainer().getTank(tank))
				{
					Logger::log( "Found residual player info");
				}
			}

			if (tank->getState().getState() == TankState::sSpectator)
			{
				// This tank is now playing (but dead)
				tank->getState().setState(TankState::sDead);
			}
			else if (state == ServerState::ServerStateNewGame)
			{
				// This tank is now playing
				tank->newGame();
			}
			else
			{
				// This tank is now playing (but dead)
				tank->getState().setState(TankState::sDead);
			}
		}*/
	}

	return resultingTanks;
}

void ServerNewGameState::removeTargets()
{
	std::map<unsigned int, Target *> targets = // Note copy
		ScorchedServer::instance()->getTargetContainer().getTargets();
	std::map<unsigned int, Target *>::iterator itor;
	for (itor = targets.begin();
		itor != targets.end();
		itor++)
	{
		unsigned int playerId = (*itor).first;
		Target *target = (*itor).second;
		if (target->isTemp())
		{
			if (target->isTarget())
			{
				Target *removedTarget = 
					ScorchedServer::instance()->getTargetContainer().removeTarget(playerId);
				delete removedTarget;
			}
			else
			{
				Tank *removedTank = 
					ScorchedServer::instance()->getTankContainer().removeTank(playerId);
				delete removedTank;
			}
		}
	}
}
