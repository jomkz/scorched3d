////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011
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

#include <common/StatsLoggerFile.h>
#include <common/OptionsScorched.h>
#include <common/LoggerI.h>
#include <common/Defines.h>
#include <server/ScorchedServer.h>
#include <weapons/Accessory.h>
#include <weapons/Shield.h>
#include <stdlib.h>

StatsLoggerFile::StatsLoggerFile() : statsLogger_(0)
{

}

StatsLoggerFile::~StatsLoggerFile()
{
}

int StatsLoggerFile::getKillCount(const char *uniqueId)
{
	return 0;
}

void StatsLoggerFile::createLogger()
{
	if (!statsLogger_)
	{
		char buffer[256];
		snprintf(buffer, 256, "StatsLog-%i-", 
			ScorchedServer::instance()->getOptionsGame().getPortNo());
		statsLogger_ = new FileLogger(buffer);
	}
}

void StatsLoggerFile::gameStart(std::list<Tank *> &tanks)
{
	createLogger();
	if (!statsLogger_) return;

	time_t theTime = time(0);
	char *time = ctime(&theTime); 
	char *nl = strchr(time, '\n'); 
	if (nl) *nl = '\0';

	LoggerInfo info("startgame", time);
	statsLogger_->logMessage(info);
}

void StatsLoggerFile::roundStart(std::list<Tank *> &tanks)
{
	createLogger();
	if (!statsLogger_) return;

	time_t theTime = time(0);
	char *time = ctime(&theTime); 
	char *nl = strchr(time, '\n'); 
	if (nl) *nl = '\0';

	LoggerInfo info("startround", time);
	statsLogger_->logMessage(info);
}

void StatsLoggerFile::updateStats(Tank *tank)
{
}

std::string StatsLoggerFile::allocateId() 
{ 
	static char buffer[128];
	snprintf(buffer, 128, "%i-%i-%i", rand(), rand(), rand());
	return buffer;
}

unsigned int StatsLoggerFile::getStatsId(const char *uniqueId)
{
	return 0;
}

std::string StatsLoggerFile::getTopRanks()
{ 
	return ""; 
}

std::string StatsLoggerFile::getPlayerInfo(const char *player)
{
	return "";
}

void StatsLoggerFile::combinePlayers(unsigned int player1, unsigned int player2)
{
}

void StatsLoggerFile::periodicUpdate()
{
}

std::list<std::string> StatsLoggerFile::getAliases(const char *unqiueId)
{
	std::list<std::string> result;
	return result;
}

std::list<std::string> StatsLoggerFile::getIpAliases(const char *unqiueId)
{
	std::list<std::string> result;
	return result;
}

void StatsLoggerFile::tankFired(Tank *firedTank, Weapon *weapon)
{
	createLogger();
	if (!statsLogger_) return;

	time_t theTime = time(0);
	char *time = ctime(&theTime); 
	char *nl = strchr(time, '\n'); 
	if (nl) *nl = '\0';

	char message[1024];
	snprintf(message, 1024, "fired \"%s\" [%s] \"%s\"", 
		firedTank->getCStrName().c_str(), 
		firedTank->getUniqueId(),
		weapon->getParent()->getName());

	LoggerInfo info(message, time);
	statsLogger_->logMessage(info);
}

void StatsLoggerFile::tankResigned(Tank *resignedTank)
{
	createLogger();
	if (!statsLogger_) return;

	time_t theTime = time(0);
	char *time = ctime(&theTime); 
	char *nl = strchr(time, '\n'); 
	if (nl) *nl = '\0';

	char message[1024];
	snprintf(message, 1024, "resigined \"%s\" [%s]", 
		resignedTank->getCStrName().c_str(), 
		resignedTank->getUniqueId());
	LoggerInfo info(message, time);
	statsLogger_->logMessage(info);
}

void StatsLoggerFile::tankJoined(Tank *tank)
{
	createLogger();
	if (!statsLogger_) return;

	time_t theTime = time(0);
	char *time = ctime(&theTime); 
	char *nl = strchr(time, '\n'); 
	if (nl) *nl = '\0';

	char message[1024];
	snprintf(message, 1024, "joined \"%s\" [%s]", 
		tank->getCStrName().c_str(), 
		tank->getUniqueId());
	LoggerInfo info(message, time);
	statsLogger_->logMessage(info);
}

void StatsLoggerFile::tankConnected(Tank *tank)
{
	createLogger();
	if (!statsLogger_) return;

	time_t theTime = time(0);
	char *time = ctime(&theTime); 
	char *nl = strchr(time, '\n'); 
	if (nl) *nl = '\0';

	char message[1024];
	snprintf(message, 1024, "connected \"%s\" [%s]", 
		tank->getCStrName().c_str(), 
		tank->getUniqueId());
	LoggerInfo info(message, time);
	statsLogger_->logMessage(info);
}

