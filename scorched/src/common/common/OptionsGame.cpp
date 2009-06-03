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

#include <common/OptionsGame.h>
#include <common/Defines.h>
#include <string.h>

static OptionEntryEnum::EnumEntry scoreEnum[] =
{
	{ "ScoreWins", 0 },
	{ "ScoreKills", 1 },
	{ "ScoreMoney", 2 },
	{ "", -1 }
};

static OptionEntryEnum::EnumEntry turnEnum[] =
{
	{ "TurnSimultaneous", 0 },
	{ "TurnSequentialLooserFirst", 1 },
	{ "TurnSequentialRandom", 2 },
	{ "", -1 }
};

static OptionEntryEnum::EnumEntry windForceEnum[] =
{
	{ "WindRandom", 0 },
	{ "WindNone", 1 },
	{ "Wind1", 2 },
	{ "Wind2", 3 },
	{ "Wind3", 4 },
	{ "Wind4", 5 },
	{ "Wind5", 6 },
	{ "WindBreezy", 7 },
	{ "WindGale", 8 },
	{ "", -1 }
};

static OptionEntryEnum::EnumEntry windTypeEnum[] =
{
	{ "WindOnRound", 0 },
	{ "WindOnMove", 1 },
	{ "", -1 }
};

static OptionEntryEnum::EnumEntry wallEnum[] =
{
	{ "WallRandom", 0 },
	{ "WallConcrete", 1 },
	{ "WallBouncy", 2 },
	{ "WallWrapAround", 3 },
	{ "WallNone", 4 },
	{ "WallActive", 5 },
	{ "WallInactive", 6},
	{ "", -1 }
};

static OptionEntryEnum::EnumEntry weapScaleEnum[] =
{
	{ "ScaleSmall", 0 },
	{ "ScaleMedium", 1 },
	{ "ScaleLarge", 2 },
	{ "", -1 }
};

static OptionEntryEnum::EnumEntry resignEnum[] =
{
	{ "ResignStart", 0 },
	{ "ResignEnd", 1 },
	{ "ResignDueToHealth", 2 },
	{ "", -1 }
};

static OptionEntryEnum::EnumEntry movementRestrictionEnum[] =
{
	{ "MovementRestrictionNone", 0 },
	{ "MovementRestrictionLand", 1 },
	{ "MovementRestrictionLandOrAbove", 2 },
	{ "", -1 }
};

static OptionEntryEnum::EnumEntry teamBallanceEnum[] =
{
	{ "TeamBallanceNone", 0 },
	{ "TeamBallanceAuto", 1 },
	{ "TeamBallanceBotsVs", 2 },
	{ "TeamBallanceAutoByScore", 3 },
	{ "TeamBallanceAutoByBots", 4 },
	{ "", -1 }
};

static OptionEntryStringEnum::EnumEntry economyEnum[] =
{
	{ "EconomyFreeMarket" },
	{ "EconomyNone" },
	{ "" }
};

static OptionEntryStringEnum::EnumEntry serverFileLoggerEnum[] =
{
	{ "none" },
	{ "file" },
	{ "" }
};

static OptionEntryStringEnum::EnumEntry statsLoggerEnum[] =
{
	{ "none" },
	{ "file" },
	{ "mysql" },
	{ "" }
};

static OptionEntryStringEnum::EnumEntry authHandlerEnum[] =
{
	{ "none" },
	{ "prefered" },
	{ "forumlogin" },
	{ "minkills" },
	{ "" }
};

