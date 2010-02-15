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

#if !defined(AFX_AdminSimAction_H__2C00E711_B337_4665_AB54_C6661FD67E5D__INCLUDED_)
#define AFX_AdminSimAction_H__2C00E711_B337_4665_AB54_C6661FD67E5D__INCLUDED_

#include <simactions/SimAction.h>

class AdminSimAction : public SimAction
{
public:
	enum AdminType
	{
		eKillAll,
		eNewGame,
		eSlap,
		eKill
	};

	AdminSimAction();
	AdminSimAction(AdminType type, unsigned int playerId, fixed amount);
	virtual ~AdminSimAction();

	virtual bool invokeAction(ScorchedContext &context);

	virtual bool writeMessage(NetBuffer &buffer);
	virtual bool readMessage(NetBufferReader &reader);

REGISTER_CLASS_HEADER(AdminSimAction);
protected:
	AdminType type_;
	unsigned int playerId_;
	fixed amount_;

	void killAll(ScorchedContext &context);
	void newGame(ScorchedContext &context);
	void slap(ScorchedContext &context);
	void kill(ScorchedContext &context);
};

#endif // !defined(AFX_AdminSimAction_H__2C00E711_B337_4665_AB54_C6661FD67E5D__INCLUDED_)
