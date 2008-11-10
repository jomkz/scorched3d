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

#include <common/Logger.h>
#include <client/ClientScoreHandler.h>
#include <client/ClientState.h>
#include <client/ScorchedClient.h>
#include <client/ClientSaveScreenState.h>
#include <tank/TankContainer.h>
#include <tank/TankScore.h>
#include <coms/ComsScoreMessage.h>

ClientScoreHandler *ClientScoreHandler::instance_ = 0;

ClientScoreHandler *ClientScoreHandler::instance()
{
	if (!instance_)
	{
		instance_ = new ClientScoreHandler;
	}
	return instance_;
}

ClientScoreHandler::ClientScoreHandler() :
	finalScore_(false)
{
	ScorchedClient::instance()->getComsMessageHandler().addHandler(
		"ComsScoreMessage",
		this);
}

ClientScoreHandler::~ClientScoreHandler()
{
}

bool ClientScoreHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	ComsScoreMessage message;
	if (!message.readMessage(reader)) return false;

	finalScore_ = message.getFinalScore();
	if (finalScore_)
	{
		Logger::log("Final scores -------");
		std::map<unsigned int, Tank *> &tanks =
			ScorchedClient::instance()->getTankContainer().getPlayingTanks();
		std::map<unsigned int, Tank *>::iterator itor;
		for (itor = tanks.begin();
			itor != tanks.end();
			itor++)
		{
			Tank *tank = (*itor).second;
			Logger::log(S3D::formatStringBuffer("%s - %s",
				tank->getCStrName().c_str(),
				tank->getScore().getScoreString()));
		}
		Logger::log("--------------------");
	}

	ClientSaveScreenState::instance()->saveScreen();

	ScorchedClient::instance()->getGameState().stimulate(ClientState::StimWait);
	ScorchedClient::instance()->getGameState().checkStimulate();
	ScorchedClient::instance()->getGameState().stimulate(ClientState::StimScore);

	return true;
}