OptionsGame::OptionsGame() :
	tutorial_(options_, "Tutorial",
		"The tutorial to load for this game", 0, ""),
	scorePerMoney_(options_, "ScorePerMoney",
		"The amount of score awarded for 100 money units earned", 0, 0, 0, 100, 2),
	scorePerAssist_(options_, "ScorePerAssist",
		"The amount of score awarded for each kill assist", 0, 2, 0, 50, 1),
	scorePerKill_(options_, "ScorePerKill",
		"The amount of score awarded for each kill", 0, 10, 0, 50, 1),
	scoreWonForRound_(options_, "ScoreWonForRound",
		"The amount of score awarded for round won", 0, 250, 0, 1000, 50),
	scoreWonForLives_(options_, "ScoreWonForLives",
		"The amount of score awarded for lives left", 0, 0, 0, 1000, 50),
	skillForRound_(options_, "SkillForRound",
		"The amount of skill awarded for winning the round", 0, 0, 0, 100, 5),
	skillForMatch_(options_, "SkillForMatch",
		"The amount of skill awarded for winning the match", 0, 0, 0, 100, 5),
	maxSkillLost_(options_, "MaxSkillLost",
		"The maximum amount of skill you can lose for being killed", 0, 10, 0, 100, 5),
	skillForSelfKill_(options_, "SkillForSelfKill",
		"The amount of skill you lose for being killed by yourself", 0, -25, -50, 0, 5),
	skillForTeamKill_(options_, "SkillForTeamKill",
		"The amount of skill you lose for killing a team memeber", 0, -25, -50, 0, 5),
	teams_(options_, "Teams",
		"The number of teams (1 == no teams)", 0, 1, 1, 4, 1),
	startArmsLevel_(options_, "StartArmsLevel",
		"The largest weapon type allowed (on round 1)", 0, 10, 0, 10, 1),
	endArmsLevel_(options_, "EndArmsLevel",
		"The largest weapon type allowed (on the last round)", 0, 10, 0, 10, 1),
	maxNumberWeapons_(options_, "MaxNumberWeapons",
		"The number of each weapon the player is allowed", 0, 90, 0, 500, 10),
	maxLandscapeSize_(options_, "MaxLandscapeSize",
		"The largest landscape (in bytes) that will be sent to the clients", 0, 200000),
	startTime_(options_, "StartTime", 
		"The amount of time before a new game starts", 0, 0, 0, 90, 5),
	shotTime_(options_, "ShotTime", 
		"The amount of time each player has for each shot", 0, 0, 0, 90, 5),
	buyingTime_(options_, "BuyingTime", 
		"The amount of time each player has to buy weapons and use auto defense", 0, 0, 0, 90, 5),
	allowedMissedMoves_(options_, "AllowedMissedMoves",
		"The number of moves a player is allowed to miss (due to the shot timer)", 0, 3, 0, 10, 1),
	roundScoreTime_(options_, "RoundScoreTime", 
		"The amount of time to show the end of each round for", 0, 5, 0, 30, 1),
	scoreTime_(options_, "ScoreTime", 
		"The amount of time to show the end of match scores for", 0, 15, 0, 90, 5),
	depricatedKeepAliveTime_(options_, "KeepAliveTime",
		"The amount of time between each client keep alive message", OptionEntry::DataDepricated, 2, 0, 30, 1),
	depricatedKeepAliveTimeoutTime_(options_, "KeepAliveTimeoutTIme",
		"The amount of time the server will allow without receiving a keep alive message", OptionEntry::DataDepricated, 0, 0, 90, 5),
	idleKickTime_(options_, "IdleKickTime",
		"The amount of time to give clients to respond after level loading before kicking them", 0, 60, 0, 90, 5),
	idleShotKickTime_(options_, "IdleShotKickTime",
		"The amount of time to give clients to respond after shots before kicking them", 0, 45, 0, 90, 5),
	minFallingDistance_(options_, "MinFallingDistance",
		"The minimum distance that can be made with no damage (value is divided by 10)", 0, 5, 0, 100, 5),
	maxClimbingDistance_(options_, "MaxClimbingDistance",
		"The maximum distance that a tank can climb per movement square (divided by 10)", 0, 8, 0, 20, 1),
	playerLives_(options_, "PlayerLives",
		"The number of lives that each player tank has", 0, 1, 0, 10, 1),
	gravity_(options_, "Gravity",
		"The gravity used by the physics engine", 0, -10, -25, 0, 1),
	maxRoundTurns_(options_, "MaxNumberOfRoundTurns",
		"The maximum number of turns all players are allowed in each round (0 = infinite)", 0, 15, 0, 50, 1),
	numberOfRounds_(options_, "NumberOfRounds", 
		"The number of rounds to play in each game", 0, 5, 1, 50, 1),
	numberOfPlayers_(options_, "NumberOfPlayers", 
		"The maximum number of players to allow", 0, 2, 2, 24, 1),
	numberOfMinPlayers_(options_, "NumberOfMinPlayers", 
		"The minimum number of players to allow", 0, 2, 2, 24, 1),
	removeBotsAtPlayers_(options_, "RemoveBotsAtPlayers",
		"The number of players to allow before remvoing bots", 0, 0, 0, 24, 1),
	limitPowerByHealth_(options_, "LimitPowerByHealth", 
		"Show power is limited by the amount of health a player has", 0, true),
	residualPlayers_(options_, "ResidualPlayers",
		"Players that leave will have the same state when reconnecting", 0, true),
	autoSendSyncCheck_(options_, "AutoSendSyncCheck",
		"Automatically send a sync check at the end of each shot", 0, false),
	actionSyncCheck_(options_, "ActionSyncCheck",
		"Gather enhanced action syncchecking", 0, false),
	resignMode_(options_, "ResignMode",
		"When does a players resign take place", 0, int(ResignEnd), resignEnum),
	movementRestriction_(options_, "MovementRestriction",
		"Where a tank is allowed to move to", 0, int(MovementRestrictionNone), movementRestrictionEnum),
	teamBallance_(options_, "TeamBallance",
		"The mode of team auto-ballancing performed for team games", 0, int(TeamBallanceNone), teamBallanceEnum),
	turnType_(options_, "TurnType", 
		"The player turn mode", 0, 	int(TurnSequentialLooserFirst), turnEnum), // Data, default, min, max
	moneyBuyOnRound_(options_, "MoneyBuyOnRound", 
		"The first round players are allowed to buy on", 0 ,2, 1, 50, 1),
	moneyWonForRound_(options_, "MoneyWonForRound", 
		"The money awarded for each won round", 0, 5000, 0, 100000, 1000),
	moneyWonForLives_(options_, "MoneyWonForLives",
		"The money awarded for each life remaining", 0, 5000, 0, 100000, 1000),
	moneyPerKillPoint_(options_, "MoneyWonPerKillPoint", 
		"The money awarded for each person killed * weapon used", 0, 750, 0, 2000, 50),
	moneyPerMultiKillPoint_(options_, "MoneyWonPerMultiKillPoint", 
		"The extra money awarded for each person multi-killed * weapon used * multi-kill", 0, 0, 0, 2000, 50),
	moneyPerAssistPoint_(options_, "MoneyWonPerAssistPoint", 
		"The money awarded for each kill assist * weapon used", 0, 250, 0, 2000, 50),
	moneyPerHitPoint_(options_, "MoneyWonPerHitPoint", 
		"The money awarded for each person hit * weapon used", 0, 250, 0, 2000, 50),
	moneyPerRound_(options_, "MoneyPerRound", 
		"The money given at the end of each round", 0, 0, 0, 250000, 5000),
	moneyPerHealthPoint_(options_, "MoneyPerHealthPoint",
		"The money awarded is proportional to the amount of health removed", 0, true),
	moneyStarting_(options_, "MoneyStarting", 
		"The money each player starts with", 0, 10000, 0, 500000, 10000),
	moneyInterest_(options_, "MoneyInterest", 
		"The interest awarded at the end of each round", 0, 15, 0, 100, 5),
	freeMarketAdjustment_(options_, "FreeMarketAdjustment",
		"The scale of the adjustment changes made by the free market", 0, 100),
	freeMarketLimits_(options_, "FreeMarketLimits",
		"The scale of the max/min prices (*1.5) allowed by the free market e.g. 2 is a 300% increase/reduction", 0, 2),
	windForce_(options_, "WindForce", 
		"The force of the wind", 0, int(WindRandom), windForceEnum),
	windType_(options_, "WindType", 
		"When the wind changes", 0, int(WindOnRound), windTypeEnum),
	wallType_(options_, "WallType", 
		"The type of walls allowed", 0, int(WallRandom), wallEnum),
	weapScale_(options_, "WeaponScale", 
		"The scale of the weapons used", 0, int(ScaleMedium), weapScaleEnum),
	statsLogger_(options_, "StatsLogger",
		"The type of player stats to be logged", 0, "none", statsLoggerEnum),
	serverFileLogger_(options_, "ServerLogger",
	"The type of server events to be logged to file", 0, "none", serverFileLoggerEnum),
	portNo_(options_, "PortNo", 
		"The port to start the server on", 0, S3D::ScorchedPort),
	managementPortNo_(options_, "ManagementPortNo", 
		"The port to start the managament web server on (0 = management off)", 0, 0),
	serverName_(options_, "ServerName", 
		"The name of the server to start", 0, "No Name"),
	mod_(options_, "Mod", 
		"The name of currently running Scorched3D mod", 0, "none"),
	motd_(options_, "MOTD",
		"The message of the day", 0, 
		"Scorched3D : Copyright 2004 Gavin Camp\n"
		"For updates and news visit:\n"
		"http://www.scorched3d.co.uk",
		true),
	modDownloadSpeed_(options_, "ModDownloadSpeed",
		"Max download speed of mods in bytes per second (0 == no mod download)", 0, 0, 0, 500000, 5000),
	maxAvatarSize_(options_, "MaxAvatarSize",
		"Max size allowed for avatars in bytes (0 == no avatars)", 0, 5000),
	economy_(options_, "Economy", 
		"Speicifies the name of the economy to use", 0, "EconomyFreeMarket", economyEnum),
	landscapes_(options_, "Landscapes", 
		"Colon seperated list of landscape names", 0, ""),
	serverPassword_(options_, "ServerPassword", 
		"The password for this server (empty password = no password)", OptionEntry::DataProtected, ""),
	allowSameIP_(options_, "AllowSameIP",
		"Allow scorched clients from same machine/NAT router to connect.", 0, true),
	allowSameUniqueId_(options_, "AllowSameUniqueId",
		"Allow scorched clients with same unique id to connect.", 0, true),
	publishServer_(options_, "PublishServer",
		"Allow other scorched net clients to see this server.  Do not use for LAN games.", 0, false),
	publishAddress_(options_, "PublishAddress",
		"IP address to publish to scorched net clients (auto-detected if not given).", 0, "AutoDetect"),
	botNamePrefix_(options_, "BotNamePrefix", 
		"Prepend and bot name with the specified text", 0, "(Bot) "),
	giveAllWeapons_(options_, "GiveAllWeapons",
		"Start the game will all the weapons", 0, false),
	registeredUserNames_(options_, "RegisteredUserNames",
		"Only allow authenticated players to use their own player names", 0, false),
	allowMultiLingualChat_(options_, "AllowMultiLingualChat",
		"Allow internaltional (non-latin) characters in chat", 0, true),
	allowMultiLingualNames_(options_, "AllowMultiLingualNames",
		"Allow internaltional (non-latin) characters in names", 0, true),
	authHandler_(options_, "AuthHandler",
		"Only allow authenticated players to connect", 0, "none", authHandlerEnum),
	cycleMaps_(options_, "CycleMaps",
		"Cycle through the maps instead of choosing them using a random probablity", 0, false),
	delayedDefenseActivation_(options_, "DelayedDefenseActivation",
		"Changes to shields and parachutes are only seen after the aiming phase", 0, false),
	randomizeBotNames_(options_, "RandomizeBotNames",
		"Choose random bot names instread of sequential names", 0, false),
	computersDeathTalk_(options_, "ComputersDeathTalk",
		"The percentage chance the computers will say something when killed", 0, 100, 0, 100, 10),
	computersAttackTalk_(options_, "ComputersAttackTalk",
		"The percentage chance the computers will say something when shooting", 0, 25, 0, 100, 10),
	debugFeatures_(options_, "DebugFeatures",
		"Set to \"true\" to enable debugging features such as InfoGrid", 0, false),


	// Deprecated options
	depricatedMasterListServer_(options_, "MasterListServer",
		"The master list server for scorched3d", OptionEntry::DataDepricated, "scorched3d.sourceforge.net"),
	depricatedMasterListServerURI_(options_, "MasterListServerURI",
		"The URI on the master list server for scorched3d", OptionEntry::DataDepricated, "/scorched"),
	depricatedServerAdminPassword_(options_, "ServerAdminPassword", 
		"The admin password for this server (empty password = no access)", OptionEntry::DataDepricated, ""),
	depricatedMaxArmsLevel_(options_, "MaxArmsLevel",
		"", OptionEntry::DataDepricated, 10, 0, 10, 1),
	depricatedAutoBallanceTeams_(options_, "AutoBallanceTeams",
		"", OptionEntry::DataDepricated, true),
	depricatedScoreType_(options_, "ScoreType",
		"How the winnder is choosen", OptionEntry::DataDepricated, 0, scoreEnum)
{
	char buffer[128];
	for (int i=0; i<24; i++)
	{
		snprintf(buffer, 128, "PlayerType%i", i+1);
		playerType_[i] = new OptionEntryString(playerTypeOptions_,
			buffer,
			"The type of the player e.g. human, computer etc..", 0,
			"Human");
	}
}

