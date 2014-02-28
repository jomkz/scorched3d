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

#if !defined(__INCLUDE_OptionsGameh_INCLUDE__)
#define __INCLUDE_OptionsGameh_INCLUDE__

#include <XML/XMLEntrySimpleTypes.h>

class OptionsGame : public XMLEntrySimpleContainer
{
public:
	OptionsGame();
	virtual ~OptionsGame();

	enum ScoreType
	{
		ScoreWins = 0,
		ScoreKills = 1,
		ScoreMoney = 2
	};
	enum TurnType
	{
		TurnSimultaneous = 0,
		TurnSequentialLoserFirst = 1,
		TurnSequentialRandom = 2,
		TurnSimultaneousNoWait = 3,
		TurnFree = 4,
		TurnFreeTimed = 5
	};
	enum WindForce
	{
		WindRandom = 0,
		WindNone = 1,
		Wind1 = 2,
		Wind2 = 3,
		Wind3 = 4,
		Wind4 = 5,
		Wind5 = 6,
		WindBreezy = 7,
		WindGale = 8
	};
	enum WindType
	{
		WindChangeNever = 0,
		WindChangeSomeTimes = 1,
		WindChangeFrequently = 2,
		WindChangeConstantly = 3,
		WindChangeAlways = 4
	};
	enum WallType
	{
		WallRandom = 0,
		WallConcrete = 1,
		WallBouncy = 2,
		WallWrapAround = 3,
		WallNone = 4,
		WallActive = 5,
		WallInactive = 6
	};
	enum WeapScale
	{
		ScaleSmall = 0,
		ScaleMedium = 1,
		ScaleLarge = 2
	};
	enum ResignType
	{
		ResignStart = 0,
		ResignTimed = 1,
		ResignNone = 2
	};
	enum MovementRestrictionType
	{
		MovementRestrictionNone = 0,
		MovementRestrictionLand = 1,
		MovementRestrictionLandOrAbove = 2
	};
	enum TeamBallanceType
	{
		TeamBallanceNone = 0,
		TeamBallanceAuto = 1,
		TeamBallanceBotsVs = 2,
		TeamBallanceAutoByScore = 3,
		TeamBallanceAutoByBots = 4
	};
	
	const char *getTutorial() { return tutorial_.getValue().c_str(); }
	XMLEntryString &getTutorialEntry() { return tutorial_; }

	int getScorePerMoney() { return scorePerMoney_.getValue(); }
	XMLEntryInt &getScorePerMoneyEntry() { return scorePerMoney_; }

	int getScorePerAssist() { return scorePerAssist_.getValue(); }
	XMLEntryInt &getScorePerAssistEntry() { return scorePerAssist_; }

	int getScorePerKill() { return scorePerKill_.getValue(); }
	XMLEntryInt &getScorePerKillEntry() { return scorePerKill_; }

	int getScorePerResign() { return scorePerResign_.getValue(); }
	XMLEntryInt &getScorePerResignEntry() { return scorePerResign_; }

	int getScorePerSpectate() { return scorePerSpectate_.getValue(); }
	XMLEntryInt &getScorePerSpectateEntry() { return scorePerSpectate_; }

	int getScoreWonForRound() { return scoreWonForRound_.getValue(); }
	XMLEntryInt &getScoreWonForRoundEntry() { return scoreWonForRound_; }

	int getScoreWonForLives() { return scoreWonForLives_.getValue(); }
	XMLEntryInt &getScoreWonForLivesEntry() { return scoreWonForLives_; }

	int getSkillForRound() { return skillForRound_.getValue(); }
	XMLEntryInt &getSkillForRoundEntry() { return skillForRound_; }

	int getSkillForMatch() { return skillForMatch_.getValue(); }
	XMLEntryInt &getSkillForMatchEntry() { return skillForMatch_; }

	int getSkillForResign() { return skillForResign_.getValue(); }
	XMLEntryInt &getSkillForResignEntry() { return skillForResign_; }

	int getMaxSkillLost() { return maxSkillLost_.getValue(); }
	XMLEntryInt &getMaxSkillLostEntry() { return maxSkillLost_; }

	int getMaxSkillGained() { return maxSkillGained_.getValue(); }
	XMLEntryInt &getMaxSkillGainedEntry() { return maxSkillGained_; }

	int getSkillForSelfKill() { return skillForSelfKill_.getValue(); }
	XMLEntryInt &getSkillForSelfKillEntry() { return skillForSelfKill_; }

