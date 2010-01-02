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

#include <dialogs/BuyAccessoryDialogTankInfo.h>
#include <client/ScorchedClient.h>
#include <tank/TankContainer.h>
#include <tank/TankScore.h>
#include <tank/TankState.h>

BuyAccessoryDialogTankInfo *BuyAccessoryDialogTankInfo::instance_ = 0;

BuyAccessoryDialogTankInfo *BuyAccessoryDialogTankInfo::instance()
{
	if (!instance_) instance_ = new BuyAccessoryDialogTankInfo();
	return instance_;
}

BuyAccessoryDialogTankInfo::BuyAccessoryDialogTankInfo() :
	tankAccessories(ScorchedClient::instance()->getContext())
{
}

void BuyAccessoryDialogTankInfo::set()
{
	Tank *tank = ScorchedClient::instance()->getTankContainer().getCurrentTank();
	if (tank)
	{
		tankAccessories.setTank(tank);
		NetBuffer buffer;
		tank->getAccessories().writeMessage(buffer, true);
		NetBufferReader reader(buffer);
		tankAccessories.readMessage(reader);
		tankColor = tank->getColor();
		tankId = tank->getPlayerId();
		tankMoney = tank->getScore().getMoney();
		tankName = tank->getTargetName();
		tankMoveId = tank->getState().getMoveId();
	} 
	else
	{
		tankAccessories.setTank(0);
		tankAccessories.clearAccessories();
		tankColor = Vector::getNullVector();
		tankName = LangString();
		tankMoney = 0;
		tankId = 0;
		tankMoveId = 0;
	}
}
