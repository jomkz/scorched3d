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

#include <client/ScorchedClient.h>
#include <client/ClientGiftMoneyHandler.h>
#include <GLW/GLWWindowManager.h>
#include <tank/TankContainer.h>
#include <tank/TankScore.h>
#include <coms/ComsGiftMoneyMessage.h>
#include <dialogs/BuyAccessoryDialog.h>
#include <lang/LangResource.h>
#include <common/ChannelManager.h>

ClientGiftMoneyHandler *ClientGiftMoneyHandler::instance_ = 0;

ClientGiftMoneyHandler *ClientGiftMoneyHandler::instance()
{
	if (!instance_)
	{
		instance_ = new ClientGiftMoneyHandler;
	}
	return instance_;
}

ClientGiftMoneyHandler::ClientGiftMoneyHandler()
{
	ScorchedClient::instance()->getComsMessageHandler().addHandler(
		"ComsGiftMoneyMessage",
		this);
}

ClientGiftMoneyHandler::~ClientGiftMoneyHandler()
{
}

bool ClientGiftMoneyHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	ComsGiftMoneyMessage message;
	if (!message.readMessage(reader)) return false;

	Tank *toTank = ScorchedClient::instance()->getTankContainer().
		getTankById(message.getToPlayerId());
	Tank *fromTank = ScorchedClient::instance()->getTankContainer().
		getTankById(message.getFromPlayerId());
	if (!toTank || !fromTank) return true;

	toTank->getScore().setMoney(toTank->getScore().getMoney() + 
		message.getMoney());
	fromTank->getScore().setMoney(fromTank->getScore().getMoney() - 
		message.getMoney());

	if (GLWWindowManager::instance()->windowVisible(BuyAccessoryDialog::instance()->getId()))
	{
		BuyAccessoryDialog::instance()->playerRefreshKeepPos();
	}

	ChannelText text("combat", 
		LANG_RESOURCE_3(
			"TANK_GIFT_MESSAGE", 
			"[p:{0}] gifts ${1} to [p:{2}]", 
			fromTank->getTargetName(), 
			message.getMoney(), 
			toTank->getTargetName()));
	ChannelManager::showText(ScorchedClient::instance()->getContext(), text);

	return true;
}