	int getSkillForTeamKill() { return skillForTeamKill_.getValue(); }
	XMLEntryInt &getSkillForTeamKillEntry() { return skillForTeamKill_; }

	int getTeams() { return teams_.getValue(); }
	XMLEntryInt &getTeamsEntry() { return teams_; }

	int getMinimumLandHeight() { return minimumLandHeight_.getValue(); }
	XMLEntryInt &getMinimumLandHeightEntry() { return minimumLandHeight_; }

	int getWeaponSpeed() { return weaponSpeed_.getValue(); }
	XMLEntryInt &getWeaponSpeedEntry() { return weaponSpeed_; }

	int getStartArmsLevel() { return startArmsLevel_.getValue(); }
	XMLEntryInt &getStartArmsLevelEntry() { return startArmsLevel_; }

	int getEndArmsLevel() { return endArmsLevel_.getValue(); }
	XMLEntryInt &getEndArmsLevelEntry() { return endArmsLevel_; }

	int getMaxNumberWeapons() { return maxNumberWeapons_.getValue(); }
	XMLEntryInt &getMaxNumberWeaponsEntry() { return maxNumberWeapons_; }

	int getNoMaxPlayers() { return numberOfPlayers_.getValue(); }
	XMLEntryInt &getNoMaxPlayersEntry() { return numberOfPlayers_; }

	int getNoMinPlayers() { return numberOfMinPlayers_.getValue(); }
	XMLEntryInt &getNoMinPlayersEntry() { return numberOfMinPlayers_; }

	int getNoRounds() { return numberOfRounds_.getValue(); }
	XMLEntryInt &getNoRoundsEntry() { return numberOfRounds_; }

	int getNoTurns() { return maxRoundTurns_.getValue(); }
	XMLEntryInt &getNoTurnsEntry() { return maxRoundTurns_; }

	int getRemoveBotsAtPlayers() { return removeBotsAtPlayers_.getValue(); }
	XMLEntryInt &getRemoveBotsAtPlayersEntry() { return removeBotsAtPlayers_; }

	int getStartMoney() { return moneyStarting_.getValue(); }
	XMLEntryInt &getStartMoneyEntry() { return moneyStarting_; }

	int getInterest() { return moneyInterest_.getValue(); }
	XMLEntryInt &getInterestEntry() { return moneyInterest_; }

	int getFreeMarketAdjustment() { return freeMarketAdjustment_.getValue(); }
	XMLEntryInt &getFreeMarketAdjustmentEntry() { return freeMarketAdjustment_; }

	int getFreeMarketLimits() { return freeMarketLimits_.getValue(); }
	XMLEntryInt &getFreeMarketLimitsEntry() { return freeMarketLimits_; }

	bool getFreeMarketTracksBotPurchases() { return freeMarketTracksBotPurchases_.getValue(); }
	XMLEntryBool &getFreeMarketTracksBotPurchasesEntry() { return freeMarketTracksBotPurchases_; }

	int getMaxLandscapeSize() { return maxLandscapeSize_.getValue(); }
	XMLEntryInt &getMaxLandscapeSizeEntry() { return maxLandscapeSize_; }

	int getStartTime() { return startTime_.getValue(); }
	XMLEntryInt &getStartTimeEntry() { return startTime_; }

	int getShotTime() { return shotTime_.getValue(); }
	XMLEntryInt &getShotTimeEntry() { return shotTime_; }

	int getAIShotTime() { return aiShotTime_.getValue(); }
	XMLEntryInt &getAIShotTimeEntry() { return aiShotTime_; }

	int getBuyingTime() { return buyingTime_.getValue(); }
	XMLEntryInt &getBuyingTimeEntry() { return buyingTime_; }

	int getRemoveTime() { return removeTime_.getValue(); }
	XMLEntryInt &getRemoveTimeEntry() { return removeTime_; }

	int getRoundTime() { return roundTime_.getValue(); }
	XMLEntryInt &getRoundTimeEntry() { return roundTime_; }

	int getRoundScoreTime() { return roundScoreTime_.getValue(); }
	XMLEntryInt &getRoundScoreTimeEntry() { return roundScoreTime_; }

	int getScoreTime() { return scoreTime_.getValue(); }
	XMLEntryInt &getScoreTimeEntry() { return scoreTime_; }

	int getIdleCycleTime() { return idleCycleTime_.getValue(); }
	XMLEntryInt &getIdleCycleTimeEntry() { return idleCycleTime_; }

