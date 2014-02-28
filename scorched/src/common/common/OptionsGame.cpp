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

OptionsGame::OptionsGame() :
	XMLEntrySimpleGroup("options", "Scorched3D options that define properties for the game engine"),
	tutorial_("Tutorial",
		"The tutorial to load for this game", 0, ""),
	scorePerMoney_("ScorePerMoney",
		"The amount of score awarded for 100 money units earned", 0, 0, 0, 100, 2),
	scorePerAssist_("ScorePerAssist",
		"The amount of score awarded for each kill assist", 0, 2, 0, 50, 1),
	scorePerKill_("ScorePerKill",
		"The amount of score awarded for each kill", 0, 10, 0, 50, 1),
	scorePerResign_("ScorePerResign",
		"The amount of score awarded for each resign", 0, 0, -50, 50, 1),
	scorePerSpectate_("ScorePerSpectate",
		"The amount of score awarded for each move to spectator (only when tank is still alive)", 0, 0, -10, 50, 1),
	scoreWonForRound_("ScoreWonForRound",
		"The amount of score awarded for round won", 0, 250, 0, 500, 10),
	scoreWonForLives_("ScoreWonForLives",
		"The amount of score awarded for lives left", 0, 0, 0, 500, 10),
	skillForRound_("SkillForRound",
		"The amount of skill awarded for winning the round", 0, 0, 0, 100, 5),
	skillForMatch_("SkillForMatch",
		"The amount of skill awarded for winning the match", 0, 0, 0, 100, 5),
	skillForResign_("SkillForResign",
		"The amount of skill awarded for resigning", 0, 0, -50, 50, 5),
	maxSkillLost_("MaxSkillLost",
		"The maximum amount of skill you can lose for being killed", 0, 10, 0, 100, 5),
	maxSkillGained_("MaxSkillGained",
		"The maximum amount of skill you can gain for killing someone", 0, 10, 0, 100, 5),
	skillForSelfKill_("SkillForSelfKill",
		"The amount of skill you lose for being killed by yourself", 0, -25, -50, 0, 5),
	skillForTeamKill_("SkillForTeamKill",
		"The amount of skill you lose for killing a team memeber", 0, -25, -50, 0, 5),
	teams_("Teams",
		"The number of teams (1 is no teams)", 0, 1, 1, 4, 1),
	minimumLandHeight_("MinimumLandHeight",
		"The minimum height the ground can reach", 0, 0),
	startArmsLevel_("StartArmsLevel",
		"The largest weapon type allowed (on round 1)", 0, 10, 0, 10, 1),
	endArmsLevel_("EndArmsLevel",
		"The largest weapon type allowed (on the last round)", 0, 10, 0, 10, 1),
	maxNumberWeapons_("MaxNumberWeapons",
		"The number of each weapon the player is allowed", 0, 90, 0, 500, 10),
	weaponSpeed_("WeaponSpeed",
		"The speed of projectiles and rollers", 0, 15000, 2500, 40000, 2500),
	maxLandscapeSize_("MaxLandscapeSize",
		"The largest landscape (in bytes) that will be sent to the clients", 0, 200000),
	startTime_("StartTime", 
		"The amount of time before a new game starts", 0, 0, 0, 90, 5),
	shotTime_("ShotTime", 
		"The amount of time each player has for each shot", 0, 0, 0, 90, 5),
	aiShotTime_("AIShotTime", 
		"The amount of time each computer controller player has for each shot", 0, 10, 0, 90, 2),
	buyingTime_("BuyingTime", 
		"The amount of time each player has to buy weapons and use auto defense", 0, 0, 0, 90, 5),
	removeTime_("RemoveTime", 
		"The amount of time each player has before being removed when disconected", 0, 10, 0, 25, 1),
	allowedMissedMoves_("AllowedMissedMoves",
		"The number of moves a player is allowed to miss (due to the shot timer)", 0, 0, 0, 10, 1),
	roundScoreTime_("RoundScoreTime", 
		"The amount of time to show the end of each round for", 0, 5, 0, 30, 1),
	idleCycleTime_("IdleCycleTime", 
		"The time before a new map is loaded when no one is playing", 0, 900, 45, 1800, 45),
	scoreTime_("ScoreTime", 
		"The amount of time to show the end of match scores for", 0, 15, 0, 90, 5),
	roundTime_("RoundTime", 
		"The amount of time for each round", 0, 0, 0, 900, 30),
	minFallingDistance_("MinFallingDistance",
		"The minimum distance that can be made with no damage (value is divided by 10)", 0, 5, 0, 100, 5),
	tankFallingDamage_("TankFallingDamage", 
		"If tanks tank damage from falling", 0, true),
	maxClimbingDistance_("MaxClimbingDistance",
		"The maximum distance that a tank can climb per movement square (divided by 10)", 0, 8, 0, 20, 1),
	playerLives_("PlayerLives",
		"The number of lives that each player tank has", 0, 1, 0, 10, 1),
	gravity_("Gravity",
		"The gravity used by the physics engine", 0, -10, -25, 0, 1),
	maxRoundTurns_("MaxNumberOfRoundTurns",
		"The maximum number of turns all players are allowed in each round (0 is infinite)", 0, 15, 0, 50, 1),
	numberOfRounds_("NumberOfRounds", 
		"The number of rounds to play in each game", 0, 5, 1, 50, 1),
	numberOfPlayers_("NumberOfPlayers", 
		"The maximum number of players to allow", 0, 2, 2, 24, 1),
	numberOfMinPlayers_("NumberOfMinPlayers", 
		"The minimum number of players to allow", 0, 2, 2, 24, 1),
	removeBotsAtPlayers_("RemoveBotsAtPlayers",
		"The number of players to allow before remvoing bots", 0, 0, 0, 24, 1),
	limitPowerByHealth_("LimitPowerByHealth", 
		"Show power is limited by the amount of health a player has", 0, true),
	residualPlayers_("ResidualPlayers",
		"Players that leave will have the same state when reconnecting", 0, true),
	delayedDefenseActivation_("DelayedDefenseActivation",
		"Changes to shields are only seen after the aiming phase.", 0, false),
	autoSendSyncCheck_("AutoSendSyncCheck",
		"Automatically send a sync check at the end of each shot", 0, false),
	actionSyncCheck_("ActionSyncCheck",
		"Gather enhanced action syncchecking", 0, false),
	actionMovementSyncCheck_("ActionMovementSyncCheck",
		"Gather enhanced action syncchecking for target movements", 0, false),
	actionRandomSyncCheck_("ActionRandomSyncCheck",
		"Gather enhanced action syncchecking for random number generation", 0, false),
	actionCollisionSyncCheck_("ActionCollisionSyncCheck",
		"Gather enhanced action syncchecking for particle collisions", 0, false),
	targetPlacementSyncCheck_("TargetPlacementSyncCheck",
		"Gather enhanced action syncchecking for initial target placements", 0, false),
	weaponSyncCheck_("WeaponSyncCheck",
		"Gather enhanced action syncchecking for weapon invocations", 0, false),
	accessoryNameSyncCheck_("AccessoryNameSyncCheck",
		"Also send the accessory names when sending the tank sync check", 0, false),
	resignMode_("ResignMode",
		"When does a players resign take place", 0, int(ResignTimed), resignEnum),
	movementRestriction_("MovementRestriction",
		"Where a tank is allowed to move to", 0, int(MovementRestrictionNone), movementRestrictionEnum),
	teamBallance_("TeamBallance",
		"The mode of team auto-ballancing performed for team games", 0, int(TeamBallanceNone), teamBallanceEnum),
	turnType_("TurnType", 
		"The player turn mode", 0, 	int(TurnSequentialLoserFirst), turnEnum), // Data, default, min, max
	moneyBuyOnRound_("MoneyBuyOnRound", 
		"The first round players are allowed to buy on", 0 ,2, 1, 50, 1),
	moneyWonForRound_("MoneyWonForRound", 
		"The money awarded for each won round", 0, 5000, 0, 100000, 1000),
	moneyWonForLives_("MoneyWonForLives",
		"The money awarded for each life remaining", 0, 5000, 0, 100000, 1000),
	moneyPerKillPoint_("MoneyWonPerKillPoint", 
		"The money awarded for each person killed * weapon used", 0, 750, 0, 2000, 50),
	moneyPerMultiKillPoint_("MoneyWonPerMultiKillPoint", 
		"The extra money awarded for each person multi-killed * weapon used * multi-kill", 0, 0, 0, 2000, 50),
	moneyPerAssistPoint_("MoneyWonPerAssistPoint", 
		"The money awarded for each kill assist * weapon used", 0, 250, 0, 2000, 50),
	moneyPerHitPoint_("MoneyWonPerHitPoint", 
		"The money awarded for each person hit * weapon used", 0, 250, 0, 2000, 50),
	moneyPerRound_("MoneyPerRound", 
		"The money given at the end of each round", 0, 0, 0, 250000, 5000),
	moneyPerHealthPoint_("MoneyPerHealthPoint",
		"The money awarded is proportional to the amount of health removed", 0, true),
	moneyStarting_("MoneyStarting", 
		"The money each player starts with", 0, 10000, 0, 500000, 10000),
	moneyInterest_("MoneyInterest", 
		"The interest awarded at the end of each round", 0, 15, 0, 100, 5),
	freeMarketAdjustment_("FreeMarketAdjustment",
		"The scale of the adjustment changes made by the free market", 0, 100),
	freeMarketLimits_("FreeMarketLimits",
		"The scale of the max/min prices (*1.5) allowed by the free market e.g. 2 is a 300% increase/reduction", 0, 2),
	freeMarketTracksBotPurchases_("FreeMarketTrackBotPurchases",
		"Allows the purchases bots (ais) make to be tracked by the free market", 0, false),
	windForce_("WindForce", 
		"The force of the wind", 0, int(WindRandom), windForceEnum),
	windType_("WindType", 
		"When the wind changes", 0, int(WindChangeNever), windTypeEnum),
	wallType_("WallType", 
		"The type of walls allowed", 0, int(WallRandom), wallEnum),
	weapScale_("WeaponScale", 
		"The scale of the weapons used", 0, int(ScaleMedium), weapScaleEnum),
	statsLogger_("StatsLogger",
		"The type of player stats to be logged", 0, "none", statsLoggerEnum),
	serverFileLogger_("ServerLogger",
	"The type of server events to be logged to file", 0, "none", serverFileLoggerEnum),
	portNo_("PortNo", 
		"The port to start the server on", 0, S3D::ScorchedPort),
	managementPortNo_("ManagementPortNo", 
		"The port to start the managament web server on (0 is management off)", 0, 0),
	serverName_("ServerName", 
		"The name of the server to start", 0, "No Name"),
	mod_("Mod", 
		"The name of currently running Scorched3D mod", 0, "none"),
	motd_("MOTD",
		"The message of the day", 0, 
		"Scorched3D : Copyright 2011 Gavin Camp\n"
		"For updates and news visit:\n"
		"http://www.scorched3d.co.uk",
		true),
	modDownloadSpeed_("ModDownloadSpeed",
		"Max download speed of mods in bytes per second (0 is no mod download)", 0, 0, 0, 500000, 5000),
	maxAvatarSize_("MaxAvatarSize",
		"Max size allowed for avatars in bytes (0 is no avatars)", 0, 5000),
	economy_("Economy", 
		"Speicifies the name of the economy to use", 0, "EconomyFreeMarket", economyEnum),
	landscapes_("Landscapes", 
		"Colon seperated list of landscape names", 0, ""),
	serverPassword_("ServerPassword", 
		"The password for this server (empty password is no password)", XMLEntrySimpleType::eDataProtected, ""),
	allowSameIP_("AllowSameIP",
		"Allow scorched clients from same machine/NAT router to connect.", 0, true),
	allowSameUniqueId_("AllowSameUniqueId",
		"Allow scorched clients with same unique id to connect.", 0, true),
	publishServer_("PublishServer",
		"Allow other scorched net clients to see this server.  Do not use for LAN games.", 0, false),
	useUPnP_("UseUPnP",
		"Try to automaticaly add external port forwarding rules using UPnP", 0, false),
	useUPnPLogging_("UseUPnPLogging",
		"Turn on packet logging for UPnP", 0, false),
	publishAddress_("PublishAddress",
		"IP address to publish to scorched net clients (auto-detected if not given).", 0, "AutoDetect"),
	botNamePrefix_("BotNamePrefix", 
		"Prepend and bot name with the specified text", 0, "(Bot) "),
	giveAllWeapons_("GiveAllWeapons",
		"Start the game will all the weapons", 0, false),
	registeredUserNames_("RegisteredUserNames",
		"Only allow authenticated players to use their own player names", 0, false),
	allowMultiLingualChat_("AllowMultiLingualChat",
		"Allow internaltional (non-latin) characters in chat", 0, true),
	allowMultiLingualNames_("AllowMultiLingualNames",
		"Allow internaltional (non-latin) characters in names", 0, true),
	authHandler_("AuthHandler",
		"Only allow authenticated players to connect", 0, "none", authHandlerEnum),
	authHandlerParam1_("AuthHandlerParam1",
		"First parameter for the authentication handler", 0, ""),
	authHandlerParam2_("AuthHandlerParam2",
		"Second parameter for the authentication handler", 0, ""),
	cycleMaps_("CycleMaps",
		"Cycle through the maps instead of choosing them using a random probablity", 0, false),
	randomizeBotNames_("RandomizeBotNames",
		"Choose random bot names instread of sequential names", 0, false),
	computersDeathTalk_("ComputersDeathTalk",
		"The percentage chance the computers will say something when killed", 0, 100, 0, 100, 10),
	computersAttackTalk_("ComputersAttackTalk",
		"The percentage chance the computers will say something when shooting", 0, 25, 0, 100, 10),
	debugFeatures_("DebugFeatures",
		"Set to \"true\" to enable debugging features such as InfoGrid", 0, false),
	waitForShotsBeforeShowingScore_("WaitForShotsBeforeShowingScore",
		"Wait for all shots to be played out before showing the score dialog", 0, true)
{
	addChildXMLEntry(&weaponSpeed_, &startArmsLevel_, &endArmsLevel_, &shotTime_, &aiShotTime_);
	addChildXMLEntry(&startTime_, &buyingTime_, &removeTime_, &roundTime_, &roundScoreTime_);
	addChildXMLEntry(&scoreTime_, &idleCycleTime_, &allowedMissedMoves_, &numberOfRounds_, &maxRoundTurns_);
	addChildXMLEntry(&maxNumberWeapons_, &gravity_, &minFallingDistance_, &tankFallingDamage_, &maxClimbingDistance_);
	addChildXMLEntry(&playerLives_, &teams_, &numberOfPlayers_, &numberOfMinPlayers_, &removeBotsAtPlayers_);
	addChildXMLEntry(&computersDeathTalk_, &computersAttackTalk_, &moneyBuyOnRound_, &moneyWonForRound_, &moneyWonForLives_);
	addChildXMLEntry(&moneyPerKillPoint_, &moneyPerMultiKillPoint_, &moneyPerAssistPoint_, &moneyPerHitPoint_, &moneyPerRound_);
	addChildXMLEntry(&moneyPerHealthPoint_, &scorePerMoney_, &scorePerAssist_, &scorePerKill_, &scorePerResign_);
	addChildXMLEntry(&scorePerSpectate_, &scoreWonForRound_, &scoreWonForLives_, &maxLandscapeSize_, &freeMarketAdjustment_);
	addChildXMLEntry(&freeMarketLimits_, &freeMarketTracksBotPurchases_, &minimumLandHeight_, &skillForRound_, &skillForMatch_);
	addChildXMLEntry(&skillForResign_, &maxSkillLost_, &maxSkillGained_, &skillForSelfKill_, &skillForTeamKill_);
	addChildXMLEntry(&moneyStarting_, &teamBallance_, &moneyInterest_, &limitPowerByHealth_, &tutorial_);
	addChildXMLEntry(&cycleMaps_, &resignMode_, &movementRestriction_, &turnType_, &windForce_);
	addChildXMLEntry(&windType_, &wallType_, &weapScale_, &modDownloadSpeed_, &maxAvatarSize_);
	addChildXMLEntry(&mod_, &motd_, &economy_, &landscapes_, &statsLogger_);
	addChildXMLEntry(&serverFileLogger_, &waitForShotsBeforeShowingScore_);

	// Server only options
	addChildXMLEntry(&botNamePrefix_, &actionSyncCheck_, &actionMovementSyncCheck_, &actionRandomSyncCheck_);
	addChildXMLEntry(&actionCollisionSyncCheck_, &targetPlacementSyncCheck_, &weaponSyncCheck_, &accessoryNameSyncCheck_);
	addChildXMLEntry(&autoSendSyncCheck_, &residualPlayers_, &delayedDefenseActivation_, &randomizeBotNames_, &giveAllWeapons_);
	addChildXMLEntry(&registeredUserNames_, &allowMultiLingualChat_, &allowMultiLingualNames_);
	addChildXMLEntry(&authHandler_, &authHandlerParam1_, &authHandlerParam2_);
	addChildXMLEntry(&serverName_, &serverPassword_, &portNo_, &managementPortNo_);
	addChildXMLEntry(&publishAddress_, &publishServer_, &useUPnP_, &useUPnPLogging_, &allowSameIP_, &allowSameUniqueId_, &debugFeatures_);

	// The type of the players
	for (int i=0; i<24; i++)
	{
		playerNames_.push_back(S3D::formatStringBuffer("PlayerType%i", i+1)); // So we can pass a pointer to the string, without the string being destroyed
		playerType_[i] = new XMLEntryString(
			playerNames_.back().c_str(),
			"The type of the player e.g. human, computer etc..", 0,
			"Human");
		addChildXMLEntry(playerType_[i]);
	}
}

OptionsGame::~OptionsGame()
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