OptionsGame::~OptionsGame()
{
	
}

bool OptionsGame::writeToBuffer(NetBuffer &buffer,
	bool useProtected,
	bool usePlayerTypes)
{
	std::list<OptionEntry *> saveOptions;
	if (usePlayerTypes) saveOptions = playerTypeOptions_;
	std::list<OptionEntry *>::iterator itor;
	for (itor = options_.begin();
		itor != options_.end();
		itor++)
	{
		OptionEntry *entry = *itor;
		saveOptions.push_back(entry);
	}

	if (!OptionEntryHelper::writeToBuffer(
		saveOptions, buffer, useProtected)) return false;
	return true;
}

bool OptionsGame::readFromBuffer(NetBufferReader &reader,
	bool useProtected,
	bool usePlayerTypes)
{
	std::list<OptionEntry *> saveOptions;
	if (usePlayerTypes) saveOptions = playerTypeOptions_;
	std::list<OptionEntry *>::iterator itor;
	for (itor = options_.begin();
		itor != options_.end();
		itor++)
	{
		OptionEntry *entry = *itor;
		saveOptions.push_back(entry);
	}

	if (!OptionEntryHelper::readFromBuffer(
		saveOptions, reader, useProtected)) return false;
	return true;
}

bool OptionsGame::writeOptionsToXML(XMLNode *node)
{
	if (!OptionEntryHelper::writeToXML(options_, node)) return false;
	return true;
}