	int getAllowedMissedMoves() { return allowedMissedMoves_.getValue(); }
	XMLEntryInt &getAllowedMissedMovesEntry() { return allowedMissedMoves_; }

	int getMinFallingDistance() { return minFallingDistance_.getValue(); }
	XMLEntryInt &getMinFallingDistanceEntry() { return minFallingDistance_; }

	bool getTankFallingDamage() { return tankFallingDamage_.getValue(); }
	XMLEntryBool &getTankFallingDamageEntry() { return tankFallingDamage_; }

	int getMaxClimbingDistance() { return maxClimbingDistance_.getValue(); }
	XMLEntryInt &getMaxClimbingDistanceEntry() { return maxClimbingDistance_; }

	int getPlayerLives() { return playerLives_.getValue(); }
	XMLEntryInt &getPlayerLivesEntry() { return playerLives_; }

	int getGravity() { return gravity_.getValue(); }
	XMLEntryInt &getGravityEntry() { return gravity_; }

	WindForce getWindForce() { return (WindForce) windForce_.getValue(); } 
	XMLEntryEnum &getWindForceEntry() { return windForce_; } 

	WindType getWindType() { return (WindType) windType_.getValue(); } 
	XMLEntryEnum &getWindTypeEntry() { return windType_; } 

	WallType getWallType() { return (WallType) wallType_.getValue(); } 
	XMLEntryEnum &getWallTypeEntry() { return wallType_; }

	WeapScale getWeapScale() { return (WeapScale) weapScale_.getValue(); } 
	XMLEntryEnum &getWeapScaleEntry() { return weapScale_; } 
	
	TurnType getTurnType() { return (TurnType) turnType_.getValue(); } 
	XMLEntryEnum &getTurnTypeEntry() { return turnType_; } 
	 
	int getBuyOnRound() { return moneyBuyOnRound_.getValue(); }
	XMLEntryInt &getBuyOnRoundEntry() { return moneyBuyOnRound_; }

	int getMoneyPerRound() { return moneyPerRound_.getValue(); }
	XMLEntryInt &getMoneyPerRoundEntry() { return moneyPerRound_; }

	int getMoneyWonForRound() { return moneyWonForRound_.getValue(); }
	XMLEntryInt &getMoneyWonForRoundEntry() { return moneyWonForRound_; }

	int getMoneyWonForLives() { return moneyWonForLives_.getValue(); }
	XMLEntryInt &getMoneyWonForLivesEntry() { return moneyWonForLives_; }

	int getMoneyWonPerKillPoint() { return moneyPerKillPoint_.getValue(); }
	XMLEntryInt &getMoneyWonPerKillPointEntry() { return moneyPerKillPoint_; }

	int getMoneyWonPerMultiKillPoint() { return moneyPerMultiKillPoint_.getValue(); }
	XMLEntryInt &getMoneyWonPerMultiKillPointEntry() { return moneyPerMultiKillPoint_; }

	int getMoneyWonPerAssistPoint() { return moneyPerAssistPoint_.getValue(); }
	XMLEntryInt &getMoneyWonPerAssistPointEntry() { return moneyPerAssistPoint_; }

	int getMoneyWonPerHitPoint() { return moneyPerHitPoint_.getValue(); }
	XMLEntryInt &getMoneyWonPerHitPointEntry() { return moneyPerHitPoint_; }

	bool getMoneyPerHealthPoint() { return moneyPerHealthPoint_.getValue(); }
	XMLEntryBool &getMoneyPerHealthPointEntry() { return moneyPerHealthPoint_; }

	bool getLimitPowerByHealth() { return limitPowerByHealth_.getValue(); }
	XMLEntryBool &getLimitPowerByHealthEntry() { return limitPowerByHealth_; }
	
	TeamBallanceType getTeamBallance() { return (TeamBallanceType) teamBallance_.getValue(); } 
	XMLEntryEnum &getTeamBallanceEntry() { return teamBallance_; } 

	int getComputersDeathTalk() { return computersDeathTalk_.getValue(); }
	XMLEntryInt &getComputersDeathTalkEntry() { return computersDeathTalk_; }

	int getComputersAttackTalk() { return computersAttackTalk_.getValue(); }
	XMLEntryInt &getComputersAttackTalkEntry() { return computersAttackTalk_; }

