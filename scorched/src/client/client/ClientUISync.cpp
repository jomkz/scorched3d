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
#include <client/ScorchedClient.h>
#include <scorched3dc/ScorchedUI.h>
#include <common/DefinesAssert.h>

ClientUISyncAction::ClientUISyncAction()
{
}

ClientUISyncAction::~ClientUISyncAction()
{
}

ClientUISyncActionRegisterable::ClientUISyncActionRegisterable(bool calledFromClient) : 
	registered_(-1), calledFromClient_(calledFromClient)
{
}

ClientUISyncActionRegisterable::~ClientUISyncActionRegisterable()
{
	if (registered_ != -1)
	{
		if (calledFromClient_)
		{
			ScorchedClient::instance()->getClientUISync().removeActionFromClient(registered_);
		}
		else
		{
			ScorchedClient::getClientUISyncExternal().removeActionFromUI(registered_);
		}
	}
}

void ClientUISyncActionRegisterable::performUIAction()
{
	registered_ = -1;
}

void ClientUISyncActionRegisterable::registerCallback()
{
	if (registered_ == -1)
	{
		if (calledFromClient_)
		{
			registered_ = ScorchedClient::instance()->getClientUISync().addActionFromClient(this);
		}
		else
		{
			registered_ = ScorchedClient::getClientUISyncExternal().addActionFromUI(this);
		}
	}
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

ClientUISync::ClientUISync() : 
	currentlySynching_(false)
{
}

ClientUISync::~ClientUISync()
{
}

int ClientUISync::addActionFromClient(ClientUISyncAction *action)
{
	ENSURE_CLIENT_THREAD
	return currentBuffers_.actionsFromClient.addClientUISyncAction(action);
}

void ClientUISync::removeActionFromClient(int position)
{
	ENSURE_CLIENT_THREAD
	currentBuffers_.actionsFromClient.removeClientUISyncAction(position);
}

int ClientUISync::addActionFromUI(ClientUISyncAction *action)
{
	ENSURE_UI_THREAD
	return currentBuffers_.actionsFromUI.addClientUISyncAction(action);
}

void ClientUISync::removeActionFromUI(int position)
{
	ENSURE_UI_THREAD
	currentBuffers_.actionsFromUI.removeClientUISyncAction(position);
}

void ClientUISync::checkForSyncFromClient()
{
	ENSURE_CLIENT_THREAD
	if (!currentBuffers_.actionsFromClient.getActionCount() &&
		!currentBuffers_.actionsFromUI.getActionCount()) return;

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
	ENSURE_UI_THREAD
	if (!currentlySynching_) return;

	{
		boost::unique_lock<boost::mutex> lock(syncMutex_);

		{
			ClientUISyncAction **startAction = currentBuffers_.actionsFromClient.getActions();
			ClientUISyncAction **currentAction = startAction;
			for (int i=0; i<currentBuffers_.actionsFromClient.getActionCount(); ++i, ++currentAction)
			{
				if (*currentAction) (*currentAction)->performUIAction();
				if (startAction != currentBuffers_.actionsFromClient.getActions())
				{
					startAction = currentBuffers_.actionsFromClient.getActions();
					currentAction = &startAction[i];
				}
			}
			currentBuffers_.actionsFromClient.resetCount();
		}
		{
			ClientUISyncAction **startAction = currentBuffers_.actionsFromUI.getActions();
			ClientUISyncAction **currentAction = startAction;
			for (int i=0; i<currentBuffers_.actionsFromUI.getActionCount(); ++i, ++currentAction)
			{
				if (*currentAction) (*currentAction)->performUIAction();
				if (startAction != currentBuffers_.actionsFromUI.getActions())
				{
					startAction = currentBuffers_.actionsFromUI.getActions();
					currentAction = &startAction[i];
				}
			}
			currentBuffers_.actionsFromUI.resetCount();
		}

		currentlySynching_ = false;
		syncCond_.notify_one();
	}
}
