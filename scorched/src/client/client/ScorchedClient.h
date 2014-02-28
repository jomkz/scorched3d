////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2013
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

#if !defined(__INCLUDE_ScorchedClienth_INCLUDE__)
#define __INCLUDE_ScorchedClienth_INCLUDE__

#include <engine/ScorchedContext.h>
#include <common/fixed.h>

class ComsMessageHandlerI;
class ParticleEngine;
class SimulatorGameState;
class ThreadCallback;
class ClientUISync;
class ClientUISyncExternal;
class ClientState;
class ClientSimulator;
class ClientHandlers;
class ClientMessageHandler;
class ClientChannelManager;
class ClientAdmin;
class Console;
class ProgressCounter;
class ScorchedClient : public ScorchedContext
{
public:
	static ScorchedClient *instance();
	static void startClientForDebug();

	static void startClient(ProgressCounter *counter);
	static void stopClient();

	virtual bool getServerMode() { return false; }

	virtual Simulator &getSimulator();
	virtual TargetSpace &getTargetSpace() { return *targetSpace_; }
	ScorchedContext &getContext() { return *this; }
	ParticleEngine &getParticleEngine() { return *particleEngine_; }
	ClientState &getClientState() { return *clientState_; }
	ClientSimulator &getClientSimulator() { return *clientSimulator_; }
	ClientHandlers &getClientHandlers() { return *clientHandlers_; }
	ClientChannelManager &getClientChannelManager() { return *channelManager_; }
	Console &getConsole() { return *console_; }
	ClientUISync &getClientUISync() { return *clientUISync_; }
	ClientAdmin &getClientAdmin() { return *clientAdmin_; }
	static ThreadCallback &getClientThreadCallback() { return *threadCallback_; }
	static ClientUISyncExternal &getClientUISyncExternal() { return *clientUISyncExternal_; }

protected:
	static ScorchedClient *instance_;
	static TargetSpace *targetSpace_;
	static ThreadCallback *threadCallback_;
	static ClientUISyncExternal *clientUISyncExternal_;
	ClientUISync *clientUISync_;
	ParticleEngine* particleEngine_;
	ClientAdmin *clientAdmin_;
	ClientState *clientState_;
	ClientSimulator *clientSimulator_;
	ClientHandlers *clientHandlers_;
	ClientMessageHandler *clientMessageHandler_;
	ClientChannelManager *channelManager_;
	Console *console_;

	ComsMessageHandlerI *processComsSimulateMessageAdapter_;
	ComsMessageHandlerI *processNetStatMessageAdapter_;

	static void startClientInternalStatic(ScorchedClient *instance, ProgressCounter *counter);
	void startClientInternal(ProgressCounter *counter);
	bool clientLoop();

	void serverStartedServerThread();
	void serverStartedClientThread();

private:
	ScorchedClient();
	virtual ~ScorchedClient();
};

#endif