	const char * getEconomy() { return economy_.getValue().c_str(); }
	XMLEntryStringEnum &getEconomyEntry() { return economy_; }

	const char * getLandscapes() { return landscapes_.getValue().c_str(); }
	XMLEntryString &getLandscapesEntry() { return landscapes_; }

	const char * getStatsLogger() { return statsLogger_.getValue().c_str(); }
	XMLEntryStringEnum &getStatsLoggerEntry() { return statsLogger_; }

	const char * getServerFileLogger() { return serverFileLogger_.getValue().c_str(); }
	XMLEntryStringEnum &getServerFileLoggerEntry() { return serverFileLogger_; }

	const char * getBotNamePrefix() { return botNamePrefix_.getValue().c_str(); }
	XMLEntryString &getBotNamePrefixEntry() { return botNamePrefix_; }

	bool getGiveAllWeapons() { return giveAllWeapons_.getValue(); }
	XMLEntryBool &getGiveAllWeaponsEntry() { return giveAllWeapons_; }

	bool getCycleMaps() { return cycleMaps_.getValue(); }
	XMLEntryBool &getCycleMapsEntry() { return cycleMaps_; }

	int getResignMode() { return resignMode_.getValue(); }
	XMLEntryInt &getResignModeEntry() { return resignMode_; }

	MovementRestrictionType getMovementRestriction() { return (MovementRestrictionType) movementRestriction_.getValue(); } 
	XMLEntryEnum &getMovementRestrictionEntry() { return movementRestriction_; } 

	bool getRandomizeBotNames() { return randomizeBotNames_.getValue(); }
	XMLEntryBool &getRandomizeBotNamesEntry() { return randomizeBotNames_; }

	int getPortNo() { return portNo_.getValue(); }
	XMLEntryInt &getPortNoEntry() { return portNo_; }

	int getManagementPortNo() { return managementPortNo_.getValue(); }
	XMLEntryInt &getManagementPortNoEntry() { return managementPortNo_; }

	const char * getMod() { return mod_.getValue().c_str(); }
	XMLEntryString &getModEntry() { return mod_; }

	const char * getMOTD() { return motd_.getValue().c_str(); }
	XMLEntryString &getMOTDEntry() { return motd_; }

	int getModDownloadSpeed() { return modDownloadSpeed_.getValue(); }
	XMLEntryInt &getModDownloadSpeedEntry() { return modDownloadSpeed_; }

	int getMaxAvatarSize() { return maxAvatarSize_.getValue(); }
	XMLEntryInt &getMaxAvatarSizeEntry() { return maxAvatarSize_; }

	const char * getServerName() { return serverName_.getValue().c_str(); }
	XMLEntryString &getServerNameEntry() { return serverName_; }

	const char * getServerPassword() { return serverPassword_.getValue().c_str(); }
	XMLEntryString &getServerPasswordEntry() { return serverPassword_; }

	XMLEntryString &getPlayerType(int no) { DIALOG_ASSERT(no<24); return *playerType_[no]; }

	const char * getPublishAddress() { return publishAddress_.getValue().c_str(); }
	XMLEntryString &getPublishAddressEntry() { return publishAddress_; }

	bool getAllowSameIP() { return allowSameIP_.getValue(); }
	XMLEntryBool &getAllowSameIPEntry() { return allowSameIP_; }

	bool getAllowSameUniqueId() { return allowSameUniqueId_.getValue(); }
	XMLEntryBool &getAllowSameUniqueIdEntry() { return allowSameUniqueId_; }

	bool getPublishServer() { return publishServer_.getValue(); }
	XMLEntryBool &getPublishServerEntry() { return publishServer_; }

	bool getUseUPnP() { return useUPnP_.getValue(); }
	XMLEntryBool &getUseUPnPEntry() { return useUPnP_; }

	bool getUseUPnPLogging() { return useUPnPLogging_.getValue(); }
	XMLEntryBool &getUseUPnPLoggingEntry() { return useUPnPLogging_; }

	bool getResidualPlayers() { return residualPlayers_.getValue(); }
	XMLEntryBool &getResidualPlayersEntry() { return residualPlayers_; }

	bool getDelayedDefenseActivation() { return delayedDefenseActivation_.getValue(); }
	XMLEntryBool &getDelayedDefenseActivationEntry() { return delayedDefenseActivation_; }

	bool getAutoSendSyncCheck() { return autoSendSyncCheck_.getValue(); }
	XMLEntryBool &getAutoSendSyncCheckEntry() { return autoSendSyncCheck_; }

