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

#include <client/ClientAdmin.h>
#include <client/ClientHandlers.h>
#include <client/ScorchedClient.h>
#include <client/ClientChannelManager.h>
#include <client/ClientAdminResultHandler.h>
#include <target/TargetContainer.h>
#include <console/Console.h>
#include <coms/ComsMessageSender.h>
#include <coms/ComsAdminMessage.h>
#include <common/Defines.h>

ClientAdmin::ClientAdmin(Console &console) 
{
	// Help
	deleter_.addRule(
		new ConsoleRuleMethodIAdapter<ClientAdmin>(console,
			this, &ClientAdmin::adminHelp, "admin", 
			ConsoleUtil::formParams(ConsoleRuleParam("help"))));

	// No Params
	deleter_.addRule(
		new ConsoleRuleMethodIAdapterEx<ClientAdmin>(console,
			this, &ClientAdmin::adminNoParams, "admin", 
			ConsoleUtil::formParams(ConsoleRuleParam("killall")),
			(unsigned int) ComsAdminMessage::AdminKillAll));
	deleter_.addRule(
		new ConsoleRuleMethodIAdapterEx<ClientAdmin>(console,
			this, &ClientAdmin::adminNoParams, "admin", 
			ConsoleUtil::formParams(ConsoleRuleParam("newgame")),
			(unsigned int) ComsAdminMessage::AdminNewGame));
	deleter_.addRule(
		new ConsoleRuleMethodIAdapterEx<ClientAdmin>(console,
			this, &ClientAdmin::adminNoParams, "admin", 
			ConsoleUtil::formParams(ConsoleRuleParam("showbanned")),
			(unsigned int) ComsAdminMessage::AdminShowBanned));
	deleter_.addRule(
		new ConsoleRuleMethodIAdapterEx<ClientAdmin>(console,
			this, &ClientAdmin::adminNoParams, "admin", 
			ConsoleUtil::formParams(ConsoleRuleParam("show")),
			(unsigned int) ComsAdminMessage::AdminShow));
	deleter_.addRule(
		new ConsoleRuleMethodIAdapterEx<ClientAdmin>(console,
			this, &ClientAdmin::adminNoParams, "admin", 
			ConsoleUtil::formParams(ConsoleRuleParam("logout")),
			(unsigned int) ComsAdminMessage::AdminLogout));
	deleter_.addRule(
		new ConsoleRuleMethodIAdapterEx<ClientAdmin>(console,
			this, &ClientAdmin::adminNoParams, "admin", 
			ConsoleUtil::formParams(ConsoleRuleParam("synccheck")),
			(unsigned int) ComsAdminMessage::AdminSyncCheck));

	// One Number Param 
	deleter_.addRule(
		new ConsoleRuleMethodIAdapterEx<ClientAdmin>(console,
			this, &ClientAdmin::adminOneParam, "admin", 
			ConsoleUtil::formParams(ConsoleRuleParam("kick"), 
			ConsoleRuleParam("player", ConsoleRuleTypeNumber)),
			(unsigned int) ComsAdminMessage::AdminKick));
	deleter_.addRule(
		new ConsoleRuleMethodIAdapterEx<ClientAdmin>(console,
			this, &ClientAdmin::adminOneParam, "admin", 
			ConsoleUtil::formParams(ConsoleRuleParam("kill"), 
			ConsoleRuleParam("player", ConsoleRuleTypeNumber)),
			(unsigned int) ComsAdminMessage::AdminKill));
	deleter_.addRule(
		new ConsoleRuleMethodIAdapterEx<ClientAdmin>(console,
			this, &ClientAdmin::adminOneParam, "admin", 
			ConsoleUtil::formParams(ConsoleRuleParam("ban"), 
			ConsoleRuleParam("player", ConsoleRuleTypeNumber)),
			(unsigned int) ComsAdminMessage::AdminBan));
	deleter_.addRule(
		new ConsoleRuleMethodIAdapterEx<ClientAdmin>(console,
			this, &ClientAdmin::adminOneParam, "admin", 
			ConsoleUtil::formParams(ConsoleRuleParam("flag"), 
			ConsoleRuleParam("player", ConsoleRuleTypeNumber)),
			(unsigned int) ComsAdminMessage::AdminFlag));
	deleter_.addRule(
		new ConsoleRuleMethodIAdapterEx<ClientAdmin>(console,
			this, &ClientAdmin::adminOneParam, "admin", 
			ConsoleUtil::formParams(ConsoleRuleParam("mute"), 
			ConsoleRuleParam("player", ConsoleRuleTypeNumber)),
			(unsigned int) ComsAdminMessage::AdminMute));
	deleter_.addRule(
		new ConsoleRuleMethodIAdapterEx<ClientAdmin>(console,
			this, &ClientAdmin::adminOneParam, "admin", 
			ConsoleUtil::formParams(ConsoleRuleParam("permmute"), 
			ConsoleRuleParam("player", ConsoleRuleTypeNumber)),
			(unsigned int) ComsAdminMessage::AdminPermMute));
	deleter_.addRule(
		new ConsoleRuleMethodIAdapterEx<ClientAdmin>(console,
			this, &ClientAdmin::adminOneParam, "admin", 
			ConsoleUtil::formParams(ConsoleRuleParam("unpermmute"), 
			ConsoleRuleParam("player", ConsoleRuleTypeNumber)),
			(unsigned int) ComsAdminMessage::AdminUnPermMute));
	deleter_.addRule(
		new ConsoleRuleMethodIAdapterEx<ClientAdmin>(console,
			this, &ClientAdmin::adminOneParam, "admin", 
			ConsoleUtil::formParams(ConsoleRuleParam("unmute"), 
			ConsoleRuleParam("player", ConsoleRuleTypeNumber)),
			(unsigned int) ComsAdminMessage::AdminUnMute));
	deleter_.addRule(
		new ConsoleRuleMethodIAdapterEx<ClientAdmin>(console,
			this, &ClientAdmin::adminOneParam, "admin", 
			ConsoleUtil::formParams(ConsoleRuleParam("poor"), 
			ConsoleRuleParam("player", ConsoleRuleTypeNumber)),
			(unsigned int) ComsAdminMessage::AdminPoor));

	// One String Param
	deleter_.addRule(
		new ConsoleRuleMethodIAdapterEx<ClientAdmin>(console,
			this, &ClientAdmin::adminOneParam, "admin", 
			ConsoleUtil::formParams(ConsoleRuleParam("talk"), 
			ConsoleRuleParam("text", ConsoleRuleTypeString)),
			(unsigned int) ComsAdminMessage::AdminTalk));
	deleter_.addRule(
		new ConsoleRuleMethodIAdapterEx<ClientAdmin>(console,
			this, &ClientAdmin::adminOneParam, "admin", 
			ConsoleUtil::formParams(ConsoleRuleParam("admintalk"), 
			ConsoleRuleParam("text", ConsoleRuleTypeString)),
			(unsigned int) ComsAdminMessage::AdminAdminTalk));
	deleter_.addRule(
		new ConsoleRuleMethodIAdapterEx<ClientAdmin>(console,
			this, &ClientAdmin::adminOneParam, "admin", 
			ConsoleUtil::formParams(ConsoleRuleParam("message"), 
			ConsoleRuleParam("text", ConsoleRuleTypeString)),
			(unsigned int) ComsAdminMessage::AdminMessage));
	deleter_.addRule(
		new ConsoleRuleMethodIAdapterEx<ClientAdmin>(console,
			this, &ClientAdmin::adminOneParam, "admin", 
			ConsoleUtil::formParams(ConsoleRuleParam("add"), 
			ConsoleRuleParam("playertype", ConsoleRuleTypeString)),
			(unsigned int) ComsAdminMessage::AdminAdd));

	// Two Number Params
	deleter_.addRule(
		new ConsoleRuleMethodIAdapterEx<ClientAdmin>(console,
			this, &ClientAdmin::adminTwoParam, "admin", 
			ConsoleUtil::formParams(ConsoleRuleParam("slap"), 
			ConsoleRuleParam("player", ConsoleRuleTypeNumber),
			ConsoleRuleParam("amount", ConsoleRuleTypeNumber)),
			(unsigned int) ComsAdminMessage::AdminSlap));
	deleter_.addRule(
		new ConsoleRuleMethodIAdapterEx<ClientAdmin>(console,
			this, &ClientAdmin::adminTwoParam, "admin", 
			ConsoleUtil::formParams(ConsoleRuleParam("changename"), 
			ConsoleRuleParam("player", ConsoleRuleTypeNumber),
			ConsoleRuleParam("name", ConsoleRuleTypeString)),
			(unsigned int) ComsAdminMessage::AdminChangeName));

	// Two String Params
	deleter_.addRule(
		new ConsoleRuleMethodIAdapterEx<ClientAdmin>(console,
			this, &ClientAdmin::adminTwoParam, "admin", 
			ConsoleUtil::formParams(ConsoleRuleParam("login"), 
			ConsoleRuleParam("username", ConsoleRuleTypeString),
			ConsoleRuleParam("password", ConsoleRuleTypeString)),
			(unsigned int) ComsAdminMessage::AdminLogin));
	deleter_.addRule(
		new ConsoleRuleMethodIAdapterEx<ClientAdmin>(console,
			this, &ClientAdmin::adminNoParams, "admin", 
			ConsoleUtil::formParams(ConsoleRuleParam("login")),
			(unsigned int) ComsAdminMessage::AdminLogin));
}

