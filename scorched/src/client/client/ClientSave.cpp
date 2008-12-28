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

#include <client/ClientSave.h>
#include <client/ScorchedClient.h>
#include <server/ScorchedServer.h>
#include <tankai/TankAIStore.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <tank/TankAvatar.h>
#include <tank/TankTeamScore.h>
#include <tank/TankModelContainer.h>
#include <net/NetBuffer.h>
#include <coms/ComsAddPlayerMessage.h>
#include <coms/ComsMessageSender.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <stdio.h>

static NetBuffer saveBuffer;
static bool stateRestoredFromFile = false;

bool ClientSave::storeClient()
{
	NetBuffer &buffer = saveBuffer;
	buffer.reset();

	// Add Version
	buffer.addToBuffer(S3D::ScorchedProtocolVersion);
	
	// GameState
	ScorchedServer::instance()->getOptionsGame().getMainOptions().writeToBuffer(buffer, true, false);
	
	// Transient State
	if (!ScorchedServer::instance()->getOptionsTransient().writeToBuffer(
		buffer)) return false;
	if (!ScorchedServer::instance()->getContext().getTankTeamScore().writeMessage(
		buffer)) return false;
	
	// No Players
	buffer.addToBuffer(ScorchedServer::instance()->getTankContainer().
		getNoOfNonSpectatorTanks());

	// Players
	std::map<unsigned int, Tank *> &tanks = 
		ScorchedServer::instance()->getTankContainer().
			getPlayingTanks();
 	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		// Add each tank
		Tank *tank = (*itor).second;

		// Remove the spectator tank
		if (!tank->getState().getSpectator())
		{
			// Add all other tanks
			buffer.addToBuffer(tank->getPlayerId());
			if (!tank->writeMessage(buffer, true)) return false;
			if (!tank->getAvatar().writeMessage(buffer)) return false;

			TankAI *tankAI = tank->getTankAI();
			if (tankAI) 
			{
				buffer.addToBuffer(tankAI->getName());
			}
			else
			{
				buffer.addToBuffer("Human");
			}
		}
	}

	return true;
}

bool ClientSave::saveClient(const std::string &fileName)
{
	FILE *file = fopen(fileName.c_str(), "wb");
	if (!file) return false;

	int size = fwrite(saveBuffer.getBuffer(),	
		sizeof(char),
		saveBuffer.getBufferUsed(),
		file);
	fclose(file);
	return (size == saveBuffer.getBufferUsed());
}

bool ClientSave::restoreClient(bool loadGameState, bool loadPlayers)
{
	NetBufferReader reader(saveBuffer);

	// Add Version
	std::string version;
	if (!reader.getFromBuffer(version)) return false;
	if (0 != strcmp(version.c_str(), S3D::ScorchedProtocolVersion.c_str()))
	{
		S3D::dialogMessage("LoadGame", 
			S3D::formatStringBuffer(
			"ERROR: Saved file version does not match game version.\n"
			"Saved version : %s\n"
			"Current version : %s\n",
			version.c_str(),
			S3D::ScorchedProtocolVersion.c_str()));
		return false;
	}
	
	// GameState
	if (loadGameState)
	{
		if (!ScorchedServer::instance()->getOptionsGame().getMainOptions().readFromBuffer(
			reader, true, false)) return false;
	}
	else
	{
		OptionsGame optionsGame;
		optionsGame.readFromBuffer(reader, true, false);
	}
	
	// Transient State
	if (!ScorchedServer::instance()->getOptionsTransient().readFromBuffer(
		reader)) return false;
	if (!ScorchedServer::instance()->getContext().getTankTeamScore().readMessage(
		reader)) return false;
	
	if (!loadPlayers) return true;

	bool specTanks = (ScorchedServer::instance()->getTankContainer().getNoOfTanks() -
		ScorchedServer::instance()->getTankContainer().getNoOfNonSpectatorTanks() > 1);

	// Players
	int noTanks = 0;
	if (!reader.getFromBuffer(noTanks)) return false;
	for (int i=0; i<noTanks; i++)
	{
		unsigned int playerId = 0;
		if (!reader.getFromBuffer(playerId)) return false;
	
		if (!specTanks)
		{
			Tank *tank = 
				ScorchedServer::instance()->getTankContainer().getTankById(playerId);
			if (tank)
			{
				if (!tank->readMessage(reader)) return false;
				if (!tank->getAvatar().readMessage(reader)) return false;
				std::string tankAIStr;
				if (!reader.getFromBuffer(tankAIStr)) return false;
			}
		}
		else
		{
			Vector color;
			Tank tank(
				ScorchedServer::instance()->getContext(),
				playerId, // PlayerId
				0, // DestinationId
				LangString(), // Name
				color,
				"",
				"");
			if (!tank.readMessage(reader)) return false;
			if (!tank.getAvatar().readMessage(reader)) return false;
		
			std::string tankAIStr;
			if (!reader.getFromBuffer(tankAIStr)) return false;
		
			if (!tank.getState().getSpectator())
			{
				ComsAddPlayerMessage message(
					tank.getPlayerId(),
					tank.getTargetName(),
					tank.getColor(),
					tank.getModelContainer().getTankModelName(),
					tank.getModelContainer().getTankTypeName(),
					ScorchedClient::instance()->getTankContainer().getCurrentDestinationId(),
					tank.getTeam(),
					tankAIStr.c_str());

				message.setPlayerIconName(tank.getAvatar().getName());
				message.getPlayerIcon().addDataToBuffer(
					tank.getAvatar().getFile().getBuffer(),
					tank.getAvatar().getFile().getBufferUsed());
				ComsMessageSender::sendToServer(message);
			}
		}
	}

	return true;
}

bool ClientSave::stateRestored()
{
	return stateRestoredFromFile;
}

void ClientSave::setStateNotRestored()
{
	stateRestoredFromFile = false;
}

bool ClientSave::loadClient(const std::string &fileName)
{
	FILE *file = fopen(fileName.c_str(), "rb");
	if (!file) 
	{
		Logger::log(S3D::formatStringBuffer("ERROR: File \"%s\" cannot be found.",
			fileName.c_str()));
		return false;
	}

	saveBuffer.reset();
	char buffer[2];
	while (0 != fread(buffer, 1, 1, file))
	{
		saveBuffer.addDataToBuffer(buffer, 1);
	}

	fclose(file);
	stateRestoredFromFile = true;
	return true;
}
