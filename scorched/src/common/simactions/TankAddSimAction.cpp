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

#include <simactions/TankAddSimAction.h>
#include <tank/TankContainer.h>
#include <tank/TankAvatar.h>
#include <tank/TankState.h>
#include <common/ChannelManager.h>
#include <common/StatsLogger.h>
#include <common/Logger.h>
#include <server/ServerBanned.h>
#include <server/ScorchedServerUtil.h>
#include <server/ServerChannelManager.h>
#ifndef S3D_SERVER
#include <client/ClientChannelManager.h>
#include <tankgraph/TargetRendererImplTank.h>
#endif

REGISTER_CLASS_SOURCE(TankAddSimAction);

TankAddSimAction::TankAddSimAction()
{
}

TankAddSimAction::TankAddSimAction(ComsAddPlayerMessage &message, 
	const std::string &uniqueId, const std::string &sUID, const std::string &hostDesc,
	unsigned int ipAddress) :
	message_(message),
	uniqueId_(uniqueId), sUID_(sUID), hostDesc_(hostDesc),
	ipAddress_(ipAddress)
{

}

TankAddSimAction::~TankAddSimAction()
{

}

bool TankAddSimAction::invokeAction(ScorchedContext &context)
{
	// Create tank
	Tank *tank = new Tank(
		context,
		message_.getPlayerId(),
		message_.getDestinationId(),
		message_.getPlayerName(),
		message_.getPlayerColor(),
		message_.getModelName(),
		message_.getTankType());
	tank->setTeam(message_.getPlayerTeam());
	tank->getAvatar().setFromBuffer(
		message_.getPlayerIconName(),
		message_.getPlayerIcon(),
		true);
	if (context.getServerMode())
	{
		tank->setUniqueId(uniqueId_.c_str());
		tank->setSUI(sUID_.c_str());
		tank->setHostDesc(hostDesc_.c_str());
		tank->setIpAddress(ipAddress_);
	}

	// Add this tank 
	context.getTankContainer().addTank(tank);
	StatsLogger::instance()->tankConnected(tank);

#ifndef S3D_SERVER
	if (!context.getServerMode())
	{
		tank->setRenderer(new TargetRendererImplTank(tank));
		if (ClientChannelManager::instance()->getMutedPlayers().find(tank->getPlayerId())
			!= ClientChannelManager::instance()->getMutedPlayers().end())
		{
			tank->getState().setMuted(true);
		}
	}
#else
	{
		// Add to dialog
		Logger::log(S3D::formatStringBuffer("Player connected dest=\"%i\" id=\"%i\" name=\"%s\" unique=[%s] SUI=[%s]",
			tank->getDestinationId(),
			tank->getPlayerId(),
			tank->getCStrName().c_str(),
			tank->getUniqueId(),
			tank->getSUI()));
	}
	// Check if admin muted
	if (ipAddress_ != 0)
	{
		ServerBanned::BannedType type = 
			ScorchedServerUtil::instance()->bannedPlayers.getBanned(tank->getUniqueId(), tank->getSUI());
		if (type == ServerBanned::Muted)	
		{
			tank->getState().setMuted(true);
			ServerChannelManager::instance()->sendText( 
				ChannelText("admin", 
					"PLAYER_ADMIN_MUTED", 
					"Player admin muted \"{0}\"",
					tank->getTargetName()),
					true);
		}
		else if (type == ServerBanned::Flagged)
		{
			ServerChannelManager::instance()->sendText( 
				ChannelText("admin",
					"PLAYER_ADMIN_FLAGGED",
					"Player admin flagged \"{0}\"",
					tank->getTargetName()),
					true);
		}
	}
#endif

	if (!context.getServerMode())
	{
		ChannelText text("info", 
			"PLAYER_CONNECTED",
			"Player connected \"{0}\"",
			tank->getTargetName());
		ChannelManager::showText(context, text);
	}

	return true;
}

bool TankAddSimAction::writeMessage(NetBuffer &buffer)
{
	message_.writeMessage(buffer);
	return true;
}

bool TankAddSimAction::readMessage(NetBufferReader &reader)
{
	if (!message_.readMessage(reader)) return false;
	return true;
}