ClientAdmin::~ClientAdmin()
{

}

void ClientAdmin::adminNoParams(std::vector<ConsoleRuleValue> &values, 
	unsigned int userData)
{
	ComsAdminMessage::ComsAdminMessageType type = 
		(ComsAdminMessage::ComsAdminMessageType) userData;

	unsigned int sid = ScorchedClient::instance()->getClientHandlers().
		getClientAdminResultHandler().getSid();
	ComsAdminMessage message(sid, type);
	ComsMessageSender::sendToServer(message);

	if (type == ComsAdminMessage::AdminLogout)
	{
		ScorchedClient::instance()->getClientChannelManager().removeChannel("admin");
	}
	else if (type == ComsAdminMessage::AdminLogin)
	{
		ScorchedClient::instance()->getClientChannelManager().addChannel("general", "admin");
	}
}

void ClientAdmin::adminOneParam(std::vector<ConsoleRuleValue> &values, 
	unsigned int userData)
{
	ComsAdminMessage::ComsAdminMessageType type = 
		(ComsAdminMessage::ComsAdminMessageType) userData;

	ConsoleRuleValue param = values[2];

	unsigned int sid = ScorchedClient::instance()->getClientHandlers().
		getClientAdminResultHandler().getSid();
	ComsAdminMessage message(sid, type, param.valueString.c_str());
	ComsMessageSender::sendToServer(message);
}

