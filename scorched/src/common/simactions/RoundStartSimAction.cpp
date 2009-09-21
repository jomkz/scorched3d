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

#include <simactions/RoundStartSimAction.h>
#include <tank/TankContainer.h>
#include <engine/ActionController.h>
#include <actions/RoundTimerAction.h>
#ifndef S3D_SERVER
#include <actions/VisualRoundTimerAction.h>
#endif

REGISTER_CLASS_SOURCE(RoundStartSimAction);

RoundStartSimAction::RoundStartSimAction() :
	roundId_(0)
{
}

RoundStartSimAction::RoundStartSimAction(unsigned int roundId, fixed timeout) :
	roundId_(roundId), timeout_(timeout)
{
}

RoundStartSimAction::~RoundStartSimAction()
{
}

bool RoundStartSimAction::invokeAction(ScorchedContext &context)
{
	context.getTankContainer().setCurrentRoundId(roundId_);

	if (timeout_ != 0)
	{
		if (!context.getServerMode())
		{
#ifndef S3D_SERVER
			VisualRoundTimerAction *timerAction = 
				new VisualRoundTimerAction(roundId_, timeout_);
			context.getActionController().addAction(timerAction);
#endif
		}
		else
		{
			RoundTimerAction *timerAction = 
				new RoundTimerAction(roundId_, timeout_);
			context.getActionController().addAction(timerAction);
		}
	}

	return true;
}

bool RoundStartSimAction::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(roundId_);
	buffer.addToBuffer(timeout_);
	return true;
}

bool RoundStartSimAction::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(roundId_)) return false;
	if (!reader.getFromBuffer(timeout_)) return false;
	return true;
}