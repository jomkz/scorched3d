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

#include <uiactions/UIRotateTankAction.h>
#include <uiactions/UITankRenderer.h>
#include <client/ScorchedClient.h>
#include <target/TargetContainer.h>
#include <scorched3dc/ScorchedUI.h>
#include <scorched3dc/UIState.h>
#include <scorched3dc/UIStatePlaying.h>
#include <scorched3dc/UIStatePlayingTargets.h>

UIRotateTankAction::UIRotateTankAction(unsigned int playerId, fixed rotation, fixed elevation) :
	playerId_(playerId), rotation_(rotation), elevation_(elevation)
{
}

UIRotateTankAction::~UIRotateTankAction()
{
}

void UIRotateTankAction::performUIAction()
{
	Tank *tank = ScorchedClient::instance()->getTargetContainer().getTankById(playerId_);
	if (tank)
	{
		UITankRenderer *renderer = (UITankRenderer *) tank->getRenderer();
		if (renderer)
		{
			renderer->getShotHistory().rotateGunXY(rotation_, false);
			renderer->getShotHistory().rotateGunXY(elevation_, false);
		}
	}
	delete this;
}
