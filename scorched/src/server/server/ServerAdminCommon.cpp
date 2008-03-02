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

#include <server/ServerAdminCommon.h>
#include <server/ServerCommon.h>
#include <server/ScorchedServer.h>
#include <server/ScorchedServerUtil.h>
#include <server/ServerChannelManager.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <target/TargetLife.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <common/FileLogger.h>

static FileLogger *serverAdminFileLogger = 0;

static void adminLog(const std::string &message)
{
	if (!serverAdminFileLogger) 
	{
		char buffer[256];
		snprintf(buffer, 256, "ServerAdminLog-%i-", 
			ScorchedServer::instance()->getOptionsGame().getPortNo());
		serverAdminFileLogger = new FileLogger(buffer);
	}	

	ServerCommon::sendString(0, message);
	LoggerInfo info(message);
	info.setTime();
	serverAdminFileLogger->logMessage(info);
}

static void internalBanPlayer(const char *adminName, 
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
				addBanned(ipAddress, tank->getName(), tank->getUniqueId(), tank->getSUI(), 
					type, adminName, reason);
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

bool ServerAdminCommon::kickPlayer(const char *adminUser, unsigned int playerId)
{
	Tank *targetTank = ScorchedServer::instance()->
		getTankContainer().getTankById(playerId);
	if (!targetTank) return false;

	adminLog(
		S3D::formatStringBuffer("\"%s\" admin kick \"%s\"",
		adminUser,
		targetTank->getName()));
	ServerCommon::kickPlayer(
		targetTank->getPlayerId());

	return true;
}

bool ServerAdminCommon::poorPlayer(const char *adminUser, unsigned int playerId)
{
	Tank *targetTank = ScorchedServer::instance()->
		getTankContainer().getTankById(playerId);
	if (!targetTank) return false;

	adminLog(
		S3D::formatStringBuffer("\"%s\" admin poor \"%s\"",
		adminUser,
		targetTank->getName()));
	targetTank->getScore().setMoney(0);

	return true;
}

bool ServerAdminCommon::banPlayer(const char *adminUser, unsigned int playerId, const char *reason)
{
	Tank *targetTank = ScorchedServer::instance()->
		getTankContainer().getTankById(playerId);
	if (!targetTank) return false;

	adminLog(
		S3D::formatStringBuffer("\"%s\" admin ban \"%s\"",
		adminUser,
		targetTank->getName()));
	internalBanPlayer(
		adminUser,
		targetTank->getPlayerId(), 
		ServerBanned::Banned, 
		reason);

	return true;
}

bool ServerAdminCommon::slapPlayer(const char *adminUser, unsigned int playerId, float slap)
{
	Tank *targetTank = ScorchedServer::instance()->
		getTankContainer().getTankById(playerId);
	if (!targetTank) return false;

	adminLog(
		S3D::formatStringBuffer("\"%s\" admin slap \"%s\" %.0f",
		adminUser,
		targetTank->getName(),
		slap));
	targetTank->getLife().setLife(
		targetTank->getLife().getLife() - fixed(int(slap)));

	return true;
}

bool ServerAdminCommon::flagPlayer(const char *adminUser, unsigned int playerId, const char *reason)
{
	Tank *targetTank = ScorchedServer::instance()->
		getTankContainer().getTankById(playerId);
	if (!targetTank) return false;

	adminLog(
		S3D::formatStringBuffer("\"%s\" admin flag \"%s\"",
		adminUser,
		targetTank->getName()));
	internalBanPlayer(
		adminUser,
		targetTank->getPlayerId(), 
		ServerBanned::Flagged, 
		reason);

	return true;
}

bool ServerAdminCommon::mutePlayer(const char *adminUser, unsigned int playerId, bool mute)
{
	Tank *targetTank = ScorchedServer::instance()->
		getTankContainer().getTankById(playerId);
	if (!targetTank) return false;

	adminLog(
		S3D::formatStringBuffer("\"%s\" admin %s \"%s\"",
		adminUser,
		(mute?"mute":"unmute"),
		targetTank->getName()));
	targetTank->getState().setMuted(mute); 

	return true;
}

bool ServerAdminCommon::permMutePlayer(const char *adminUser, unsigned int playerId, const char *reason)
{
	Tank *targetTank = ScorchedServer::instance()->
		getTankContainer().getTankById(playerId);
	if (!targetTank) return false;

	adminLog(
		S3D::formatStringBuffer("\"%s\" admin permmute \"%s\"",
		adminUser,
		targetTank->getName()));
	internalBanPlayer(
		adminUser,
		targetTank->getPlayerId(),
		ServerBanned::Muted, 
		reason);
	targetTank->getState().setMuted(true);

	return true;
}

bool ServerAdminCommon::unpermMutePlayer(const char *adminUser, unsigned int playerId)
{
	Tank *targetTank = ScorchedServer::instance()->
		getTankContainer().getTankById(playerId);
	if (!targetTank) return false;

	adminLog(
		S3D::formatStringBuffer("\"%s\" admin unpermmute \"%s\"",
		adminUser,
		targetTank->getName()));
	internalBanPlayer(
		adminUser,
		targetTank->getPlayerId(),
		ServerBanned::NotBanned, 
		"");
	targetTank->getState().setMuted(false);

	return true;
}

bool ServerAdminCommon::newGame(const char *adminUser)
{
	adminLog(
		S3D::formatStringBuffer("\"%s\" admin new game",
		adminUser));
	ServerCommon::killAll();
	ScorchedServer::instance()->getOptionsTransient().startNewGame();	

	return true;
}

bool ServerAdminCommon::killAll(const char *adminUser)
{
	adminLog(
		S3D::formatStringBuffer("\"%s\" admin kill all",
		adminUser));
	ServerCommon::killAll();

	return true;
}

bool ServerAdminCommon::adminSay(ServerAdminSessions::SessionParams *session,
	const char *channel, const char *text)
{
	ChannelText channelText(channel, text);
	channelText.setAdminPlayer(session->userName.c_str());
	ServerChannelManager::instance()->sendText(channelText, true);

	return true;
}