	bool getWaitForShotsBeforeShowingScore() { return waitForShotsBeforeShowingScore_.getValue(); }
	XMLEntryBool &getWaitForShotsBeforeShowingScoreEntry() { return waitForShotsBeforeShowingScore_; }

	bool getActionSyncCheck() { return actionSyncCheck_.getValue(); }
	XMLEntryBool &getActionSyncCheckEntry() { return actionSyncCheck_; }

	bool getActionRandomSyncCheck() { return actionRandomSyncCheck_.getValue(); }
	XMLEntryBool &getActionRandomSyncCheckEntry() { return actionRandomSyncCheck_; }

	bool getActionMovementSyncCheck() { return actionMovementSyncCheck_.getValue(); }
	XMLEntryBool &getActionMovementSyncCheckEntry() { return actionMovementSyncCheck_; }

	bool getActionCollisionSyncCheck() { return actionCollisionSyncCheck_.getValue(); }
	XMLEntryBool &getActionCollisionSyncCheckEntry() { return actionCollisionSyncCheck_; }

	bool getTargetPlacementSyncCheck() { return targetPlacementSyncCheck_.getValue(); }
	XMLEntryBool &getTargetPlacementSyncCheckEntry() { return targetPlacementSyncCheck_; }

	bool getWeaponSyncCheck() { return weaponSyncCheck_.getValue(); }
	XMLEntryBool &getWeaponSyncCheckEntry() { return weaponSyncCheck_; }

	bool getAccessoryNameSyncCheck() { return accessoryNameSyncCheck_.getValue(); }
	XMLEntryBool &getAccessoryNameSyncCheckEntry() { return accessoryNameSyncCheck_; }

	const char *getAuthHandler() { return authHandler_.getValue().c_str(); }
	XMLEntryStringEnum &getAuthHandlerEntry() { return authHandler_; }

	const char *getAuthHandlerParam1() { return authHandlerParam1_.getValue().c_str(); }
	XMLEntryString &getAuthHandlerParam1Entry() { return authHandlerParam1_; }

	const char *getAuthHandlerParam2() { return authHandlerParam2_.getValue().c_str(); }
	XMLEntryString &getAuthHandlerParam2Entry() { return authHandlerParam2_; }

	bool getRegisteredUserNames() { return registeredUserNames_.getValue(); }
	XMLEntryBool &getRegisteredUserNamesEntry() { return registeredUserNames_; }

	bool getAllowMultiLingualChat() { return allowMultiLingualChat_.getValue(); }
	XMLEntryBool &getAllowMultiLingualChatEntry() { return allowMultiLingualChat_; }

	bool getAllowMultiLingualNames() { return allowMultiLingualNames_.getValue(); }
	XMLEntryBool &getAllowMultiLingualNamesEntry() { return allowMultiLingualNames_; }

	bool getDebugFeatures() { return debugFeatures_.getValue(); }
	XMLEntryBool &getDebugFeaturesEntry() { return debugFeatures_; }
protected:
	std::list<XMLEntry *> playerTypeOptions_;

