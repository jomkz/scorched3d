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

#include <common/OptionsGame.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <string.h>

static XMLEntryEnum::EnumEntry scoreEnum[] =
{
	{ "ScoreWins", OptionsGame::ScoreWins },
	{ "ScoreKills", OptionsGame::ScoreKills },
	{ "ScoreMoney", OptionsGame::ScoreMoney },
	{ "", -1 }
};

static XMLEntryEnum::EnumEntry turnEnum[] =
{
	{ "TurnSimultaneous", OptionsGame::TurnSimultaneous },
	{ "TurnSimultaneousNoWait", OptionsGame::TurnSimultaneousNoWait },
	{ "TurnSequentialLoserFirst", OptionsGame::TurnSequentialLoserFirst },
	{ "TurnSequentialRandom", OptionsGame::TurnSequentialRandom },
	{ "TurnFree", OptionsGame::TurnFree },
	{ "TurnFreeTimed", OptionsGame::TurnFreeTimed },
	{ "", -1 }
};

static XMLEntryEnum::EnumEntry windForceEnum[] =
{
	{ "WindRandom", OptionsGame::WindRandom },
	{ "WindNone", OptionsGame::WindNone },
	{ "Wind1", OptionsGame::Wind1 },
	{ "Wind2", OptionsGame::Wind2 },
	{ "Wind3", OptionsGame::Wind3 },
	{ "Wind4", OptionsGame::Wind4 },
	{ "Wind5", OptionsGame::Wind5 },
	{ "WindBreezy", OptionsGame::WindBreezy },
	{ "WindGale", OptionsGame::WindGale },
	{ "", -1 }
};

static XMLEntryEnum::EnumEntry windTypeEnum[] =
{
	{ "WindChangeNever", OptionsGame::WindChangeNever },
	{ "WindChangeSomeTimes", OptionsGame::WindChangeSomeTimes },
	{ "WindChangeFrequently", OptionsGame::WindChangeFrequently },
	{ "WindChangeConstantly", OptionsGame::WindChangeConstantly },
	{ "WindChangeAlways", OptionsGame::WindChangeAlways },
	{ "", -1 }
};

static XMLEntryEnum::EnumEntry wallEnum[] =
{
	{ "WallRandom", OptionsGame::WallRandom },
	{ "WallConcrete", OptionsGame::WallConcrete },
	{ "WallBouncy", OptionsGame::WallBouncy },
	{ "WallWrapAround", OptionsGame::WallWrapAround },
	{ "WallNone", OptionsGame::WallNone },
	{ "WallActive", OptionsGame::WallActive },
	{ "WallInactive", OptionsGame::WallInactive },
	{ "", -1 }
};

static XMLEntryEnum::EnumEntry weapScaleEnum[] =
{
	{ "ScaleSmall", OptionsGame::ScaleSmall },
	{ "ScaleMedium", OptionsGame::ScaleMedium },
	{ "ScaleLarge", OptionsGame::ScaleLarge },
	{ "", -1 }
};

static XMLEntryEnum::EnumEntry resignEnum[] =
{
	{ "ResignStart", OptionsGame::ResignStart },
	{ "ResignTimed", OptionsGame::ResignTimed },
	{ "ResignNone", OptionsGame::ResignNone },
	{ "", -1 }
};

static XMLEntryEnum::EnumEntry movementRestrictionEnum[] =
{
	{ "MovementRestrictionNone", OptionsGame::MovementRestrictionNone },
	{ "MovementRestrictionLand", OptionsGame::MovementRestrictionLand },
	{ "MovementRestrictionLandOrAbove", OptionsGame::MovementRestrictionLandOrAbove },
	{ "", -1 }
};

static XMLEntryEnum::EnumEntry teamBallanceEnum[] =
{
	{ "TeamBallanceNone", OptionsGame::TeamBallanceNone },
	{ "TeamBallanceAuto", OptionsGame::TeamBallanceAuto },
	{ "TeamBallanceBotsVs", OptionsGame::TeamBallanceBotsVs },
	{ "TeamBallanceAutoByScore", OptionsGame::TeamBallanceAutoByScore },
	{ "TeamBallanceAutoByBots", OptionsGame::TeamBallanceAutoByBots },
	{ "", -1 }
};

static XMLEntryStringEnum::EnumEntry economyEnum[] =
{
	{ "EconomyFreeMarket" },
	{ "EconomyNone" },
	{ "" }
};

static XMLEntryStringEnum::EnumEntry serverFileLoggerEnum[] =
{
	{ "none" },
	{ "file" },
	{ "" }
};

