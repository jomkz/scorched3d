////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
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

#if !defined(__INCLUDE_ComsAdminMessageh_INCLUDE__)
#define __INCLUDE_ComsAdminMessageh_INCLUDE__

#include <coms/ComsMessage.h>

class ComsAdminMessage : public ComsMessage
{
public:
	enum ComsAdminMessageType
	{
		AdminNone,
		AdminLogin,
		AdminLogout,
		AdminShow,
		AdminShowBanned,
		AdminSyncCheck,
		AdminKillAll,
		AdminKick,
		AdminSlap,
		AdminPoor,
		AdminBan,
		AdminFlag,
		AdminMute,
		AdminPermMute,
		AdminUnPermMute,
		AdminUnMute,
		AdminAdminTalk,
		AdminTalk,
		AdminMessage,
		AdminNewGame
	};

	ComsAdminMessage(
		ComsAdminMessageType type = AdminNone, 
		const char *param1_ = "",
		const char *param2_ = "");
	virtual ~ComsAdminMessage();

	ComsAdminMessageType getType() { return type_; }
	const char *getParam1() { return param1_.c_str(); }
	const char *getParam2() { return param2_.c_str(); }

	// Inherited from ComsMessage
	virtual bool writeMessage(NetBuffer &buffer);
	virtual bool readMessage(NetBufferReader &reader);

protected:
	ComsAdminMessageType type_;
	std::string param1_;
	std::string param2_;

private:
	ComsAdminMessage(const ComsAdminMessage &);
	const ComsAdminMessage & operator=(const ComsAdminMessage &);

};

#endif // __INCLUDE_ComsAdminMessageh_INCLUDE__

