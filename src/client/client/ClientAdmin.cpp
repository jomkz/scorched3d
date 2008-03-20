////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
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

#include <client/ClientAdmin.h>
#include <client/ScorchedClient.h>
#include <client/ClientChannelManager.h>
#include <tank/TankContainer.h>
#include <console/Console.h>
#include <coms/ComsMessageSender.h>
#include <coms/ComsAdminMessage.h>
#include <common/Defines.h>

ClientAdmin *ClientAdmin::instance_ = 0;

ClientAdmin *ClientAdmin::instance()
{
	if (!instance_)
	{
		instance_ = new ClientAdmin();
	}
	return instance_;
}

ClientAdmin::ClientAdmin() 
{
	new ConsoleRuleMethodIAdapterEx2<ClientAdmin>(
		this, &ClientAdmin::admin, "admin");
}

ClientAdmin::~ClientAdmin()
{

}

void ClientAdmin::admin(std::list<ConsoleRuleSplit> split, 
	std::list<std::string> &result)
{
	split.pop_front();
	if (split.empty()) adminHelp(result);
	else
	{
		ConsoleRuleSplit firstsplit = split.front();
		split.pop_front();

		bool failed = false;
		if (0 == stricmp(firstsplit.rule.c_str(), "help")) adminHelp(result);
		else if (0 == stricmp(firstsplit.rule.c_str(), "killall") ||
			0 == stricmp(firstsplit.rule.c_str(), "newgame") ||
			0 == stricmp(firstsplit.rule.c_str(), "showbanned") ||
			0 == stricmp(firstsplit.rule.c_str(), "show") ||
			0 == stricmp(firstsplit.rule.c_str(), "logout") ||
			0 == stricmp(firstsplit.rule.c_str(), "synccheck"))
		{
			result.push_back(S3D::formatStringBuffer("  sending %s...", 
				firstsplit.rule.c_str()));

			ComsAdminMessage::ComsAdminMessageType type = 
				ComsAdminMessage::AdminShow;
			if (0 == stricmp(firstsplit.rule.c_str(), "show"))
				type = ComsAdminMessage::AdminShow;
			else if (0 == stricmp(firstsplit.rule.c_str(), "showbanned"))
				type = ComsAdminMessage::AdminShowBanned;
			else if (0 == stricmp(firstsplit.rule.c_str(), "killall"))
				type = ComsAdminMessage::AdminKillAll;
			else if (0 == stricmp(firstsplit.rule.c_str(), "newgame"))
				type = ComsAdminMessage::AdminNewGame;
			else if (0 == stricmp(firstsplit.rule.c_str(), "logout"))
				type = ComsAdminMessage::AdminLogout;
			else if (0 == stricmp(firstsplit.rule.c_str(), "synccheck"))
				type = ComsAdminMessage::AdminSyncCheck;

			ComsAdminMessage message(type);
			ComsMessageSender::sendToServer(message);

			if (type == ComsAdminMessage::AdminLogout)
			{
				ClientChannelManager::instance()->removeChannel("admin");
			}
		}
		else if (
			0 == stricmp(firstsplit.rule.c_str(), "kick") ||
			0 == stricmp(firstsplit.rule.c_str(), "ban") ||
			0 == stricmp(firstsplit.rule.c_str(), "flag") ||
			0 == stricmp(firstsplit.rule.c_str(), "mute") ||
			0 == stricmp(firstsplit.rule.c_str(), "permmute") ||
			0 == stricmp(firstsplit.rule.c_str(), "unpermmute") ||
			0 == stricmp(firstsplit.rule.c_str(), "unmute") ||
			0 == stricmp(firstsplit.rule.c_str(), "talk") ||
			0 == stricmp(firstsplit.rule.c_str(), "poor") ||
			0 == stricmp(firstsplit.rule.c_str(), "admintalk") ||
			0 == stricmp(firstsplit.rule.c_str(), "message"))
		{
			if (split.empty()) failed = true;
			else
			{
				ConsoleRuleSplit secondsplit = split.front();
				result.push_back(S3D::formatStringBuffer("  sending %s %s...", 
					firstsplit.rule.c_str(),
					secondsplit.rule.c_str()));
					
				ComsAdminMessage::ComsAdminMessageType type = 
					ComsAdminMessage::AdminKick;
				if (0 == stricmp(firstsplit.rule.c_str(), "kick"))
					type = ComsAdminMessage::AdminKick;
				else if (0 == stricmp(firstsplit.rule.c_str(), "ban"))
					type = ComsAdminMessage::AdminBan;
				else if (0 == stricmp(firstsplit.rule.c_str(), "flag"))
					type = ComsAdminMessage::AdminFlag;
				else if (0 == stricmp(firstsplit.rule.c_str(), "mute"))
					type = ComsAdminMessage::AdminMute;
				else if (0 == stricmp(firstsplit.rule.c_str(), "permmute"))
					type = ComsAdminMessage::AdminPermMute;
				else if (0 == stricmp(firstsplit.rule.c_str(), "unpermmute"))
					type = ComsAdminMessage::AdminUnPermMute;
				else if (0 == stricmp(firstsplit.rule.c_str(), "unmute"))
					type = ComsAdminMessage::AdminUnMute;	
				else if (0 == stricmp(firstsplit.rule.c_str(), "talk"))
					type = ComsAdminMessage::AdminTalk;	
				else if (0 == stricmp(firstsplit.rule.c_str(), "poor"))
					type = ComsAdminMessage::AdminPoor;	
				else if (0 == stricmp(firstsplit.rule.c_str(), "admintalk"))
					type = ComsAdminMessage::AdminAdminTalk;	
				else if (0 == stricmp(firstsplit.rule.c_str(), "message"))
					type = ComsAdminMessage::AdminMessage;	

				ComsAdminMessage message(type, secondsplit.rule.c_str());
				ComsMessageSender::sendToServer(message);
			}
		}
		else if (0 == stricmp(firstsplit.rule.c_str(), "slap") ||
			0 == stricmp(firstsplit.rule.c_str(), "login"))
		{
			if (split.empty()) failed = true;
			else
			{
				ConsoleRuleSplit secondsplit = split.front();
				split.pop_front();
				if (split.empty()) failed = true;
				else
				{
					ConsoleRuleSplit thirdsplit = split.front();
					result.push_back(S3D::formatStringBuffer("  sending %s %s %s...", 
						firstsplit.rule.c_str(),
						secondsplit.rule.c_str(),
						thirdsplit.rule.c_str()));
					
					ComsAdminMessage::ComsAdminMessageType type = 
						ComsAdminMessage::AdminLogin;
					if (0 == stricmp(firstsplit.rule.c_str(), "slap"))
						type = ComsAdminMessage::AdminSlap;
					ComsAdminMessage message(type, 
						secondsplit.rule.c_str(), 
						thirdsplit.rule.c_str());
					ComsMessageSender::sendToServer(message);

					if (type == ComsAdminMessage::AdminLogin)
					{
						ClientChannelManager::instance()->addChannel("general", "admin");
					}
				}
			}
		}
		else failed = true;

		if (failed)
		{
			std::string failed;
			ConsoleRule::addRuleFail(failed, firstsplit.position, 
					(int) firstsplit.rule.length());
			result.push_back(failed);
			result.push_back(std::string("Unrecognised admin function ") + 
				"\"" + firstsplit.rule + "\"");
			adminHelp(result);
		}
	}
}