bool OptionsGame::readOptionsFromXML(XMLNode *node)
{
	if (!OptionEntryHelper::readFromXML(options_, node)) return false;
	return true;
}

bool OptionsGame::writeOptionsToFile(const std::string &filePath)
{
	std::list<OptionEntry *> saveOptions = 
		playerTypeOptions_; // Note: The players are also saved
	std::list<OptionEntry *>::iterator itor;
	for (itor = options_.begin();
		itor != options_.end();
		itor++)
	{
		OptionEntry *entry = *itor;
		saveOptions.push_back(entry);
	} 

	if (!OptionEntryHelper::writeToFile(saveOptions, filePath)) return false;
	return true;
}

bool OptionsGame::readOptionsFromFile(const std::string &filePath)
{
	std::list<OptionEntry *> saveOptions = 
		playerTypeOptions_; // Note: The players are also saved
        std::list<OptionEntry *>::iterator itor;
        for (itor = options_.begin();
                itor != options_.end();
                itor++)
        {
                OptionEntry *entry = *itor;
                saveOptions.push_back(entry);
        } 

	if (!OptionEntryHelper::readFromFile(saveOptions, filePath)) return false;
	return true;
}

std::list<OptionEntry *> &OptionsGame::getPlayerTypeOptions()
{
	return playerTypeOptions_;
}

std::list<OptionEntry *> &OptionsGame::getOptions()
{
	return options_;
}