void StatsLoggerFile::tankDisconnected(Tank *tank)
{
	createLogger();
	if (!statsLogger_) return;

	time_t theTime = time(0);
	char *time = ctime(&theTime); 
	char *nl = strchr(time, '\n'); 
	if (nl) *nl = '\0';

	char message[1024];
	snprintf(message, 1024, "disconnected \"%s\" [%s]", 
		tank->getCStrName().c_str(), 
		tank->getUniqueId());
	LoggerInfo info(message, time);
	statsLogger_->logMessage(info);
}

void StatsLoggerFile::tankKilled(Tank *firedTank, Tank *deadTank, Weapon *weapon)
{
	createLogger();
	if (!statsLogger_) return;

	time_t theTime = time(0);
	char *time = ctime(&theTime); 
	char *nl = strchr(time, '\n'); 
	if (nl) *nl = '\0';

	char message[1024];
	snprintf(message, 1024, "killed \"%s\" [%s] \"%s\" [%s] \"%s\"", 
		firedTank->getCStrName().c_str(), firedTank->getUniqueId(),
		deadTank->getCStrName().c_str(), deadTank->getUniqueId(),
		weapon->getParent()->getName());
	LoggerInfo info(message, time);
	statsLogger_->logMessage(info);
}

void StatsLoggerFile::tankTeamKilled(Tank *firedTank, Tank *deadTank, Weapon *weapon)
{
	createLogger();
	if (!statsLogger_) return;

	time_t theTime = time(0);
	char *time = ctime(&theTime); 
	char *nl = strchr(time, '\n'); 
	if (nl) *nl = '\0';

	char message[1024];
	snprintf(message, 1024, "teamkilled \"%s\" [%s] \"%s\" [%s] \"%s\"", 
		firedTank->getCStrName().c_str(), firedTank->getUniqueId(),
		deadTank->getCStrName().c_str(), deadTank->getUniqueId(),
		weapon->getParent()->getName());
	LoggerInfo info(message, time);
	statsLogger_->logMessage(info);
}

void StatsLoggerFile::tankSelfKilled(Tank *firedTank, Weapon *weapon)
{
	createLogger();
	if (!statsLogger_) return;

	time_t theTime = time(0);
	char *time = ctime(&theTime); 
	char *nl = strchr(time, '\n'); 
	if (nl) *nl = '\0';

	char message[1024];
	snprintf(message, 1024, "selfkilled \"%s\" [%s] \"%s\"", 
		firedTank->getCStrName().c_str(), 
		firedTank->getUniqueId(),
		weapon->getParent()->getName());
	LoggerInfo info(message, time);
	statsLogger_->logMessage(info);
}

void StatsLoggerFile::tankWon(Tank *tank)
{
	createLogger();
	if (!statsLogger_) return;

	time_t theTime = time(0);
	char *time = ctime(&theTime); 
	char *nl = strchr(time, '\n'); 
	if (nl) *nl = '\0';

	char message[1024];
	snprintf(message, 1024, "won \"%s\" [%s]", 
		tank->getCStrName().c_str(), tank->getUniqueId());
	LoggerInfo info(message, time);
	statsLogger_->logMessage(info);
}

void StatsLoggerFile::tankOverallWinner(Tank *tank)
{
	createLogger();
	if (!statsLogger_) return;

	time_t theTime = time(0);
	char *time = ctime(&theTime); 
	char *nl = strchr(time, '\n'); 
	if (nl) *nl = '\0';

	char message[1024];
	snprintf(message, 1024, "overallwinner \"%s\" [%s]", 
		tank->getCStrName().c_str(), tank->getUniqueId());
	LoggerInfo info(message, time);
	statsLogger_->logMessage(info);
}

void StatsLoggerFile::weaponFired(Weapon *weapon, bool deathAni)
{
	createLogger();
	if (!statsLogger_) return;

	time_t theTime = time(0);
	char *time = ctime(&theTime); 
	char *nl = strchr(time, '\n'); 
	if (nl) *nl = '\0';

	char message[1024];
	snprintf(message, 1024, "weaponfired \"%s\"", weapon->getParent()->getName());
	LoggerInfo info(message, time);
	statsLogger_->logMessage(info);
}

void StatsLoggerFile::weaponKilled(Weapon *weapon, bool deathAni)
{
	createLogger();
	if (!statsLogger_) return;

	time_t theTime = time(0);
	char *time = ctime(&theTime); 
	char *nl = strchr(time, '\n'); 
	if (nl) *nl = '\0';

	char message[1024];
	snprintf(message, 1024, "weaponkilled \"%s\"", weapon->getParent()->getName());
	LoggerInfo info(message, time);
	statsLogger_->logMessage(info);
}