void ClientAdmin::adminHelp(std::list<std::string> &result)
{
	result.push_back("  help - This help");
	result.push_back("  login <username> <password> - Login as admin");
	result.push_back("  logout - Logoff as admin");
	result.push_back("  show - Show ids for all current players");
	result.push_back("  showbanned - Shows all banned/perm muted players");
	result.push_back("  killall - Kills all current players and starts next round");
	result.push_back("  newgame - Kills all current players and starts new game");	
	result.push_back("  kick <player id> - Kicks specified player");
	result.push_back("  ban <player id> - Bans and kicks specified player");
	result.push_back("  poor <player id> - Removes all money from player");
	result.push_back("  mute <player id> - Mutes specified player for everyone");
	result.push_back("  flag <player id> - Flags specified player for all admins");
	result.push_back("  unmute <player id> - Un-mutes specified player for everyone");
	result.push_back("  permmute <player id> - Mutes specified player for everyone perminantly");
	result.push_back("  unpermmute <player id> - Un-Mutes specified player for everyone perminantly");
	result.push_back("  slap <player id> <health> - Removes health from specified player");
	result.push_back("  talk <text> - Admin talk to all players (white with no name)");
	result.push_back("  admintalk <text> - Admin talk to all admin players only");
	result.push_back("  message <text> - Message to all players (yellow in center of screen)");
	result.push_back("  synccheck - Check client landscape is in sync with server");
}
