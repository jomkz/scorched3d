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

class ClientUISyncActionRegisterable : public ClientUISyncAction
{
public:
	ClientUISyncActionRegisterable();
	virtual ~ClientUISyncActionRegisterable();

	// ClientUISyncAction
	virtual void performUIAction();

	void registerCallback();
protected:
	int registered_;
};

class ClientUISyncActionBuffer
{
public:
	ClientUISyncActionBuffer();
	~ClientUISyncActionBuffer();

	int addClientUISyncAction(ClientUISyncAction *action);
	void removeClientUISyncAction(int position);

	int getActionCount() { return actionCount_; }
	void resetCount() { actionCount_ = 0; }
	ClientUISyncAction **getActions() { return actions_; }

protected:
	int actionCount_;
	int actionsSize_;
	ClientUISyncAction **actions_;
};

class ClientUISyncActionBuffers
{
public:
	ClientUISyncActionBuffer actionsFromClient;
	ClientUISyncActionBuffer actionsFromUI;
};

class ClientUISyncFromClient 
{
public:
	virtual ~ClientUISyncFromClient() {}

	virtual void checkForSyncFromClient() = 0;
	virtual int addActionFromClient(ClientUISyncAction *action) = 0;
	virtual void removeActionFromClient(int position) = 0;
};

class ClientUISyncFromUI 
{
public:
	virtual ~ClientUISyncFromUI() {}

	virtual void checkForSyncFromUI() = 0;
	virtual int addActionFromUI(ClientUISyncAction *action) = 0;
	virtual void removeActionFromUI(int position) = 0;
};

class ClientUISync : public ClientUISyncFromClient, 
					public ClientUISyncFromUI
{
public:
	ClientUISync();
	virtual ~ClientUISync();

	virtual void checkForSyncFromClient();
	virtual void checkForSyncFromUI();

	virtual int addActionFromClient(ClientUISyncAction *action);
	virtual void removeActionFromClient(int position);
	virtual int addActionFromUI(ClientUISyncAction *action);
	virtual void removeActionFromUI(int position);

	bool currentlySynching() { return currentlySynching_; }
protected:
	ClientUISyncActionBuffers firstBuffers_, secondBuffers_;
	ClientUISyncActionBuffers *currentBuffers_;
	bool pointingToFirstBuffers_;
	bool currentlySynching_;
	boost::mutex syncMutex_;
	boost::condition_variable syncCond_;
};

#endif
