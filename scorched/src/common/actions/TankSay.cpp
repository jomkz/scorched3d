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

#include <actions/TankSay.h>
#ifndef S3D_SERVER
	#include <sprites/TalkRenderer.h>
	#include <sprites/ExplosionTextures.h>
#endif
#include <tank/TankContainer.h>
#include <tank/TankPosition.h>
#include <tank/TankAvatar.h>
#include <engine/ScorchedContext.h>
#include <engine/ActionController.h>
#include <common/Defines.h>
#include <common/ChannelManager.h>

TankSay::TankSay(unsigned int playerId, const LangString &text) :
	ActionReferenced("TankSay"),
	playerId_(playerId), text_(text)
{

}

TankSay::~TankSay()
{
}

void TankSay::init()
{
	Tank *tank = 
		context_->getTankContainer().getTankById(playerId_);
	if (tank)
	{
#ifndef S3D_SERVER
		if (!context_->getServerMode()) 
		{
			// put a speach bubble over the talking tank
			Vector white(1.0f, 1.0f, 1.0f);
			TalkRenderer *talk = new TalkRenderer(
				tank->getPosition().getTankTurretPosition().asVector(),
				white);
			context_->getActionController().addAction(new SpriteAction(talk));

			ChannelText text("general", text_);
			text.setSrcPlayerId(playerId_);
			ChannelManager::showText(*context_, text);
		}
#endif // #ifndef S3D_SERVER
	}
}

void TankSay::simulate(fixed frameTime, bool &remove)
{
	remove = true;
	Action::simulate(frameTime, remove);
}
