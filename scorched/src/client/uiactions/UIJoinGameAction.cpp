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

#include <uiactions/UIJoinGameAction.h>
#include <client/ScorchedClient.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <coms/ComsTankChangeMessage.h>
#include <coms/ComsMessageSender.h>

UIJoinGameAction::UIJoinGameAction()
{
}

UIJoinGameAction::~UIJoinGameAction()
{
}

void UIJoinGameAction::performUIAction()
{
	std::map<unsigned int, Tank *> &tanks = 
		ScorchedClient::instance()->getTargetContainer().getTanks();
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		++itor)
	{
		Tank *tank = (*itor).second;
		if ((tank->getDestinationId() == 
			ScorchedClient::instance()->getTargetContainer().getCurrentDestinationId()) &&
			(tank->getPlayerId() != TargetID::SPEC_TANK_ID))
		{
			// Add this player
			ComsTankChangeMessage message(tank->getPlayerId(),
				LANG_STRING("TestName"),
				Vector(),
				"none",
				"none",
				ScorchedClient::instance()->getTargetContainer().getCurrentDestinationId(),
				0,
				"Human",
				false);
			ComsMessageSender::sendToServer(message);
		}
	}


	// Add avatar (if not one)
	// TODO
	/*
	Tank *tank = ScorchedClient::instance()->getTargetContainer().
		getTankById(currentPlayerId_);
	if (tank && 
		strcmp(tank->getAvatar().getName(), imageList_->getCurrentShortPath()) != 0)
	{
		if (tank->getAvatar().loadFromFile(imageList_->getCurrentLongPath()))
		{
			if (tank->getAvatar().getFile().getBufferUsed() <=
				(unsigned) ScorchedClient::instance()->getOptionsGame().getMaxAvatarSize())
			{
				message.setPlayerIconName(imageList_->getCurrentShortPath());
				message.getPlayerIcon().addDataToBuffer(
					tank->getAvatar().getFile().getBuffer(),
					tank->getAvatar().getFile().getBufferUsed());
			}
			else
			{
				Logger::log( "Warning: Avatar too large to send to server");
			}
		}
	}
	*/

	delete this;
}
