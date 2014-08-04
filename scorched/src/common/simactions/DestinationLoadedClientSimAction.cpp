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

#include <simactions/DestinationLoadedClientSimAction.h>
#include <target/TargetContainer.h>
#ifndef S3D_SERVER
#include <client/ScorchedClient.h>
#include <uistate/UIState.h>
#endif

REGISTER_CLASS_SOURCE(DestinationLoadedClientSimAction);

DestinationLoadedClientSimAction::DestinationLoadedClientSimAction() :
	destinationId_(0), initialLevel_(false)
{
}

DestinationLoadedClientSimAction::DestinationLoadedClientSimAction(unsigned int destinationId, bool initialLevel) :
	destinationId_(destinationId), initialLevel_(initialLevel)
{
}

DestinationLoadedClientSimAction::~DestinationLoadedClientSimAction()
{
}

bool DestinationLoadedClientSimAction::invokeAction(ScorchedContext &context)
{
	if (!context.getServerMode())
	{
		setLoaded(context);
	}

	return true;
}

void DestinationLoadedClientSimAction::setLoaded(ScorchedContext &context)
{
	ScorchedClient &client = (ScorchedClient &) context;
	if (client.getTargetContainer().getCurrentDestinationId() != destinationId_) return;

#ifndef S3D_SERVER
	// Show the player selection dialogs
	if (initialLevel_)
	{
		initialLevel_ = false;
		// TODO
		/*
		if (ClientParams::instance()->getConnectedToServer())
		{
		PlayerInGameDialog::instance()->displayDialog();
		PlayerInGameDialog::instance()->initializeFirst();
		}
		else
		{
		PlayerInitialDialog::instance()->displayDialog();
		}
		*/

		UIState::setStateNonUIThread(UIState::StateJoining);
	}
	else
	{
		UIState::setStateNonUIThread(UIState::StatePlaying);
	}
#endif
}

bool DestinationLoadedClientSimAction::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(destinationId_);
	buffer.addToBuffer(initialLevel_);
	return true;
}

bool DestinationLoadedClientSimAction::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(destinationId_)) return false;
	if (!reader.getFromBuffer(initialLevel_)) return false;
	return true;
}
