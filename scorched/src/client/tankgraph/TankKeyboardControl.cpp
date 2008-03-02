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

#include <tankgraph/TankKeyboardControl.h>
#include <tankgraph/TankKeyboardControlUtil.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <common/Defines.h>

TankKeyboardControl * TankKeyboardControl::instance_ = 0;

TankKeyboardControl * TankKeyboardControl::instance()
{
	if (!instance_)
	{
		instance_ = new TankKeyboardControl;
	}

	return instance_;
}

TankKeyboardControl::TankKeyboardControl() :
	GameStateI("TankKeyboardControl")
{

}

TankKeyboardControl::~TankKeyboardControl()
{

}

void TankKeyboardControl::enterState(const unsigned state)
{
	if (state == ClientState::StateShot)
	{
		Tank *currentTank =
			ScorchedClient::instance()->getTankContainer().getCurrentTank();
		if (currentTank)
		{
			if (currentTank->getState().getState() == TankState::sNormal)
			{
				TankKeyboardControlUtil::endPlayMove(currentTank);
			}
		}
	}
}

void TankKeyboardControl::keyboardCheck(const unsigned state, float frameTime, 
							char *buffer, unsigned int keyState,
							KeyboardHistory::HistoryElement *history, int hisCount, 
							bool &skipRest)
{
	Tank *currentTank =
		ScorchedClient::instance()->getTankContainer().getCurrentTank();
	if (currentTank)
	{
		if (currentTank->getState().getState() == TankState::sNormal)
		{
			TankKeyboardControlUtil::keyboardCheck(currentTank, state, frameTime,
				buffer, keyState);
		}
	}
}
