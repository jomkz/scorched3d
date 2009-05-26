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

#include <server/ServerAdminCommon.h>
#include <server/ServerCommon.h>
#include <server/ScorchedServer.h>
#include <server/ScorchedServerUtil.h>
#include <server/ServerChannelManager.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <tankai/TankAIAdder.h>
#include <target/TargetLife.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <common/FileLogger.h>

static FileLogger *serverAdminFileLogger = 0;

static void adminLog(const ChannelText &message)
{
	if (!serverAdminFileLogger) 
	{
		char buffer[256];
		snprintf(buffer, 256, "ServerAdminLog-%i-", 
			ScorchedServer::instance()->getOptionsGame().getPortNo());
		serverAdminFileLogger = new FileLogger(buffer);
	}	

	ServerChannelManager::instance()->sendText(message, true);

	LangString str = ((ChannelText &)message).getMessage();
	LoggerInfo info(LangStringUtil::convertFromLang(str));
	info.setTime();
	serverAdminFileLogger->logMessage(info);
}

static void internalBanPlayer(ServerAdminSessions::Credential &credential,  
	unsigned int playerId,
	ServerBanned::BannedType type, const char *reason)
{
	Tank *tank = ScorchedServer::instance()->
		getTankContainer().getTankById(playerId);
	if (tank)
	{
		if (tank->getDestinationId() == 0)
		{
			return;
		}
		unsigned int ipAddress = tank->getIpAddress();
		if (ipAddress != 0)
		{	
			ScorchedServerUtil::instance()->bannedPlayers.
				addBanned(ipAddress, tank->getTargetName(), 
					tank->getUniqueId(), tank->getSUI(), 
					type, credential.username.c_str(), reason);
			if (type == ServerBanned::Banned)
			{
				ServerCommon::kickPlayer(playerId);

				ServerAuthHandler *authHandler =
					ScorchedServerUtil::instance()->getAuthHandler();
				if (authHandler)
				{
					authHandler->banUser(tank->getUniqueId());
				}
			}
		}
	}
}

bool ServerAdminCommon::addPlayer(ServerAdminSessions::Credential &credential, const char *playerType)
{
	if (!credential.hasPermission(ServerAdminSessions::PERMISSION_ADDPLAYER)) return false;
	TankAIAdder::addTankAI(*ScorchedServer::instance(), playerType);
	return true;
}

bool ServerAdminCommon::kickPlayer(ServerAdminSessions::Credential &credential, unsigned int playerId)
{
	if (!credential.hasPermission(ServerAdminSessions::PERMISSION_KICKPLAYER)) return false;

	Tank *targetTank = ScorchedServer::instance()->
		getTankContainer().getTankById(playerId);
	if (!targetTank) return false;

	adminLog(ChannelText("info",
		"ADMIN_KICK",
		"\"{0}\" admin kick \"{1}\"",
		credential.username,
		targetTank->getTargetName()));
	ServerCommon::kickPlayer(
		targetTank->getPlayerId());

	return true;
}

bool ServerAdminCommon::poorPlayer(ServerAdminSessions::Credential &credential, unsigned int playerId)
{
	Tank *targetTank = ScorchedServer::instance()->
		getTankContainer().getTankById(playerId);
	if (!targetTank) return false;

	adminLog(ChannelText("info",
		"ADMIN_POOR",
		"\"{0}\" admin poor \"{1}\"",
		credential.username,
		targetTank->getTargetName()));
	targetTank->getScore().setMoney(0);

	return true;
}

bool ServerAdminCommon::banPlayer(ServerAdminSessions::Credential &credential, unsigned int playerId, const char *reason)
{
	if (!credential.hasPermission(ServerAdminSessions::PERMISSION_BANPLAYER)) return false;

	Tank *targetTank = ScorchedServer::instance()->
		getTankContainer().getTankById(playerId);
	if (!targetTank) return false;

	adminLog(ChannelText("info",
		"ADMIN_BAN",
		"\"{0}\" admin ban \"{1}\"",
		credential.username,
		targetTank->getTargetName()));
	internalBanPlayer(
		credential,
		targetTank->getPlayerId(), 
		ServerBanned::Banned, 
		reason);

	return true;
}

