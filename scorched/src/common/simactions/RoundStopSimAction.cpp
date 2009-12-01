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

#include <simactions/RoundStopSimAction.h>
#include <simactions/TankStopMoveSimAction.h>
#include <tank/TankContainer.h>
#ifndef S3D_SERVER
#include <graph/ShotCountDown.h>
#endif

REGISTER_CLASS_SOURCE(RoundStopSimAction);

RoundStopSimAction::RoundStopSimAction()
{
}

RoundStopSimAction::~RoundStopSimAction()
{
}

bool RoundStopSimAction::invokeAction(ScorchedContext &context)
{
	// Stop the current round
	context.getTankContainer().setCurrentRoundId(0);

	// Stop any tanks from making any moves
	std::map<unsigned int, Tank *> &tanks =
		context.getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank *>::iterator tankItor;
	for (tankItor = tanks.begin();
		tankItor != tanks.end();
		tankItor++)
	{
		TankStopMoveSimAction::stopMove(context, tankItor->second);
	}

	if (!context.getServerMode())
	{
#ifndef S3D_SERVER
		ShotCountDown::instance()->hideRoundTime();
#endif
	}

	return true;
}

bool RoundStopSimAction::writeMessage(NetBuffer &buffer)
{
	return true;
}

bool RoundStopSimAction::readMessage(NetBufferReader &reader)
{
	return true;
}