void ClientAdmin::adminTwoParam(std::vector<ConsoleRuleValue> &values, 
	unsigned int userData)
{
	ComsAdminMessage::ComsAdminMessageType type = 
		(ComsAdminMessage::ComsAdminMessageType) userData;

	ConsoleRuleValue param1 = values[2];
	ConsoleRuleValue param2 = values[3];

	unsigned int sid = ScorchedClient::instance()->getClientHandlers().
		getClientAdminResultHandler().getSid();
	ComsAdminMessage message(sid, type, 
		param1.valueString.c_str(), param2.valueString.c_str());
	ComsMessageSender::sendToServer(message);

	if (type == ComsAdminMessage::AdminLogin)
	{
		ScorchedClient::instance()->getClientChannelManager().addChannel("general", "admin");
	}
}

void ClientAdmin::adminHelp()
{
	ScorchedClient::instance()->getConsole().addLine(false, "  help - This help");
	ScorchedClient::instance()->getConsole().addLine(false, "  login <username> <password> - Login as admin");
	ScorchedClient::instance()->getConsole().addLine(false, "  logout - Logoff as admin");
	ScorchedClient::instance()->getConsole().addLine(false, "  show - Show ids for all current players");
	ScorchedClient::instance()->getConsole().addLine(false, "  showbanned - Shows all banned/perm muted players");
	ScorchedClient::instance()->getConsole().addLine(false, "  killall - Kills all current players and starts next round");
	ScorchedClient::instance()->getConsole().addLine(false, "  newgame - Kills all current players and starts new game");	
	ScorchedClient::instance()->getConsole().addLine(false, "  kick <player id> - Kicks specified player");
	ScorchedClient::instance()->getConsole().addLine(false, "  ban <player id> - Bans and kicks specified player");
	ScorchedClient::instance()->getConsole().addLine(false, "  poor <player id> - Removes all money from player");
	ScorchedClient::instance()->getConsole().addLine(false, "  kill <player id> - Kill specified player");
	ScorchedClient::instance()->getConsole().addLine(false, "  mute <player id> - Mutes specified player for everyone");
	ScorchedClient::instance()->getConsole().addLine(false, "  flag <player id> - Flags specified player for all admins");
	ScorchedClient::instance()->getConsole().addLine(false, "  unmute <player id> - Un-mutes specified player for everyone");
	ScorchedClient::instance()->getConsole().addLine(false, "  permmute <player id> - Mutes specified player for everyone perminantly");
	ScorchedClient::instance()->getConsole().addLine(false, "  unpermmute <player id> - Un-Mutes specified player for everyone perminantly");
	ScorchedClient::instance()->getConsole().addLine(false, "  slap <player id> <health> - Removes health from specified player");
	ScorchedClient::instance()->getConsole().addLine(false, "  talk <text> - Admin talk to all players (white with no name)");
	ScorchedClient::instance()->getConsole().addLine(false, "  admintalk <text> - Admin talk to all admin players only");
	ScorchedClient::instance()->getConsole().addLine(false, "  message <text> - Message to all players (yellow in center of screen)");
	ScorchedClient::instance()->getConsole().addLine(false, "  synccheck - Check client landscape is in sync with server");
}