bool ServerAdminCommon::slapPlayer(ServerAdminSessions::Credential &credential, unsigned int playerId, float slap)
{
	Tank *targetTank = ScorchedServer::instance()->
		getTankContainer().getTankById(playerId);
	if (!targetTank) return false;

	adminLog(ChannelText("info",
		"ADMIN_SLAP",
		"\"{0}\" admin slap \"{1}\" {2}",
		credential.username,
		targetTank->getTargetName(),
		slap));
	targetTank->getLife().setLife(
		targetTank->getLife().getLife() - fixed(int(slap)));

	return true;
}

bool ServerAdminCommon::flagPlayer(ServerAdminSessions::Credential &credential, unsigned int playerId, const char *reason)
{
	Tank *targetTank = ScorchedServer::instance()->
		getTankContainer().getTankById(playerId);
	if (!targetTank) return false;

	adminLog(ChannelText("info",
		"ADMIN_FLAG",
		"\"{0}\" admin flag \"{1}\"",
		credential.username,
		targetTank->getTargetName()));
	internalBanPlayer(
		credential,
		targetTank->getPlayerId(), 
		ServerBanned::Flagged, 
		reason);

	return true;
}

bool ServerAdminCommon::mutePlayer(ServerAdminSessions::Credential &credential, unsigned int playerId, bool mute)
{
	Tank *targetTank = ScorchedServer::instance()->
		getTankContainer().getTankById(playerId);
	if (!targetTank) return false;

	adminLog(ChannelText("info",
		mute?"ADMIN_MUTE":"ADMIN_UNMUTE",
		mute?"\"{0}\" admin mute \"{1}\"":"\"{0}\" admin unmute \"{1}\"",
		credential.username,
		targetTank->getTargetName()));
	targetTank->getState().setMuted(mute); 

	return true;
}

bool ServerAdminCommon::permMutePlayer(ServerAdminSessions::Credential &credential, unsigned int playerId, const char *reason)
{
	Tank *targetTank = ScorchedServer::instance()->
		getTankContainer().getTankById(playerId);
	if (!targetTank) return false;

	adminLog(ChannelText("info",
		"ADMIN_PERMMUTE",
		"\"{0}\" admin permmute \"{1}\"",
		credential.username,
		targetTank->getTargetName()));
	internalBanPlayer(
		credential,
		targetTank->getPlayerId(),
		ServerBanned::Muted, 
		reason);
	targetTank->getState().setMuted(true);

	return true;
}

bool ServerAdminCommon::unpermMutePlayer(ServerAdminSessions::Credential &credential, unsigned int playerId)
{
	Tank *targetTank = ScorchedServer::instance()->
		getTankContainer().getTankById(playerId);
	if (!targetTank) return false;

	adminLog(ChannelText("info",
		"ADMIN_UNPERMMUTE",
		"\"{0}\" admin unpermmute \"{1}\"",
		credential.username,
		targetTank->getTargetName()));
	internalBanPlayer(
		credential,
		targetTank->getPlayerId(),
		ServerBanned::NotBanned, 
		"");
	targetTank->getState().setMuted(false);

	return true;
}

bool ServerAdminCommon::newGame(ServerAdminSessions::Credential &credential)
{
	if (!credential.hasPermission(
		ServerAdminSessions::PERMISSION_ALTERGAME)) return false;

	adminLog(ChannelText("info",
		"ADMIN_NEW_GAME",
		"\"{0}\" admin new game",
		credential.username));
	ServerCommon::killAll();
	ScorchedServer::instance()->getOptionsTransient().startNewGame();	

	return true;
}

bool ServerAdminCommon::killAll(ServerAdminSessions::Credential &credential)
{
	if (!credential.hasPermission(
		ServerAdminSessions::PERMISSION_ALTERGAME)) return false;

	adminLog(ChannelText("info",
		"ADMIN_KILL_ALL",
		"\"{0}\" admin kill all",
		credential.username));
	ServerCommon::killAll();

	return true;
}

bool ServerAdminCommon::adminSay(ServerAdminSessions::Credential &credential,
	const char *channel, const char *text)
{
	LangString langString(LANG_STRING(text));

	ChannelText channelText(channel, langString);
	channelText.setAdminPlayer(credential.username.c_str());
	ServerChannelManager::instance()->sendText(channelText, true);

	return true;
}
