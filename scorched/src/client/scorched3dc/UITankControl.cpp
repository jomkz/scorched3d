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

#include <scorched3dc/UITankControl.h>
#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <target/TargetContainer.h>
#include <tanket/TanketAccessories.h>
#include <tanket/TanketShotInfo.h>
#include <tank/Tank.h>
#include <weapons/Accessory.h>
#include <coms/ComsPlayedMoveMessage.h>
#include <coms/ComsMessageSender.h>

UITankControlSyncActionFireShot::UITankControlSyncActionFireShot(UITankShotHistory::ShotEntry &entry, unsigned int playerId) :
	playerId_(playerId), entry_(entry)
{
}

UITankControlSyncActionFireShot::~UITankControlSyncActionFireShot()
{
}

void UITankControlSyncActionFireShot::performUIAction()
{
	Tank *tank = ScorchedClient::instance()->getTargetContainer().getTankById(playerId_);
	if (!tank) return;
	Accessory *currentWeapon = tank->getAccessories().getWeapons().getCurrent();
	if (!currentWeapon) return;
	unsigned int moveId = tank->getShotInfo().getMoveId();
	if (moveId == 0) return;

	// Send message saying we are finished with shot
	ComsPlayedMoveMessage comsMessage(
		tank->getPlayerId(), 
		moveId,
		ComsPlayedMoveMessage::eShot);
	comsMessage.setShot(
		currentWeapon->getAccessoryId(),
		entry_.rot,
		entry_.ele,
		entry_.power,
		0,
		0);
	ComsMessageSender::sendToServer(comsMessage);
}

UITankControl::UITankControl() 
{

}

UITankControl::~UITankControl()
{

}

void UITankControl::fireShot(UITankRenderer *tankRenderer)
{
	UITankControlSyncActionFireShot *syncAction = 
		new UITankControlSyncActionFireShot(
			tankRenderer->getShotHistory().getCurrentValues(), 
			tankRenderer->getTarget()->getPlayerId());
	tankRenderer->getShotHistory().madeShot();
	ScorchedClient::getClientUISyncExternal().addActionFromUI(syncAction);
}
