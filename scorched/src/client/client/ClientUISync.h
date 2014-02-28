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

#if !defined(__INCLUDE_ClientUISynch_INCLUDE__)
#define __INCLUDE_ClientUISynch_INCLUDE__

class ClientUISyncAction
{
public:
	ClientUISyncAction();
	virtual ~ClientUISyncAction();

	virtual void performUIAction() = 0;
};

class ClientUISyncActionBuffer
{
public:
	ClientUISyncActionBuffer();
	~ClientUISyncActionBuffer();

	void addClientUISyncAction(ClientUISyncAction *action);

	int getActionCount() { return actionCount_; }
	void resetCount() { actionCount_ = 0; }
	ClientUISyncAction **getActions() { return actions_; }

protected:
	int actionCount_;
	int actionsSize_;
	ClientUISyncAction **actions_;
};

class ClientUISyncFromClient 
{
public:
	virtual ~ClientUISyncFromClient() {}

	virtual void checkForSyncFromClient() = 0;
	virtual void addActionFromClient(ClientUISyncAction *action) = 0;
};

class ClientUISyncFromUI 
{
public:
	virtual ~ClientUISyncFromUI() {}

	virtual void checkForSyncFromUI() = 0;
	virtual void addActionFromUI(ClientUISyncAction *action) = 0;
};

class ClientUISync : public ClientUISyncFromClient, 
					public ClientUISyncFromUI
{
public:
	ClientUISync();
	virtual ~ClientUISync();

	virtual void checkForSyncFromClient();
	virtual void checkForSyncFromUI();

	virtual void addActionFromClient(ClientUISyncAction *action);
	virtual void addActionFromUI(ClientUISyncAction *action);

	bool currentlySynching() { return currentlySynching_; }
protected:
	ClientUISyncActionBuffer actionsFromClient_;
	ClientUISyncActionBuffer actionsFromUI_;
	bool currentlySynching_;
	boost::mutex syncMutex_;
	boost::condition_variable syncCond_;
};

#endif