	XMLEntryBoundedInt weaponSpeed_;
	XMLEntryBoundedInt startArmsLevel_;
	XMLEntryBoundedInt endArmsLevel_;
	XMLEntryBoundedInt shotTime_;
	XMLEntryBoundedInt aiShotTime_;
	XMLEntryBoundedInt startTime_;
	XMLEntryBoundedInt buyingTime_;
	XMLEntryBoundedInt removeTime_;
	XMLEntryBoundedInt roundTime_;
	XMLEntryBoundedInt roundScoreTime_;
	XMLEntryBoundedInt scoreTime_;
	XMLEntryBoundedInt idleCycleTime_;
	XMLEntryBoundedInt allowedMissedMoves_;
	XMLEntryBoundedInt numberOfRounds_;
	XMLEntryBoundedInt maxRoundTurns_;
	XMLEntryBoundedInt maxNumberWeapons_;
	XMLEntryBoundedInt gravity_;
	XMLEntryBoundedInt minFallingDistance_;
	XMLEntryBool tankFallingDamage_;
	XMLEntryBoundedInt maxClimbingDistance_;
	XMLEntryBoundedInt playerLives_;
	XMLEntryBoundedInt teams_;
	XMLEntryBoundedInt numberOfPlayers_;
	XMLEntryBoundedInt numberOfMinPlayers_;
	XMLEntryBoundedInt removeBotsAtPlayers_;
	XMLEntryBoundedInt computersDeathTalk_;
	XMLEntryBoundedInt computersAttackTalk_;
	XMLEntryBoundedInt moneyBuyOnRound_;
	XMLEntryBoundedInt moneyWonForRound_;
	XMLEntryBoundedInt moneyWonForLives_;
	XMLEntryBoundedInt moneyPerKillPoint_;
	XMLEntryBoundedInt moneyPerMultiKillPoint_;
	XMLEntryBoundedInt moneyPerAssistPoint_;
	XMLEntryBoundedInt moneyPerHitPoint_;
	XMLEntryBoundedInt moneyPerRound_;
	XMLEntryBool moneyPerHealthPoint_;
	XMLEntryBoundedInt scorePerMoney_;
	XMLEntryBoundedInt scorePerAssist_;
	XMLEntryBoundedInt scorePerKill_;
	XMLEntryBoundedInt scorePerResign_;
	XMLEntryBoundedInt scorePerSpectate_;
	XMLEntryBoundedInt scoreWonForRound_;
	XMLEntryBoundedInt scoreWonForLives_;
	XMLEntryInt maxLandscapeSize_;
	XMLEntryInt freeMarketAdjustment_;
	XMLEntryInt freeMarketLimits_;
	XMLEntryBool freeMarketTracksBotPurchases_;
	XMLEntryInt minimumLandHeight_;
	XMLEntryBoundedInt skillForRound_;
	XMLEntryBoundedInt skillForMatch_;
	XMLEntryBoundedInt skillForResign_;
	XMLEntryBoundedInt maxSkillLost_;
	XMLEntryBoundedInt maxSkillGained_;
	XMLEntryBoundedInt skillForSelfKill_;
	XMLEntryBoundedInt skillForTeamKill_;
	XMLEntryBoundedInt moneyStarting_;
	XMLEntryEnum teamBallance_;
	XMLEntryBoundedInt moneyInterest_;
	XMLEntryBool limitPowerByHealth_;
	XMLEntryString tutorial_;
	XMLEntryBool cycleMaps_;
	XMLEntryEnum resignMode_;
	XMLEntryEnum movementRestriction_;
	XMLEntryEnum turnType_;
	XMLEntryEnum windForce_;
	XMLEntryEnum windType_;
	XMLEntryEnum wallType_;
	XMLEntryEnum weapScale_;
	XMLEntryBoundedInt modDownloadSpeed_;
	XMLEntryInt maxAvatarSize_;
	XMLEntryString mod_;
	XMLEntryString motd_;
	XMLEntryStringEnum economy_;
	XMLEntryString landscapes_;
	XMLEntryStringEnum statsLogger_;
	XMLEntryStringEnum serverFileLogger_;
	XMLEntryBool waitForShotsBeforeShowingScore_;

	// Server only options
	XMLEntryString botNamePrefix_;
	XMLEntryBool actionSyncCheck_;
	XMLEntryBool actionMovementSyncCheck_;
	XMLEntryBool actionRandomSyncCheck_;
	XMLEntryBool actionCollisionSyncCheck_;
	XMLEntryBool targetPlacementSyncCheck_;
	XMLEntryBool weaponSyncCheck_;
	XMLEntryBool accessoryNameSyncCheck_;
	XMLEntryBool autoSendSyncCheck_;
	XMLEntryBool residualPlayers_;
	XMLEntryBool delayedDefenseActivation_;
	XMLEntryBool randomizeBotNames_;
	XMLEntryBool giveAllWeapons_;
	XMLEntryBool registeredUserNames_;
	XMLEntryBool allowMultiLingualChat_;
	XMLEntryBool allowMultiLingualNames_;
	XMLEntryStringEnum authHandler_;
	XMLEntryString authHandlerParam1_;
	XMLEntryString authHandlerParam2_;
	XMLEntryString serverName_;
	XMLEntryString *playerType_[24];
	XMLEntryString serverPassword_;
	XMLEntryInt portNo_;
	XMLEntryInt managementPortNo_;
	XMLEntryString publishAddress_;
	XMLEntryBool publishServer_;
	XMLEntryBool useUPnP_;
	XMLEntryBool useUPnPLogging_;
	XMLEntryBool allowSameIP_;
	XMLEntryBool allowSameUniqueId_;
	XMLEntryBool debugFeatures_;
};

#endif