static XMLEntryStringEnum::EnumEntry statsLoggerEnum[] =
{
	{ "none" },
	{ "file" },
	{ "mysql" },
	{ "" }
};

static XMLEntryStringEnum::EnumEntry authHandlerEnum[] =
{
	{ "none" },
	{ "prefered" },
	{ "forumlogin" },
	{ "minkills" },
	{ "" }
};

OptionsGameBase::OptionsGameBase(const char *typeName, const char *description, bool required) :
	XMLEntrySimpleContainer(typeName, description, required),
	tutorial_("The tutorial to load for this game", 0, ""),
	scorePerMoney_("The amount of score awarded for 100 money units earned", 0, 0, 0, 100, 2),
	scorePerAssist_("The amount of score awarded for each kill assist", 0, 2, 0, 50, 1),
	scorePerKill_("The amount of score awarded for each kill", 0, 10, 0, 50, 1),
	scorePerResign_("The amount of score awarded for each resign", 0, 0, -50, 50, 1),
	scorePerSpectate_("The amount of score awarded for each move to spectator (only when tank is still alive)", 0, 0, -10, 50, 1),
	scoreWonForRound_("The amount of score awarded for round won", 0, 250, 0, 500, 10),
	scoreWonForLives_("The amount of score awarded for lives left", 0, 0, 0, 500, 10),
	skillForRound_("The amount of skill awarded for winning the round", 0, 0, 0, 100, 5),
	skillForMatch_("The amount of skill awarded for winning the match", 0, 0, 0, 100, 5),
	skillForResign_("The amount of skill awarded for resigning", 0, 0, -50, 50, 5),
	maxSkillLost_("The maximum amount of skill you can lose for being killed", 0, 10, 0, 100, 5),
	maxSkillGained_("The maximum amount of skill you can gain for killing someone", 0, 10, 0, 100, 5),
	skillForSelfKill_("The amount of skill you lose for being killed by yourself", 0, -25, -50, 0, 5),
	skillForTeamKill_("The amount of skill you lose for killing a team memeber", 0, -25, -50, 0, 5),
	teams_("The number of teams (1 is no teams)", 0, 1, 1, 4, 1),
	minimumLandHeight_("The minimum height the ground can reach", 0, 0),
	startArmsLevel_("The largest weapon type allowed (on round 1)", 0, 10, 0, 10, 1),
	endArmsLevel_("The largest weapon type allowed (on the last round)", 0, 10, 0, 10, 1),
	maxNumberWeapons_("The number of each weapon the player is allowed", 0, 90, 0, 500, 10),
	weaponSpeed_("The speed of projectiles and rollers", 0, 15000, 2500, 40000, 2500),
	maxLandscapeSize_("The largest landscape (in bytes) that will be sent to the clients", 0, 200000),
	startTime_("The amount of time before a new game starts", 0, 0, 0, 90, 5),
	shotTime_("The amount of time each player has for each shot", 0, 0, 0, 90, 5),
	aiShotTime_("The amount of time each computer controller player has for each shot", 0, 10, 0, 90, 2),
	buyingTime_("The amount of time each player has to buy weapons and use auto defense", 0, 0, 0, 90, 5),
	removeTime_("The amount of time each player has before being removed when disconected", 0, 10, 0, 25, 1),
	allowedMissedMoves_("The number of moves a player is allowed to miss (due to the shot timer)", 0, 0, 0, 10, 1),
	roundScoreTime_("The amount of time to show the end of each round for", 0, 5, 0, 30, 1),
	idleCycleTime_("The time before a new map is loaded when no one is playing", 0, 900, 45, 1800, 45),
	scoreTime_("The amount of time to show the end of match scores for", 0, 15, 0, 90, 5),
	roundTime_("The amount of time for each round", 0, 0, 0, 900, 30),
	minFallingDistance_("The minimum distance that can be made with no damage (value is divided by 10)", 0, 5, 0, 100, 5),
	tankFallingDamage_("If tanks tank damage from falling", 0, true),
	maxClimbingDistance_("The maximum distance that a tank can climb per movement square (divided by 10)", 0, 8, 0, 20, 1),
	playerLives_("The number of lives that each player tank has", 0, 1, 0, 10, 1),
	gravity_("The gravity used by the physics engine", 0, -10, -25, 0, 1),
	maxRoundTurns_("The maximum number of turns all players are allowed in each round (0 is infinite)", 0, 15, 0, 50, 1),
	numberOfRounds_("The number of rounds to play in each game", 0, 5, 1, 50, 1),
	numberOfPlayers_("The maximum number of players to allow", 0, 2, 2, 24, 1),
	numberOfMinPlayers_("The minimum number of players to allow", 0, 2, 2, 24, 1),
	removeBotsAtPlayers_("The number of players to allow before remvoing bots", 0, 0, 0, 24, 1),
	limitPowerByHealth_("Show power is limited by the amount of health a player has", 0, true),
	residualPlayers_("Players that leave will have the same state when reconnecting", 0, true),
	delayedDefenseActivation_("Changes to shields are only seen after the aiming phase.", 0, false),
	autoSendSyncCheck_("Automatically send a sync check at the end of each shot", 0, false),
	actionSyncCheck_("Gather enhanced action syncchecking", 0, false),
	actionMovementSyncCheck_("Gather enhanced action syncchecking for target movements", 0, false),
	actionRandomSyncCheck_("Gather enhanced action syncchecking for random number generation", 0, false),
	actionCollisionSyncCheck_("Gather enhanced action syncchecking for particle collisions", 0, false),
	targetPlacementSyncCheck_("Gather enhanced action syncchecking for initial target placements", 0, false),
	weaponSyncCheck_("Gather enhanced action syncchecking for weapon invocations", 0, false),
	accessoryNameSyncCheck_("Also send the accessory names when sending the tank sync check", 0, false),
	resignMode_("When does a players resign take place", 0, int(ResignTimed), resignEnum),
	movementRestriction_("Where a tank is allowed to move to", 0, int(MovementRestrictionNone), movementRestrictionEnum),
	teamBallance_("The mode of team auto-ballancing performed for team games", 0, int(TeamBallanceNone), teamBallanceEnum),
	turnType_("The player turn mode", 0, 	int(TurnSequentialLoserFirst), turnEnum), // Data, default, min, max
	moneyBuyOnRound_("The first round players are allowed to buy on", 0 ,2, 1, 50, 1),
	moneyWonForRound_("The money awarded for each won round", 0, 5000, 0, 100000, 1000),
	moneyWonForLives_("The money awarded for each life remaining", 0, 5000, 0, 100000, 1000),
	moneyPerKillPoint_("The money awarded for each person killed * weapon used", 0, 750, 0, 2000, 50),
	moneyPerMultiKillPoint_("The extra money awarded for each person multi-killed * weapon used * multi-kill", 0, 0, 0, 2000, 50),
	moneyPerAssistPoint_("The money awarded for each kill assist * weapon used", 0, 250, 0, 2000, 50),
	moneyPerHitPoint_("The money awarded for each person hit * weapon used", 0, 250, 0, 2000, 50),
	moneyPerRound_("The money given at the end of each round", 0, 0, 0, 250000, 5000),
	moneyPerHealthPoint_("The money awarded is proportional to the amount of health removed", 0, true),
	moneyStarting_("The money each player starts with", 0, 10000, 0, 500000, 10000),
	moneyInterest_("The interest awarded at the end of each round", 0, 15, 0, 100, 5),
	freeMarketAdjustment_("The scale of the adjustment changes made by the free market", 0, 100),
	freeMarketLimits_("The scale of the max/min prices (*1.5) allowed by the free market e.g. 2 is a 300% increase/reduction", 0, 2),
	freeMarketTracksBotPurchases_("Allows the purchases bots (ais) make to be tracked by the free market", 0, false),
	windForce_("The force of the wind", 0, int(WindRandom), windForceEnum),
	windType_("When the wind changes", 0, int(WindChangeNever), windTypeEnum),
	wallType_("The type of walls allowed", 0, int(WallRandom), wallEnum),
	weapScale_("The scale of the weapons used", 0, int(ScaleMedium), weapScaleEnum),
	statsLogger_("The type of player stats to be logged", 0, "none", statsLoggerEnum),
	serverFileLogger_("The type of server events to be logged to file", 0, "none", serverFileLoggerEnum),
	portNo_("The port to start the server on", 0, S3D::ScorchedPort),
	managementPortNo_("The port to start the managament web server on (0 is management off)", 0, 0),
	serverName_("The name of the server to start", 0, "No Name"),
	mod_("The name of currently running Scorched3D mod", 0, "none"),
	motd_("The message of the day", 0, 
		"Scorched3D : Copyright 2011 Gavin Camp\n"
		"For updates and news visit:\n"
		"http://www.scorched3d.co.uk",
		true),
	modDownloadSpeed_("Max download speed of mods in bytes per second (0 is no mod download)", 0, 0, 0, 500000, 5000),
	maxAvatarSize_("Max size allowed for avatars in bytes (0 is no avatars)", 0, 5000),
	economy_("Speicifies the name of the economy to use", 0, "EconomyFreeMarket", economyEnum),
	landscapes_("Colon seperated list of landscape names", 0, ""),
	serverPassword_("The password for this server (empty password is no password)", XMLEntrySimpleType::eDataProtected, ""),
	allowSameIP_("Allow scorched clients from same machine/NAT router to connect.", 0, true),
	allowSameUniqueId_("Allow scorched clients with same unique id to connect.", 0, true),
	publishServer_("Allow other scorched net clients to see this server.  Do not use for LAN games.", 0, false),
	useUPnP_("Try to automaticaly add external port forwarding rules using UPnP", 0, false),
	useUPnPLogging_("Turn on packet logging for UPnP", 0, false),
	publishAddress_("IP address to publish to scorched net clients (auto-detected if not given).", 0, "AutoDetect"),
	botNamePrefix_("Prepend and bot name with the specified text", 0, "(Bot) "),
	giveAllWeapons_("Start the game will all the weapons", 0, false),
	registeredUserNames_("Only allow authenticated players to use their own player names", 0, false),
	allowMultiLingualChat_("Allow internaltional (non-latin) characters in chat", 0, true),
	allowMultiLingualNames_("Allow internaltional (non-latin) characters in names", 0, true),
	authHandler_("Only allow authenticated players to connect", 0, "none", authHandlerEnum),
	authHandlerParam1_("First parameter for the authentication handler", 0, ""),
	authHandlerParam2_("Second parameter for the authentication handler", 0, ""),
	cycleMaps_("Cycle through the maps instead of choosing them using a random probablity", 0, false),
	randomizeBotNames_("Choose random bot names instread of sequential names", 0, false),
	computersDeathTalk_("The percentage chance the computers will say something when killed", 0, 100, 0, 100, 10),
	computersAttackTalk_("The percentage chance the computers will say something when shooting", 0, 25, 0, 100, 10),
	debugFeatures_("Set to \"true\" to enable debugging features such as InfoGrid", 0, false),
	waitForShotsBeforeShowingScore_("Wait for all shots to be played out before showing the score dialog", 0, true)
{
	addChildXMLEntry("WeaponSpeed", &weaponSpeed_, "StartArmsLevel", &startArmsLevel_, "EndArmsLevel", &endArmsLevel_, "ShotTime", &shotTime_, "AIShotTime", &aiShotTime_);
	addChildXMLEntry("StartTime", &startTime_, "BuyingTime", &buyingTime_, "RemoveTime", &removeTime_, "RoundTime", &roundTime_, "RoundScoreTime", &roundScoreTime_);
	addChildXMLEntry("ScoreTime", &scoreTime_, "IdleCycleTime", &idleCycleTime_, "AllowedMissedMoves", &allowedMissedMoves_, "NumberOfRounds", &numberOfRounds_, "MaxNumberOfRoundTurns", &maxRoundTurns_);
	addChildXMLEntry("MaxNumberWeapons", &maxNumberWeapons_, "Gravity", &gravity_, "MinFallingDistance", &minFallingDistance_, "TankFallingDamage", &tankFallingDamage_, "MaxClimbingDistance", &maxClimbingDistance_);
	addChildXMLEntry("PlayerLives", &playerLives_, "Teams", &teams_, "NumberOfPlayers", &numberOfPlayers_, "NumberOfMinPlayers", &numberOfMinPlayers_, "RemoveBotsAtPlayers", &removeBotsAtPlayers_);
	addChildXMLEntry("ComputersDeathTalk", &computersDeathTalk_, "ComputersAttackTalk", &computersAttackTalk_, "MoneyBuyOnRound", &moneyBuyOnRound_, "MoneyWonForRound", &moneyWonForRound_, "MoneyWonForLives", &moneyWonForLives_);
	addChildXMLEntry("MoneyWonPerKillPoint", &moneyPerKillPoint_, "MoneyWonPerMultiKillPoint", &moneyPerMultiKillPoint_, "MoneyWonPerAssistPoint", &moneyPerAssistPoint_, "MoneyWonPerHitPoint", &moneyPerHitPoint_, "MoneyPerRound", &moneyPerRound_);
	addChildXMLEntry("MoneyPerHealthPoint", &moneyPerHealthPoint_, "ScorePerMoney", &scorePerMoney_, "ScorePerAssist", &scorePerAssist_, "ScorePerKill", &scorePerKill_, "ScorePerResign", &scorePerResign_);
	addChildXMLEntry("ScorePerSpectate", &scorePerSpectate_, "ScoreWonForRound", &scoreWonForRound_, "ScoreWonForLives", &scoreWonForLives_, "MaxLandscapeSize", &maxLandscapeSize_, "FreeMarketAdjustment", &freeMarketAdjustment_);
	addChildXMLEntry("FreeMarketLimits", &freeMarketLimits_, "FreeMarketTrackBotPurchases", &freeMarketTracksBotPurchases_, "MinimumLandHeight", &minimumLandHeight_, "SkillForRound", &skillForRound_, "SkillForMatch", &skillForMatch_);
	addChildXMLEntry("SkillForResign", &skillForResign_, "MaxSkillLost", &maxSkillLost_, "MaxSkillGained", &maxSkillGained_, "SkillForSelfKill", &skillForSelfKill_, "SkillForTeamKill", &skillForTeamKill_);
	addChildXMLEntry("MoneyStarting", &moneyStarting_, "TeamBallance", &teamBallance_, "MoneyInterest", &moneyInterest_, "LimitPowerByHealth", &limitPowerByHealth_, "Tutorial", &tutorial_);
	addChildXMLEntry("CycleMaps", &cycleMaps_, "ResignMode", &resignMode_, "MovementRestriction", &movementRestriction_, "TurnType", &turnType_, "WindForce", &windForce_);
	addChildXMLEntry("WindType", &windType_, "WallType", &wallType_, "WeaponScale", &weapScale_, "ModDownloadSpeed", &modDownloadSpeed_, "MaxAvatarSize", &maxAvatarSize_);
	addChildXMLEntry("Mod", &mod_, "MOTD", &motd_, "Economy", &economy_, "Landscapes", &landscapes_, "StatsLogger", &statsLogger_);
	addChildXMLEntry("ServerLogger", &serverFileLogger_, "WaitForShotsBeforeShowingScore", &waitForShotsBeforeShowingScore_);

	// Server only options
	addChildXMLEntry("BotNamePrefix", &botNamePrefix_, "ActionSyncCheck", &actionSyncCheck_, "ActionMovementSyncCheck", &actionMovementSyncCheck_, "ActionRandomSyncCheck", &actionRandomSyncCheck_);
	addChildXMLEntry("ActionCollisionSyncCheck", &actionCollisionSyncCheck_, "TargetPlacementSyncCheck", &targetPlacementSyncCheck_, "WeaponSyncCheck", &weaponSyncCheck_, "AccessoryNameSyncCheck", &accessoryNameSyncCheck_);
	addChildXMLEntry("AutoSendSyncCheck", &autoSendSyncCheck_, "ResidualPlayers", &residualPlayers_, "DelayedDefenseActivation", &delayedDefenseActivation_, "RandomizeBotNames", &randomizeBotNames_, "GiveAllWeapons", &giveAllWeapons_);
	addChildXMLEntry("RegisteredUserNames", &registeredUserNames_, "AllowMultiLingualChat", &allowMultiLingualChat_, "AllowMultiLingualNames", &allowMultiLingualNames_);
	addChildXMLEntry("AuthHandler", &authHandler_, "AuthHandlerParam1", &authHandlerParam1_, "AuthHandlerParam2", &authHandlerParam2_);
	addChildXMLEntry("ServerName", &serverName_, "ServerPassword", &serverPassword_, "PortNo", &portNo_, "ManagementPortNo", &managementPortNo_);
	addChildXMLEntry("PublishAddress", &publishAddress_, "PublishServer", &publishServer_, "UseUPnP", &useUPnP_, "UseUPnPLogging", &useUPnPLogging_, "AllowSameIP", &allowSameIP_, "AllowSameUniqueId", &allowSameUniqueId_, "DebugFeatures", &debugFeatures_);

	// The type of the players
	for (int i=0; i<24; i++)
	{
		std::string playerName = S3D::formatStringBuffer("PlayerType%i", i+1);
		playerType_[i] = new XMLEntryString(
			"The type of the player e.g. human, computer etc..", 0,
			"Human");
		addChildXMLEntry(playerName.c_str(), playerType_[i]);
	}
}

OptionsGameBase::~OptionsGameBase()
{
	std::list<XMLEntry *>::iterator itor;
	for (itor = playerTypeOptions_.begin();
		itor != playerTypeOptions_.end();
		++itor)
	{
		XMLEntry *entry = *itor;
		delete entry;
	}
}

OptionsGame::OptionsGame() :
	XMLEntryRoot<OptionsGameBase>(
		S3D::eInvalidLocation, "multiple", "OptionsGame",
		"OptionsGame", "Scorched3D options that define properties for the game engine", true)
{
}

OptionsGame::~OptionsGame()
{
}
