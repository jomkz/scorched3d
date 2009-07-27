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

#include <simactions/TankChangeSimAction.h>
#include <tank/TankAvatar.h>
#include <tank/TankState.h>
#include <tank/TankContainer.h>

REGISTER_CLASS_SOURCE(TankChangeSimAction);

TankChangeSimAction::TankChangeSimAction()
{
}

TankChangeSimAction::TankChangeSimAction(Tank *tank)
{
	buffer_.addToBuffer(tank->getPlayerId());
	buffer_.addToBuffer(tank->getTargetName());
	buffer_.addToBuffer(tank->getTeam());
	buffer_.addToBuffer(tank->getColor());
	tank->getAvatar().writeMessage(buffer_);
}

TankChangeSimAction::~TankChangeSimAction()
{

}

bool TankChangeSimAction::invokeAction(ScorchedContext &context)
{
	NetBufferReader reader(buffer_);
	unsigned int playerId;
	if (!reader.getFromBuffer(playerId)) return false;
	Tank *tank = context.getTankContainer().getTankById(playerId);
	if (!tank) return false;
	LangString targetName;
	if (!reader.getFromBuffer(targetName)) return false;
	tank->setName(targetName);
	unsigned int team;
	if (!reader.getFromBuffer(team)) return false;
	tank->setTeam(team);
	Vector color;
	if (!reader.getFromBuffer(color)) return false;
	tank->setColor(color);
	if (!tank->getAvatar().readMessage(reader)) return false;

	tank->getState().setState(TankState::sDead);
	tank->getState().setNotSpectator(true);

	return true;
}

bool TankChangeSimAction::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(buffer_);
	return true;
}

bool TankChangeSimAction::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(buffer_)) return false;
	return true;
}
