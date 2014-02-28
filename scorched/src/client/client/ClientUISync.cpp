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

#include <client/ClientUISync.h>
#include <common/DefinesAssert.h>

ClientUISyncAction::ClientUISyncAction()
{
}

ClientUISyncAction::~ClientUISyncAction()
{
}

ClientUISyncActionBuffer::ClientUISyncActionBuffer()
{
	actionsSize_ = 1024;
	actions_ = new ClientUISyncAction*[actionsSize_];
	actionCount_ = 0;
}

ClientUISyncActionBuffer::~ClientUISyncActionBuffer()
{
	delete [] actions_;
	actions_ = 0;
}

int ClientUISyncActionBuffer::addClientUISyncAction(ClientUISyncAction *action)
{
	if (actionCount_ == actionsSize_)
	{
		ClientUISyncAction **newActions = new ClientUISyncAction*[actionsSize_ * 2];
		memcpy(newActions, actions_, sizeof(ClientUISyncAction *) * actionsSize_);
		delete [] actions_;
		actions_ = newActions;
		actionsSize_ *= 2;
	}
	actions_[actionCount_] = action;
	++actionCount_;
	return actionCount_ - 1;
}

void ClientUISyncActionBuffer::removeClientUISyncAction(int position)
{
	DIALOG_ASSERT(position >= 0 && position < actionCount_);
	actions_[position] = 0;
}

ClientUISync::ClientUISync() : currentlySynching_(false)
{
}

ClientUISync::~ClientUISync()
{
}

int ClientUISync::addActionFromClient(ClientUISyncAction *action)
{
	return actionsFromClient_.addClientUISyncAction(action);
}

void ClientUISync::removeActionFromClient(int position)
{
	actionsFromClient_.removeClientUISyncAction(position);
}

int ClientUISync::addActionFromUI(ClientUISyncAction *action)
{
	return actionsFromUI_.addClientUISyncAction(action);
}

void ClientUISync::removeActionFromUI(int position)
{
	actionsFromUI_.removeClientUISyncAction(position);
}

void ClientUISync::checkForSyncFromClient()
{
	if (!actionsFromClient_.getActionCount()) return;

	{
		boost::unique_lock<boost::mutex> lock(syncMutex_);
		currentlySynching_ = true;
		while (currentlySynching_)
		{
			syncCond_.wait(lock);
		}
	}
}

void ClientUISync::checkForSyncFromUI()
{
	if (!currentlySynching_) return;

	{
		boost::unique_lock<boost::mutex> lock(syncMutex_);

		{
			ClientUISyncAction **currentAction = actionsFromClient_.getActions();
			for (int i=0; i<actionsFromClient_.getActionCount(); ++i, ++currentAction)
			{
				if (*currentAction) (*currentAction)->performUIAction();
			}
			actionsFromClient_.resetCount();
		}
		{
			ClientUISyncAction **currentAction = actionsFromUI_.getActions();
			for (int i=0; i<actionsFromUI_.getActionCount(); ++i, ++currentAction)
			{
				if (*currentAction) (*currentAction)->performUIAction();
			}
			actionsFromUI_.resetCount();
		}

		currentlySynching_ = false;
		syncCond_.notify_one();
	}
}
