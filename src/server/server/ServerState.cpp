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

#include <engine/GameState.h>
#include <engine/ActionController.h>
#include <server/ServerState.h>
#include <server/ServerNewGameState.h>
#include <server/ServerNextRoundState.h>
#include <server/ServerNextTurnState.h>
#include <server/ServerNextShotState.h>
#include <server/ServerReadyState.h>
#include <server/ServerResetState.h>
#include <server/ServerTooFewPlayersStimulus.h>
#include <server/ServerStateTooFewPlayersState.h>
#include <server/ServerPlayingState.h>
#include <server/ServerStartingState.h>
#include <server/ServerShotState.h>
#include <server/ServerShotFinishedState.h>
#include <server/ScorchedServer.h>

void ServerState::setupStates(GameState &gameState)
{
	static class DummyLoop : public GameStateI
	{
	public:
		DummyLoop() : GameStateI("DummyLoop") {}
	} dummyLoop_;

	gameState.clear();

	// ServerStateTooFewPlayers (Start State)
	ServerStateTooFewPlayersState *serverTooFewPlayers = 
		new ServerStateTooFewPlayersState();
	gameState.addStateEntry(ServerStateTooFewPlayers,
		serverTooFewPlayers);
	gameState.addStateStimulus(ServerStateTooFewPlayers, 
		serverTooFewPlayers, ServerStateStarting);

	// ServerStateStarting
	ServerStartingState *serverStarting = 
		new ServerStartingState();
	gameState.addStateEntry(ServerStateStarting,
		serverStarting);
	gameState.addStateStimulus(ServerStateStarting, 
		ServerStimulusStarting, ServerStateStarting);	
	gameState.addStateStimulus(ServerStateStarting, 
		ServerTooFewPlayersStimulus::instance(), ServerStateTooFewPlayers);	
	gameState.addStateStimulus(ServerStateStarting, 
		serverStarting, ServerStateReset);

	// ServerStateReset 
	ServerResetState *serverReset = new ServerResetState();
	gameState.addStateEntry(ServerStateReset,
		serverReset);
	gameState.addStateStimulus(ServerStateReset, 
		ServerStimulusNewGame, ServerStateNewGame);

	// ServerStateNewGame
	ServerNewGameState *serverNewGame = new ServerNewGameState;
	gameState.addStateEntry(ServerStateNewGame,
		serverNewGame);
	gameState.addStateStimulus(ServerStateNewGame, 
		ServerStimulusNewGameReady, ServerStateNewGameReady);

	// ServerStateNewGameReady
	ServerReadyState *serverNewGameReady = new ServerReadyState();
#ifdef S3D_SERVER
	// Only required on the server.
	// For client games, there will only be one client waiting
	// so the server and client will start simulating in sync anyway
	// The server starts simulating after this state (and will exit this
	// state as soon as the only client is ready).
	gameState.addStateLoop(ServerStateNewGameReady, &dummyLoop_, 
		&ScorchedServer::instance()->getActionController());
#endif
	gameState.addStateEntry(ServerStateNewGameReady,
		serverNewGameReady);
	gameState.addStateStimulus(ServerStateNewGameReady, 
		serverNewGameReady, ServerStateNextRound);

	// ServerStateNextRound
	ServerNextRoundState *serverNextRound = new ServerNextRoundState();
	gameState.addStateLoop(ServerStateNextRound, &dummyLoop_, 
		&ScorchedServer::instance()->getActionController());
	gameState.addStateEntry(ServerStateNextRound,
		serverNextRound);
	gameState.addStateStimulus(ServerStateNextRound, 
		ServerStimulusNextShot, ServerStateNextShot);

	// ServerStateNextShot
	ServerNextShotState *serverNextShot = new ServerNextShotState;
	gameState.addStateLoop(ServerStateNextShot, &dummyLoop_, 
		&ScorchedServer::instance()->getActionController());
	gameState.addStateEntry(ServerStateNextShot,
		serverNextShot);
	gameState.addStateStimulus(ServerStateNextShot, 
		ServerStimulusNextTurn, ServerStateNextTurn);
	gameState.addStateStimulus(ServerStateNextShot,
		ServerStimulusNewGame, ServerStateNewGame);	
	gameState.addStateStimulus(ServerStateNextShot,
		ServerStimulusStarting, ServerStateStarting);	
	gameState.addStateStimulus(ServerStateNextShot, 
		ServerStimulusNextRound, ServerStateNextRound);

	// ServerStateNextTurn
	ServerNextTurnState *serverNextTurn = new ServerNextTurnState;
	gameState.addStateLoop(ServerStateNextTurn, &dummyLoop_, 
		&ScorchedServer::instance()->getActionController());
	gameState.addStateEntry(ServerStateNextTurn,
		serverNextTurn);
	gameState.addStateStimulus(ServerStateNextTurn,
		ServerStimulusBuying, ServerStateBuying);	
	gameState.addStateStimulus(ServerStateNextTurn,
		ServerStimulusPlaying, ServerStatePlaying);	
	gameState.addStateStimulus(ServerStateNextTurn,
		ServerStimulusShot, ServerStateShot);	

	// ServerStatePlaying
	ServerPlayingState *serverPlaying = new ServerPlayingState;
	gameState.addStateLoop(ServerStatePlaying, &dummyLoop_, 
		&ScorchedServer::instance()->getActionController());
	gameState.addStateEntry(ServerStatePlaying,
		serverPlaying);
	gameState.addStateStimulus(ServerStatePlaying,
		ServerTooFewPlayersStimulus::instance(), ServerStateShot);	
	gameState.addStateStimulus(ServerStatePlaying,
		serverPlaying, ServerStateNextTurn);

	// ServerStateBuying
	ServerPlayingState *serverBuying = new ServerPlayingState;
	gameState.addStateLoop(ServerStateBuying, &dummyLoop_, 
		&ScorchedServer::instance()->getActionController());
	gameState.addStateEntry(ServerStateBuying,
		serverBuying);
	gameState.addStateStimulus(ServerStateBuying,
		ServerTooFewPlayersStimulus::instance(), ServerStateShot);	
	gameState.addStateStimulus(ServerStateBuying,
		serverBuying, ServerStateNextTurn);

	// ServerStateShot
	ServerShotState *serverShot = new ServerShotState();
	gameState.addStateLoop(ServerStateShot, &dummyLoop_, 
		&ScorchedServer::instance()->getActionController());
	gameState.addStateEntry(ServerStateShot,
		serverShot);
	gameState.addStateStimulus(ServerStateShot,
		serverShot, ServerStateShotReady);

	// ServerStateShotReady
	ServerReadyState *serverShotReady = new ServerReadyState();
	gameState.addStateLoop(ServerStateShotReady, &dummyLoop_, 
		&ScorchedServer::instance()->getActionController());
	gameState.addStateEntry(ServerStateShotReady,
		serverShotReady);
	gameState.addStateStimulus(ServerStateShotReady, 
		serverShotReady, ServerStateShotFinished);

	// ServerStateShotFinished
	ServerShotFinishedState *serverShotFinished = new ServerShotFinishedState(serverShot);
	gameState.addStateLoop(ServerStateShotFinished, &dummyLoop_, 
		&ScorchedServer::instance()->getActionController());
	gameState.addStateEntry(ServerStateShotFinished,
		serverShotFinished);
	gameState.addStateStimulus(ServerStateShotFinished, 
		ServerStimulusTooFewPlayers, ServerStateTooFewPlayers);
	gameState.addStateStimulus(ServerStateShotFinished, 
		serverShotFinished, ServerStateNextShot);

	// Set the start state
	gameState.setState(ServerStateTooFewPlayers);
}
